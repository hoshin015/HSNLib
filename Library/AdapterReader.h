#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>


class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);	// adapterのポインターを受け取る
	IDXGIAdapter* pAdapter = nullptr;		// 受け取ったadapterのポインタの保存先
	DXGI_ADAPTER_DESC description;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
private:
	static std::vector<AdapterData> adapters;
};