#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log(std::string message)
{
	std::string error_message = "Error: " + message;	// エラーメッセージの作成

	// ダイアログボックスの生成
	MessageBoxA(
		NULL,					// 所有者ウィンドウへのハンドル
		error_message.c_str(),	// 表示するメッセージ
		"Error",				// ダイアログボックスのタイトル
		MB_ICONERROR			// ダイアログボックスの内容と動作
	);
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::wstring wideMessage(message.begin(), message.end());
	std::wstring errorMessage = L"Error: " + wideMessage + L"\n" + error.ErrorMessage();

	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::wstring errorMessage = L"Error: " + message + L"\n" + error.ErrorMessage();

	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}
