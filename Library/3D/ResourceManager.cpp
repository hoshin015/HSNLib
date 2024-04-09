#include "ResourceManager.h"

// ���f�����\�[�X�ǂݍ���
std::shared_ptr<SkinnedMesh> ResourceManager::LoadModelResource(const char* filename)
{
	// ���f��������
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// �����N(����)���؂�Ă��Ȃ����m�F
		if (!it->second.expired())
		{
			// �ǂݍ��ݍς݂̃��f�����\�[�X��Ԃ�
			return it->second.lock();
		}
	}

	// �V�K���f�����\�[�X�쐬���ǂݍ���
	auto model = std::make_shared<SkinnedMesh>(filename);

	// �}�b�v�ɓo�^
	models[filename] = model;

	return model;
}
