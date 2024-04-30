#pragma once
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "strmiids.lib")

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <memory>

#include "Library/Timer.h"

class Video {
public:
	Video() = default;
	Video(ID3D11Device* device, const wchar_t* filePath) { LoadFile(device, filePath); }

	void LoadFile(ID3D11Device* device, const wchar_t* filePath);
	void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize,
		DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize);
	void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize, DirectX::XMFLOAT4 color = { 1,1,1,1 }) {
		Render(deviceContext, dPos, dSize, color, 0, { 0,0 }, _videoSize);
	}
	void LoadFrame(ID3D11DeviceContext* deviceContext);
	bool Play(bool loop = false);
	void Pause();
	void Stop();
	void SeekPosition(LONGLONG seekTime);

	bool Empty() { return _isEmpty; }

private:
	void LoadAudio();

	void InitializeVideo(ID3D11Device* device);
public:

private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IMFSourceReader> _sourceReader;
	ComPtr<IMFTransform> _transform;

	ComPtr<IMFMediaType> _audioType;
	ComPtr<IMFMediaType> _videoTypeNV12;
	ComPtr<IMFMediaType> _videoTypeARGB;

	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11ShaderResourceView> _textureView;

	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3D11PixelShader> _pixelShader;

	DirectX::XMFLOAT2 _videoSize;
	LONGLONG _timeStamp;
	float _framerate;

	float _pauseTime = 0;
	bool _isPlay = false;
	bool _isLoop = false;
	bool _isEmpty = true;

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};


class VideoManager {
public:
	static VideoManager& Instance() {
		static VideoManager instance;
		return instance;
	}
	~VideoManager() { MFShutdown(); }

	void Update();
	void Draw(size_t num, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize,
		DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize);
	void Draw(size_t num, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize, DirectX::XMFLOAT4 color = { 1,1,1,1 });

	bool Play(size_t num, bool loop = false);
	void Pause(size_t num);
	void Stop(size_t num);
	void SeekPosition(size_t num, LONGLONG time/*10��s(1/10000000)�ł����*/);

	void LoadFile(size_t num, const wchar_t* filePath);
	void Clear(size_t num = 0);

private:
	VideoManager(){
		MFStartup(MF_VERSION);
		//_videos.resize(10);
	}

	std::vector<Video> _videos;
};