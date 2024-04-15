#include "BehaviorTreeNode.h"
#include "BehaviorTree.h"



// �f�X�g���N�^
Node::~Node()
{
}
// �q�m�[�h�Q�b�^�[(����)
Node* Node::GetLastChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}

	return children.at(children.size() - 1);
}

// �q�m�[�h�Q�b�^�[(�擪)
Node* Node::GetTopChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}
	return children.at(0);
}


// �m�[�h����
Node* Node::SearchNode(const int searchName)
{
	// ���O����v
	if (kind == searchName)
	{
		return this;
	}
	else {
		// �q�m�[�h�Ō���
		for (auto itr = children.begin(); itr != children.end(); itr++)
		{
			Node* ret = (*itr)->SearchNode(searchName);

			if (ret != nullptr)
			{
				return ret;
			}
		}
	}

	return nullptr;
}

// �m�[�h���_
Node* Node::Inference()
{
	std::vector<Node*> list;
	Node* result = nullptr;

	// children�̐��������[�v���s���B
	for (int i = 0; i < children.size(); i++)
	{
		// children.at(i)->judgment��nullptr�łȂ����
		if (children.at(i)->owner != nullptr)
		{
			if (children.at(i)->owner->GetBTreeJudge(children.at(i)->kind)) {
				list.emplace_back(children.at(i));
			}
		}
		else {
			//����N���X���Ȃ���Ζ������ɒǉ�
			list.emplace_back(children.at(i));

		}
	}

	// �I�����[���Ńm�[�h����
	switch (selectRule)
	{
		// �D�揇��
	case IBTree::RULE::Priority:
		result = SelectPriority(&list);
		break;
		// �����_��
	case IBTree::RULE::Random:
		result = SelectRandom(&list);
		break;
		// �V�[�P���X
	case IBTree::RULE::Sequence:
		// �V�[�P���X���[�v
	case IBTree::RULE::SequentialLooping:
		result = SelectSequence(&list);
		break;
	}

	if (result != nullptr)
	{
		// �s��������ΏI��
		if (result->GetTopChild() == nullptr) {//���[�m�[�h�i���s�\�m�[�h�j
			return result;
		}
		else {
			// ���܂����m�[�h�Ő��_�J�n
			result = result->Inference();
		}
	}
	return result;
}



// �D�揇�ʂŃm�[�h�I��
Node* Node::SelectPriority(std::vector<Node*>* list)
{
	Node* selectNode = nullptr;
	int priority = INT_MAX;
	//list�ɓo�^���ꂽ�m�[�h�̒��ň�ԗD�揇�ʂ��������̂�T����selectNode�Ɋi�[(�������������D��͍����j
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		if ((*itr)->GetPriority() < priority) {
			priority = (*itr)->GetPriority();

			selectNode = (*itr);
		}
	}

	return selectNode;
}


// �����_���Ńm�[�h�I��
Node* Node::SelectRandom(std::vector<Node*>* list)
{
	int selectNo = 0;
	selectNo = rand() % list->size();
	// list��selectNo�Ԗڂ̎��Ԃ����^�[��
	return (*list).at(selectNo);
}

// �V�[�P���X�E�V�[�P���V�������[�s���O�Ńm�[�h�I��
Node* Node::SelectSequence(std::vector<Node*>* list)
{
	// ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ�A
	if (step >= list->size())
	{
		step = 0;
		if (this->selectRule == IBTree::RULE::Sequence) return nullptr;
	}

	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		// �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩
		if (list->at(step)->GetName() == (*itr)->GetName())
		{
			behaviorTree->sequenceStack.push(this);
			return list->at(step++);
		}
	}

	// �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�m�[�h���Ȃ��̂�nullptr�����^�[������
	step = 0;
	return nullptr;
}

// ����
bool Node::Judgment()
{
	if (owner != nullptr) {
		//judgment�����邩���f�B����΃����o�֐�Judgment()���s�������ʂ����^�[���B
		return owner->GetBTreeJudge(kind);
	}
	return true;
}

// �m�[�h���s
IBTree::STATE Node::Run()
{
	if (owner != nullptr) {
		return owner->ActBTree(kind);
	}

	return IBTree::STATE::Failed;
}
