#include "DamageTextManager.h"

// �X�V����
void DamageTextManager::Update()
{
	// �X�V����
	for (DamageText* damageText : damageTexts)
	{
		damageText->Update();
	}

	// �j������
	for (DamageText* damageText : removes)
	{
		std::vector<DamageText*>::iterator it = std::find(damageTexts.begin(), damageTexts.end(), damageText);

		if (it != damageTexts.end())
		{
			damageTexts.erase(it);
		}
		// �_���[�W�e�L�X�g�̔j������
		delete damageText;
	}
	// �j�����X�g���N���A
	removes.clear();
}

//�@�`�揈��
void DamageTextManager::Render()
{
	for (DamageText* damageText : damageTexts)
	{
		damageText->Render();
	}
}

// �_���[�W�e�L�X�g�o�^
void DamageTextManager::Register(DamageText* damageText)
{
	damageTexts.emplace_back(damageText);
}


// �_���[�W�e�L�X�g�폜
void DamageTextManager::Remove(DamageText* damageText)
{
	// �j�����X�g�ɒǉ�
	removes.insert(damageText);
}

// �_���[�W�e�L�X�g�S�폜
void DamageTextManager::Clear()
{
	for (DamageText* damageText : damageTexts)
	{
		delete damageText;
	}
	damageTexts.clear();
}
