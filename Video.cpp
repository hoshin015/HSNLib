#include "Video.h"
#include "Library/Graphics/Graphics.h"
#include "Library/Graphics/Shader.h"
#include "Library/ErrorLogger.h"
using namespace DirectX;

#define CLIP(x) do{if(x < 0){x = 0;} else if(x > 255){x = 255;}} while(0)
#define CONVERT_R(Y, V)    ((298 * (Y - 16) + 409 * (V - 128) + 128) >> 8)
#define CONVERT_G(Y, U, V) ((298 * (Y - 16) - 100 * (U - 128) - 208 * (V - 128) + 128) >> 8)
#define CONVERT_B(Y, U)    ((298 * (Y - 16) + 516 * (U - 128) + 128) >> 8)

//このまま使うと変換だけで20msかかる(つまりFPSが50以下になる)
//NV12(YUV 4:2:0)からRGBへ変換
//計算はめんどいので下のサイトからコピー
//https://www.klab.com/jp/blog/tech/2016/1054828175.html
void NV12ToRGB(BYTE* rgbBufferOut, BYTE* yuvBuffer, int width, int height) {
	BYTE* uvStart = yuvBuffer + width * height + 15360/3;
	BYTE y[2] = { 0, 0 };
	BYTE u = 0;
	BYTE v = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	for (int rowCnt = 0; rowCnt < height; rowCnt++) {
		for (int colCnt = 0; colCnt < width; colCnt += 2) {
			u = *(uvStart + colCnt + 0);
			v = *(uvStart + colCnt + 1);

			for (int cnt = 0; cnt < 2; cnt++) {
				y[cnt] = yuvBuffer[rowCnt * width + colCnt + cnt];

				r = CONVERT_R(y[cnt], v);
				CLIP(r);
				g = CONVERT_G(y[cnt], u, v);
				CLIP(g);
				b = CONVERT_B(y[cnt], u);
				CLIP(b);
				rgbBufferOut[(rowCnt * width + colCnt + cnt) * 4 + 0] = static_cast<BYTE>(b);
				rgbBufferOut[(rowCnt * width + colCnt + cnt) * 4 + 1] = static_cast<BYTE>(g);
				rgbBufferOut[(rowCnt * width + colCnt + cnt) * 4 + 2] = static_cast<BYTE>(r);
				rgbBufferOut[(rowCnt * width + colCnt + cnt) * 4 + 3] = 255;
			}
		}

		uvStart += width * (rowCnt % 2);
	}
}

void moveUV(BYTE* yuvBuffer, int bufferSize, int width, int height) {
	size_t uvStart;
	int yEnd = width * height;
	for (uvStart = yEnd; yuvBuffer[uvStart] != 0; uvStart += width);
	if (uvStart != yEnd)memmove(&yuvBuffer[width * height], &yuvBuffer[uvStart], bufferSize - uvStart);
}

inline void  Rotate(XMFLOAT2& pos, XMFLOAT2 cPos, float cos, float sin) {
	pos.x -= cPos.x;
	pos.y -= cPos.y;

	float tx { pos.x }, ty { pos.y };
	pos.x = cos * tx + -sin * ty;
	pos.y = sin * tx + cos * ty;

	pos.x += cPos.x;
	pos.y += cPos.y;
};

void Video::LoadFile(ID3D11Device* device, const wchar_t* filePath) {
	HRESULT hr = {};
	_isEmpty = false;

	hr = MFCreateSourceReaderFromURL(filePath, NULL, _sourceReader.GetAddressOf());

	//音声読み込み
	//今回は作らない
	LoadAudio();

	//動画読み込み初期設定
	InitializeVideo(device);
}

void Video::Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize) {
	D3D11_VIEWPORT viewport {};
	UINT numViewports { 1 };
	deviceContext->RSGetViewports(&numViewports, &viewport);

	// left top
	XMFLOAT2 lt = dPos;
	// right top
	XMFLOAT2 rt = { dPos.x + dSize.x, dPos.y };
	// left bottom
	XMFLOAT2 lb = { dPos.x, dPos.y + dSize.y };
	// right bottom
	XMFLOAT2 rb = { dPos.x + dSize.x, dPos.y + dSize.y };

	//--- < 頂点回転処理 > ---

	// 回転の中心を矩形の中心点にした場合
	XMFLOAT2 cPos = { rb.x * 0.5f,rb.y * 0.5f };

	float cos { cosf(DirectX::XMConvertToRadians(angle)) };
	float sin { sinf(DirectX::XMConvertToRadians(angle)) };

	Rotate(lt, cPos, cos, sin);
	Rotate(rt, cPos, cos, sin);
	Rotate(lb, cPos, cos, sin);
	Rotate(rb, cPos, cos, sin);

	//--- < スクリーン座標系からNDCへ座標変換 > ---
	lt.x = 2.0f * lt.x / viewport.Width - 1.0f;
	lt.y = 1.0f - 2.0f * lt.y / viewport.Height;
	rt.x = 2.0f * rt.x / viewport.Width - 1.0f;
	rt.y = 1.0f - 2.0f * rt.y / viewport.Height;
	lb.x = 2.0f * lb.x / viewport.Width - 1.0f;
	lb.y = 1.0f - 2.0f * lb.y / viewport.Height;
	rb.x = 2.0f * rb.x / viewport.Width - 1.0f;
	rb.y = 1.0f - 2.0f * rb.y / viewport.Height;


	//--- < テクセル座標系からUV座標系への変換 > ---
	float u0 { sPos.x / _videoSize.x };
	float v0 { sPos.y / _videoSize.y };
	float u1 { (sPos.x + sSize.x) / _videoSize.x };
	float v1 { (sPos.y + sSize.y) / _videoSize.y };

	//--- < 計算結果で頂点バッファオブジェクトを更新する > ---
	HRESULT hr { S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource {};
	hr = deviceContext->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	Vertex* vertices { reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr) {
		vertices[0].position = { lt.x ,lt.y, 0 };
		vertices[1].position = { rt.x ,rt.y, 0 };
		vertices[2].position = { lb.x ,lb.y, 0 };
		vertices[3].position = { rb.x ,rb.y, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

		vertices[0].texcoord = { u0,v0 };
		vertices[1].texcoord = { u1,v0 };
		vertices[2].texcoord = { u0,v1 };
		vertices[3].texcoord = { u1,v1 };
	}
	deviceContext->Unmap(_vertexBuffer.Get(), 0);

	//--- < 頂点バッファーのバインド > ---
	UINT stride { sizeof(Vertex) };
	UINT offset { 0 };
	deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < プリミティブタイプおよにデータ順序に関する情報のバインド > ---
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//--- < 入力レイアウトオブジェクトのバインド > ---
	deviceContext->IASetInputLayout(_inputLayout.Get());

	//--- < シェーダーのバインド > ---
	deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);

	//--- < シェーダーリソースのバインド > ---
	deviceContext->PSSetShaderResources(0, 1, _textureView.GetAddressOf());

	//--- < プリミティブの描画 > ---
	deviceContext->Draw(4, 0);
}

void Video::LoadAudio() {}

//mp4などのデータをTextureとして読み込むための下準備
void Video::InitializeVideo(ID3D11Device* device) {
	//1280x720以外の動画を読み込むとUVの座標がずれる
	//1920*1080だと15360BYTE分

	//ソースの情報を取得
	HRESULT hr = {};
	ComPtr<IMFMediaType> nativeType;
	GUID subtype;
	UINT32 avgBitrate;
	UINT32 interlace;
	UINT32 frame, ratio;
	UINT32 width, height;
	UINT32 pixelX, pixelY;
	//UINT32 allSamplesIndependent;

	hr = _sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nativeType.GetAddressOf());
	hr = nativeType->GetGUID(MF_MT_SUBTYPE, &subtype);
	//hr = nativeType->GetUINT32(MF_MT_SAMPLE_SIZE, &sampleSize);
	hr = nativeType->GetUINT32(MF_MT_INTERLACE_MODE, &interlace);
	hr = nativeType->GetUINT32(MF_MT_AVG_BITRATE, &avgBitrate);
	hr = MFGetAttributeRatio(static_cast<IMFAttributes*>(nativeType.Get()), MF_MT_FRAME_RATE, &frame, &ratio);
	hr = MFGetAttributeSize(static_cast<IMFAttributes*>(nativeType.Get()), MF_MT_FRAME_SIZE, &width, &height);
	hr = MFGetAttributeRatio(static_cast<IMFAttributes*>(nativeType.Get()), MF_MT_PIXEL_ASPECT_RATIO, &pixelX, &pixelY);

	_videoSize = { static_cast<float>(width),static_cast<float>(height) };
	_framerate = static_cast<float>(frame) / ratio;

	MFCreateMediaType(_videoTypeNV12.GetAddressOf());
	nativeType->CopyAllItems(_videoTypeNV12.Get());
	//hr = _videoTypeNV12->SetUINT32(MF_MT_AVG_BITRATE, avgBitrate);
	//hr = _videoTypeNV12->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
	//hr = _videoTypeNV12->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);
	//hr = _videoTypeNV12->SetUINT32(MF_MT_SAMPLE_SIZE, 3133440);
	//hr = _videoTypeNV12->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	//hr = _videoTypeNV12->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_Wide);
	//hr = _videoTypeNV12->SetUINT32(MF_MT_VIDEO_CHROMA_SITING, MFVideoChromaSubsampling_MPEG2);

	//MediaTypeの設定(下は必須)
	hr = _videoTypeNV12->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	hr = _videoTypeNV12->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
	hr = _videoTypeNV12->SetUINT32(MF_MT_DEFAULT_STRIDE, width);
	hr = _videoTypeNV12->SetUINT32(MF_MT_AVG_BITRATE, avgBitrate);
	hr = MFSetAttributeRatio((IMFAttributes*)_videoTypeNV12.Get(), MF_MT_FRAME_RATE, frame, ratio);
	hr = MFSetAttributeRatio((IMFAttributes*)_videoTypeNV12.Get(), MF_MT_FRAME_SIZE, width, height);
	hr = MFSetAttributeRatio((IMFAttributes*)_videoTypeNV12.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	hr = _sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, _videoTypeNV12.Get());

	MFCreateMediaType(_videoTypeARGB.GetAddressOf());
	nativeType->CopyAllItems(_videoTypeARGB.Get());
	//hr = _videoTypeARGB->SetUINT32(MF_MT_AVG_BITRATE, avgBitrate);
	//hr = _videoTypeARGB->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
	//hr = _videoTypeARGB->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);
	//hr = _videoTypeARGB->SetUINT32(MF_MT_SAMPLE_SIZE, 3133440);
	//hr = _videoTypeARGB->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	//hr = _videoTypeARGB->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_Wide);
	//hr = _videoTypeARGB->SetUINT32(MF_MT_VIDEO_CHROMA_SITING, MFVideoChromaSubsampling_MPEG2);

	//MediaTypeの設定(下は必須)
	hr = _videoTypeARGB->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	hr = _videoTypeARGB->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32);
	hr = _videoTypeARGB->SetUINT32(MF_MT_DEFAULT_STRIDE, width * 4);//値を入れないと何故か反転するくせに最大値入れなければ何でもいい謎
	hr = _videoTypeARGB->SetUINT32(MF_MT_AVG_BITRATE, avgBitrate);
	hr = MFSetAttributeRatio((IMFAttributes*)_videoTypeARGB.Get(), MF_MT_FRAME_RATE, frame, ratio);
	hr = MFSetAttributeRatio((IMFAttributes*)_videoTypeARGB.Get(), MF_MT_FRAME_SIZE, width, height);
	hr = MFSetAttributeRatio((IMFAttributes*)_videoTypeARGB.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);

	//MFT
	IMFActivate** activates = nullptr;
	UINT32 num_actuvates = 0;
	MFTEnumEx(MFT_CATEGORY_VIDEO_PROCESSOR, MFT_ENUM_FLAG_ALL, nullptr, nullptr, &activates, &num_actuvates);

	activates[0]->ActivateObject(_uuidof(IMFTransform), (void**)_transform.GetAddressOf());

#if _DEBUG
	LPWSTR friendlyName;
	UINT32 nameLength;
	activates[0]->GetAllocatedString(MFT_FRIENDLY_NAME_Attribute, &friendlyName, &nameLength);
	OutputDebugString(friendlyName);
	OutputDebugString(L"\n");
#endif // _DEBUG

	activates[0]->Release();
	CoTaskMemFree(activates);

	hr = _transform->SetInputType(0, _videoTypeNV12.Get(), 0);
	hr = _transform->SetOutputType(0, _videoTypeARGB.Get(), 0);

	//テクスチャの作成
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_B8G8R8A8_UNORM, _videoSize.x, _videoSize.y);
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MipLevels = 1;

	std::vector<UINT32> dummyData;
	dummyData.resize(_videoSize.x * _videoSize.y, 0xffffffff);
	D3D11_SUBRESOURCE_DATA texSubresourceData = {};
	texSubresourceData.pSysMem = dummyData.data();
	texSubresourceData.SysMemPitch = _videoSize.x * sizeof(UINT32);

	hr = device->CreateTexture2D(&textureDesc, &texSubresourceData, _texture.GetAddressOf());

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 1);
	hr = device->CreateShaderResourceView(_texture.Get(), &srvDesc, _textureView.GetAddressOf());

	//頂点
	Vertex vertices[] =
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},
		{{  0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 1, 0 }},
		{{ -0.5f, -0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 1 }},
		{{  0.5f, -0.5f, 0 }, { 1, 1, 1, 1 }, { 1, 1 }},
	};

	//頂点バッファの作成
	D3D11_BUFFER_DESC bufferDesc {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData {};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, _vertexBuffer.GetAddressOf());

	const char* shaderFile = "./Data/Shader/Sprite_VS.cso";

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(shaderFile, _vertexShader.GetAddressOf(), _inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

	shaderFile = "./Data/Shader/Sprite_PS.cso";
	CreatePsFromCso(shaderFile, _pixelShader.GetAddressOf());
}

void Video::LoadFrame(ID3D11DeviceContext* deviceContext) {
	if (!_isPlay) return;
	if ((Timer::Instance().TimeStamp() - _pauseTime) * 10000000 < _timeStamp) return;

	ComPtr<IMFSample> sample;
	DWORD striamIndex, dwStreamFlags;
	HRESULT hr = {};

	//次のフレームのTextureを取得
	hr = _sourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &striamIndex, &dwStreamFlags, &_timeStamp, sample.GetAddressOf());
	if (!(dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)) {
		//YからUVまでのデータを移動する
		ComPtr<IMFMediaBuffer> yuvBuffer;
		{
			sample->GetBufferByIndex(0, yuvBuffer.GetAddressOf());

			BYTE* bBuffer;
			DWORD maxlen, currentlen;
			yuvBuffer->Lock(&bBuffer, &maxlen, &currentlen);

			moveUV(bBuffer, maxlen, _videoSize.x, _videoSize.y);

			yuvBuffer->Unlock();

			sample->RemoveAllBuffers();
			sample->AddBuffer(yuvBuffer.Get());
		}

		//IMFTransformはデータをインプットするときはしてアウトプットするときはアウトプットのみする
		//同時にやってしまうとインプットができない
		hr = _transform->ProcessInput(0, sample.Get(), 0);

		DWORD status;
		hr = _transform->GetOutputStatus(&status);
		if (status & MFT_OUTPUT_STATUS_SAMPLE_READY) {
			while (true) {
				//デコーダで処理される出力バッファの大きさを取得する
				ComPtr<IMFMediaBuffer> buffer;
				MFT_OUTPUT_STREAM_INFO streamInfo;
				ZeroMemory(&streamInfo, sizeof(MFT_OUTPUT_STREAM_INFO));
				hr = _transform->GetOutputStreamInfo(0, &streamInfo);
				hr = MFCreateMemoryBuffer(streamInfo.cbSize, buffer.GetAddressOf());

				//バッファを取得
				MFT_OUTPUT_DATA_BUFFER outBuffer;
				ZeroMemory(&outBuffer, sizeof(MFT_OUTPUT_DATA_BUFFER));
				hr = MFCreateSample(&outBuffer.pSample);
				outBuffer.pSample->AddBuffer(buffer.Get());
				hr = _transform->ProcessOutput(0, 1, &outBuffer, &status);

				//出力できるデータがなければループを抜ける
				if (FAILED(hr)) {
					outBuffer.pSample->RemoveAllBuffers();
					outBuffer.pSample->Release();
					break;
				}

#if 1
				//Texture2Dに書き込み
				BYTE* bBuffer;
				DWORD maxlen, currentlen;

				hr = buffer->Lock(&bBuffer, &maxlen, &currentlen);

				D3D11_MAPPED_SUBRESOURCE subresource;
				deviceContext->Map(_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
				memcpy(subresource.pData, bBuffer, subresource.DepthPitch);
				deviceContext->Unmap(_texture.Get(), 0);

				hr = buffer->Unlock();
#else
				ComPtr<IMFMediaBuffer> nv12Buffer;
				sample->GetBufferByIndex(0, nv12Buffer.GetAddressOf());

				BYTE* bBuffer;
				DWORD maxlen, currentlen;

				hr = nv12Buffer->Lock(&bBuffer, &maxlen, &currentlen);

				std::vector<BYTE> rgbBuffer(_videoSize.x * _videoSize.y * 4);
				NV12ToRGB(rgbBuffer.data(), bBuffer, _videoSize.x, _videoSize.y);

				D3D11_MAPPED_SUBRESOURCE subresource;
				deviceContext->Map(_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
				memcpy(subresource.pData, rgbBuffer.data(), subresource.DepthPitch);
				deviceContext->Unmap(_texture.Get(), 0);

				hr = nv12Buffer->Unlock();
#endif // 1

				//終了処理
				outBuffer.pSample->RemoveAllBuffers();
				outBuffer.pSample->Release();
				_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0);
				_transform->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0);
			}
		}
	}
	else {
		//再生場所を0に設定
		SeekPosition(0);

		if (_isLoop) _pauseTime = Timer::Instance().TimeStamp();
		else _isPlay = false;
	}
}

void Video::Play(bool loop) {
	_isPlay = true;
	_isLoop = loop;
	_pauseTime = Timer::Instance().TimeStamp() + _timeStamp == 0 ? 0 : _timeStamp * -0.1f;
}

void Video::Pause() {
	_isPlay = false;
}

inline void Video::Stop() {
	_isPlay = false;
	SeekPosition(0);
}

void Video::SeekPosition(LONGLONG seekTime) {
	_timeStamp = seekTime;
	PROPVARIANT time;
	PropVariantInit(&time);
	time.vt = VT_I8;
	time.lVal = seekTime;
	_sourceReader->SetCurrentPosition(GUID_NULL, time);
	PropVariantClear(&time);
}

void VideoManager::Update() {
	for (Video& video : _videos) {
		if (video.Empty()) return;
		video.LoadFrame(Graphics::Instance().deviceContext.Get());
	}
}

void VideoManager::Draw(size_t num, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize) {
	try {
		if(_videos.at(num).Empty()) ErrorLogger::Log("VideoManager: Video is not loaded");
		_videos.at(num).Render(Graphics::Instance().deviceContext.Get(), dPos, dSize, color, angle, sPos, sSize);
	}
	catch (std::out_of_range& ex) {
		ErrorLogger::Log("VideoManager: Out of range");
	}
}

void VideoManager::Draw(size_t num, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize) {
	try {
		if (_videos.at(num).Empty()) ErrorLogger::Log("VideoManager: Video is not loaded");
		_videos.at(num).Render(Graphics::Instance().deviceContext.Get(), dPos, dSize);
	}
	catch (std::out_of_range& ex) {
		ErrorLogger::Log("VideoManager: out of range");
	}
}

void VideoManager::Play(size_t num, bool loop) {
	try {
		if (_videos.at(num).Empty()) ErrorLogger::Log("VideoManager: Video is not loaded");
		_videos.at(num).Play(loop);
	}
	catch (std::out_of_range& ex) {
		ErrorLogger::Log("VideoManager: out of range");
	}
}

void VideoManager::Pause(size_t num) {
	try {
		if (_videos.at(num).Empty()) ErrorLogger::Log("VideoManager: Video is not loaded");
		_videos.at(num).Pause();
	}
	catch (std::out_of_range& ex) {
		ErrorLogger::Log("VideoManager: out of range");
	}
}

void VideoManager::Stop(size_t num) {
	try {
		if (_videos.at(num).Empty()) ErrorLogger::Log("VideoManager: Video is not loaded");
		_videos.at(num).Stop();
	}
	catch (std::out_of_range& ex) {
		ErrorLogger::Log("VideoManager: out of range");
	}
}

void VideoManager::SeekPosition(size_t num, LONGLONG time) {
	try {
		if (_videos.at(num).Empty()) ErrorLogger::Log("VideoManager: Video is not loaded");
		_videos.at(num).SeekPosition(time);
	}
	catch (std::out_of_range& ex) {
		ErrorLogger::Log("VideoManager: out of range");
	}
}

void VideoManager::LoadFile(size_t num, const wchar_t* filePath) {
	if (_videos.size() < num + 1) _videos.resize(num + 1);
	_videos.at(num).LoadFile(Graphics::Instance().device.Get(), filePath);
}

void VideoManager::Clear(size_t num) {
	if (num == 0) {
		_videos.clear();
		return;
	}

	_videos.erase(_videos.begin() + num);
	_videos.insert(_videos.begin() + num, Video());
}

