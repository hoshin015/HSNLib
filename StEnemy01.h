#include "SpinningTopEnemy.h"


class StEnemy01 : public SpinningTopEnemy
{
public:
	StEnemy01();
	~StEnemy01() override;

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render(bool drawShadow = false) override;

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

	// TargetPosition �X�V
	void UpdateTargetPosition();

	// DrawDebugPrimitive


protected:
	// ���S����
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;
};