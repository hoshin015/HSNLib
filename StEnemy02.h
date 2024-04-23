#include "SpinningTopEnemy.h"


class StEnemy02 : public SpinningTopEnemy
{
public:
	StEnemy02();
	~StEnemy02() override;

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