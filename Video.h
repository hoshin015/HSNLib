#pragma once
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wrl.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class Video {
public:
	static Video& Instance() {
		static Video instance;
		return instance;
	}
	~Video() {}

	void LoadVideoFlie(const char* fliePath);

private:
	Video();

public:


private:
	ComPtr<IMFSourceReader> _sourceReader;
	ComPtr<IMFMediaType> _audioType;

	ComPtr<IMFMediaType> _videoTypeIYUV;
	ComPtr<IMFMediaType> _videoTypeARGB;
	ComPtr<IMFTransform> _transform;
};
