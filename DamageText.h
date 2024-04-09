#pragma once

#include <DirectXMath.h>
#include <tchar.h>

class DamageText
{
public:
	DamageText(DirectX::XMFLOAT3 pos, const TCHAR* str)
	{
		position = pos;
		// ������̒��� + 1�inull �I�[�������j�̃������𓮓I�Ɋm�ۂ��A��������R�s�[����
		text = new TCHAR[_tcslen(str) + 1];
		_tcscpy_s(const_cast<TCHAR*>(text), _tcslen(str) + 1, str);
	}
	~DamageText()  {}

	// �X�V����
	void Update();

	// �`�揈��
	void Render();

	// �j��
	void Destroy();

	// ���W�X�V
	void UpdatePosition();

	//--------------------------------------------------------------
	//  �X�e�[�g�Ǘ�
	//--------------------------------------------------------------

	// �ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();
	// �ҋ@�X�e�[�g�X�V����
	void UpdateIdleState();

	// �t�F�[�h�A�E�g�X�e�[�g�֑J��
	void TransitionFadeOutState();
	// �t�F�[�h�A�E�g�X�e�[�g�X�V����
	void UpdateFadeOutState();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 drawPosition = { 0,0 };
	const TCHAR* text;
	float size = 32;

	float timer = 0.0f;
	float idleTime = 0.5f;
	float fadeOutTime = 0.15f;

	float alpha = 1.0f;

	float addY = 0.0f;

	enum class State
	{
		Idle,
		fadeOut,
	};
	State state = State::Idle;
};
