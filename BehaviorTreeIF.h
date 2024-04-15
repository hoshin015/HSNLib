#pragma once

class IBTree
{
public:
	// �I�����[��
	enum class RULE
	{
		Non,				// ���[�m�[�h
		Priority,			// �D�揇��
		Sequence,			// �V�[�P���X
		SequentialLooping,	// �V�[�P���V�������[�s���O
		Random,				// �����_��
	};

	// ���s���
	enum class STATE
	{
		Run,		// ���s��
		Failed,		// ���s���s
		Complete,	// ���s����	
	};

	IBTree() {}
	virtual ~IBTree() {}
	// BehaviorTree �֌W	
	virtual bool GetBTreeJudge(const int kind) = 0;
	virtual STATE ActBTree(const int kind) = 0;
};