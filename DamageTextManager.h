#pragma once

#include <vector>
#include <set>
#include "DamageText.h"

// �_���[�W�e�L�X�g�}�l�[�W���[
class DamageTextManager
{
private:
	DamageTextManager() {}
	~DamageTextManager() {}

public:
	// �B��̃C���X�^���X�擾
	static DamageTextManager& Instance()
	{
		static DamageTextManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	//�@�`�揈��
	void Render();

	// �_���[�W�e�L�X�g�o�^
	void Register(DamageText* damageText);

	// �_���[�W�e�L�X�g�폜
	void Remove(DamageText* damageText);

	// �_���[�W�e�L�X�g�S�폜
	void Clear();

	// �_���[�W�e�L�X�g���擾
	int GetDamageTextCount() const { return static_cast<int>(damageTexts.size()); }

	// �_���[�W�e�L�X�g�擾
	DamageText* GetDamageText(int index) { return damageTexts.at(index); }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

private:
	std::vector<DamageText*> damageTexts;
	std::set<DamageText*> removes;
};