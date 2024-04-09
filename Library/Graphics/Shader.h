#pragma once
#include <d3d11.h>

HRESULT CreateVsFromCso(const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);

HRESULT CreatePsFromCso(const char* csoName, ID3D11PixelShader** pixelShader);

// PARTICLE
HRESULT CreateGsFromCso(const char* cso_name, ID3D11GeometryShader** geometry_shader);
HRESULT CreateCsFromCso(const char* cso_name, ID3D11ComputeShader** compute_shader);

