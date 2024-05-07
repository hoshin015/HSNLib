#include "MaskPrimitive.h"
#include <sstream>
#include "Library/Graphics/Graphics.h"
#include "Library/Graphics/Shader.h"
#include "Library/ErrorLogger.h"


inline void  Rotate(float& x, float& y, float cx, float cy, float cos, float sin) {
	x -= cx;
	y -= cy;

	float tx { x }, ty { y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};

MaskPrimitive::MaskPrimitive() {
	Graphics* gfx = &Graphics::Instance();

	//--- < ���_���̃Z�b�g > ---
	vertex vertices[] =
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { -0.5f, -0.5f }},
		{{  0.5f,  0.5f, 0 }, { 1, 0, 0, 1 }, {  0.5f, -0.5f }},
		{{ -0.5f, -0.5f, 0 }, { 0, 1, 0, 1 }, { -0.5f,  0.5f }},
		{{  0.5f, -0.5f, 0 }, { 0, 0, 1, 1 }, {  0.5f,  0.5f }},
	};

	//--- < ���_�o�b�t�@�I�u�W�F�N�g�̐��� > ---
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
	HRESULT hr = gfx->device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to create buffer.");
	}

	//--- < ���_�V�F�[�_�[�I�u�W�F�N�g�Ɠ��̓��C�A�E�g�I�u�W�F�N�g�̐��� > ---
	const char* csoName { "./Data/Shader/MaskPrimitive_VS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� > ---
	csoName = { "./Data/Shader/MaskPrimitive_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	//--- �萔�o�b�t�@�I�u�W�F�N�g�̐��� ---
	D3D11_BUFFER_DESC constantBufferDesc {};
	constantBufferDesc.ByteWidth = sizeof(CBuffer);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gfx->device->CreateBuffer(&constantBufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	CD3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = gfx->device->CreateBlendState(&blendDesc, blendState[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = gfx->device->CreateBlendState(&blendDesc, blendState[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

MaskPrimitive::~MaskPrimitive() {

}

void MaskPrimitive::Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float radius, DirectX::XMFLOAT2 circlePos, int blend) {
	Graphics* gfx = &Graphics::Instance();

	//--- < �X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾 > ---
	D3D11_VIEWPORT viewport {};
	UINT numViewports { 1 };
	gfx->deviceContext->RSGetViewports(&numViewports, &viewport);

	//--- < ��`�̊e���_�̈ʒu(�X�N���[�����W�n)���v�Z���� > ---

	// left top
	float x0 { dx };
	float y0 { dy };
	// right top
	float x1 { dx + dw };
	float y1 { dy };
	// left bottom
	float x2 { dx };
	float y2 { dy + dh };
	// right bottom
	float x3 { dx + dw };
	float y3 { dy + dh };

	//--- < ���_��]���� > ---

	// ��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;

	float cos { cosf(DirectX::XMConvertToRadians(angle)) };
	float sin { sinf(DirectX::XMConvertToRadians(angle)) };

	Rotate(x0, y0, cx, cy, cos, sin);
	Rotate(x1, y1, cx, cy, cos, sin);
	Rotate(x2, y2, cx, cy, cos, sin);
	Rotate(x3, y3, cx, cy, cos, sin);


	//--- < �X�N���[�����W�n����NDC�֍��W�ϊ� > ---
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	DirectX::XMFLOAT2 tex = { dw * .5f,dh * .5f };

	//--- < �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V���� > ---
	HRESULT hr { S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource {};
	hr = gfx->deviceContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to Map function");
	}

	vertex* vertices { reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr) {
		vertices[0].position = { x0 ,y0, 0 };
		vertices[1].position = { x1 ,y1, 0 };
		vertices[2].position = { x2 ,y2, 0 };
		vertices[3].position = { x3 ,y3, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

		vertices[0].texcoord = { -tex.x, -tex.y };
		vertices[1].texcoord = {  tex.x, -tex.y };
		vertices[2].texcoord = { -tex.x,  tex.y };
		vertices[3].texcoord = {  tex.x,  tex.y };
	}
	gfx->deviceContext->Unmap(vertexBuffer.Get(), 0);

	//--- < ���_�o�b�t�@�[�̃o�C���h > ---
	UINT stride { sizeof(vertex) };
	UINT offset { 0 };
	gfx->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < �v���~�e�B�u�^�C�v����Ƀf�[�^�����Ɋւ�����̃o�C���h > ---
	gfx->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//--- < ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h > ---
	gfx->deviceContext->IASetInputLayout(inputLayout.Get());

	//--- < �V�F�[�_�[�̃o�C���h > ---
	gfx->deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	gfx->deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	CBuffer data;
	data.radius = radius;
	data.circlePos = circlePos;
	gfx->deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx->deviceContext->PSSetConstantBuffers(7, 1, constantBuffer.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11BlendState> deststate;
	UINT mask;
	gfx->deviceContext->OMGetBlendState(deststate.GetAddressOf(), nullptr, &mask);
	gfx->deviceContext->OMSetBlendState(blendState[blend].Get(), nullptr, 0xffffffff);

	//--- < �v���~�e�B�u�̕`�� > ---
	gfx->deviceContext->Draw(4, 0);

	gfx->deviceContext->OMSetBlendState(deststate.Get(), nullptr, mask);

}
