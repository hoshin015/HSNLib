#include "FullScreenQuad.h"
#include "Shader.h"
#include "Graphics.h"
#include "../ErrorLogger.h"

FullScreenQuad::FullScreenQuad()
{
	CreateVsFromCso("Data/Shader/FullScreenQuad_VS.cso", embeddedVertexShader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	CreatePsFromCso("Data/Shader/FullScreenQuad_PS.cso", embeddedPixelShader.ReleaseAndGetAddressOf());
}

void FullScreenQuad::blit(ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader, ID3D11VertexShader* replacedVertexShader)
{
	Graphics& gfx = Graphics::Instance();

	gfx.deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	gfx.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gfx.deviceContext->IASetInputLayout(nullptr);

	replacedVertexShader ? gfx.deviceContext->VSSetShader(replacedVertexShader, 0, 0) : gfx.deviceContext->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ? gfx.deviceContext->PSSetShader(replacedPixelShader, 0, 0) : gfx.deviceContext->PSSetShader(embeddedPixelShader.Get(), 0, 0);

	gfx.deviceContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);
	gfx.deviceContext->VSSetShaderResources(startSlot, numViews, shaderResourceView);
	gfx.deviceContext->Draw(4, 0);
}

void FullScreenQuad::blit(ID3D11ShaderResourceView** shaderResourceView, ID3D11ShaderResourceView** bloomFilterViews, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader, ID3D11VertexShader* replacedVertexShader)
{
	Graphics& gfx = Graphics::Instance();

	gfx.deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	gfx.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gfx.deviceContext->IASetInputLayout(nullptr);

	replacedVertexShader ? gfx.deviceContext->VSSetShader(replacedVertexShader, 0, 0) : gfx.deviceContext->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ? gfx.deviceContext->PSSetShader(replacedPixelShader, 0, 0) : gfx.deviceContext->PSSetShader(embeddedPixelShader.Get(), 0, 0);

	gfx.deviceContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);
	gfx.deviceContext->VSSetShaderResources(startSlot, numViews, shaderResourceView);


	gfx.deviceContext->Draw(4, 0);
}


