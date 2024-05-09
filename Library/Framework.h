#pragma once
#include <wtypes.h>
#include <string>
#include <vector>

//--------------------------------------------------------------
//  Framework
//--------------------------------------------------------------
class Framework
{
private:
	Framework() {}
	~Framework() {}

public:
	static Framework& Instance()
	{
		static Framework instance;
		return instance;
	}

public:
	int windowWidth = 1280;	//1280
	int windowHeight = 720;	//720

private:
	HWND hwnd;

	const wchar_t* windowName = L"Shot Away";
	const int screenWidth = 1280;
	const int screenHeight = 720;

	BOOL isFullScreen = FALSE;
public:
	// ������
	bool Initialize(HINSTANCE hInstance);
	// �X�V
	void Update();
	// �I����
	void Finalize();

private:
	// �E�B���h�E�쐬
	void CreateHSNWindow(HINSTANCE hInstance);
	// �E�B���h�E�v���V�[�W��
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// FPS ���^�C�g���o�[�ɃZ�b�g
	void CalculateFrame();
};