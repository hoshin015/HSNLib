#include "AdapterReader.h"
#include "ErrorLogger.h"

std::vector<AdapterData> AdapterReader::adapters;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
	// adapterが既に初期化されている場合はそれを返す
	if (adapters.size() > 0)
		return adapters;


	//// まだ初期化されていない場合はadapterを列挙する ////


	// DXGIファクトリの作成
	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to craete DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	// adapterの取得
	IDXGIAdapter* pAdapter;
	UINT index = 0;

	// EnumAdaptersでindexのadapterを取得できる
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
	{
		// ここではadapter本体ではなくコンストラクタで取得した説明をpushしている
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return adapters;
}

AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&this->description);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to Get Description for IDXGIAdapter.");
	}
}