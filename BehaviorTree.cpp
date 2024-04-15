#include "BehaviorTree.h"
#include "BehaviorTreeNode.h"


// �f�X�g���N�^
BTree::~BTree()
{
	NodeAllClear(root);
}

void BTree::AddNode(const int parentName, const int entryName, int priority, IBTree::RULE selectRule, IBTree* judgment)
{
	if (parentName != 0)
	{
		Node* parentNode = root->SearchNode(parentName);

		if (parentNode != nullptr)
		{
			Node* addNode = new Node(entryName, parentNode, priority, selectRule, judgment, this);

			parentNode->AddChild(addNode);
		}
	}
	else {
		if (root == nullptr)
		{
			root = new Node(entryName, nullptr, priority, selectRule, judgment, this);
		}
	}
}

// �V�[�P���X�m�[�h�̃|�b�v
Node* BTree::PopSequenceNode()
{
	// ��Ȃ�NULL
	if (sequenceStack.empty() != 0)
	{
		return nullptr;
	}
	Node* node = sequenceStack.top();
	if (node != nullptr)
	{
		// ���o�����f�[�^���폜
		sequenceStack.pop();
	}
	return node;
}
// �V�[�P���X�m�[�h�̃X�^�b�N�N���A
void BTree::ClearSequenceNode()
{

	while (sequenceStack.empty() == 0)
	{
		Node* node = sequenceStack.top();
		// ���o�����f�[�^���폜
		node->step = 0;
		sequenceStack.pop();
	}
	return;
}














// ���_
Node* BTree::ActiveNodeInference()
{
	// �f�[�^�����Z�b�g���ĊJ�n
//	data->Init();
	return root->Inference();
}

// �V�[�P���X�m�[�h����̐��_�J�n
Node* BTree::SequenceBack(Node* sequenceNode)
{
	return sequenceNode->Inference();
}

// �m�[�h���s
Node* BTree::Run(Node* actionNode)
{
	// �m�[�h���s
	IBTree::STATE state = actionNode->Run();

	// ����I��
	if (state == IBTree::STATE::Complete)
	{
		// �V�[�P���X�̓r�����𔻒f
		Node* sequenceNode = PopSequenceNode();

		// �r������Ȃ��Ȃ�I��
		if (sequenceNode == nullptr)
		{
			return nullptr;
		}
		else {
			// �r���Ȃ炻������n�߂�
			return SequenceBack(sequenceNode);
		}
		// ���s�͏I��
	}
	else if (state == IBTree::STATE::Failed) {
		ClearSequenceNode();
		return nullptr;
	}

	// ����ێ�
	return actionNode;
}

//���s
bool BTree::Update() {
	// ���ݎ��s����Ă���m�[�h���������
	if (activeNode == nullptr)
	{
		// ���Ɏ��s����m�[�h�𐄘_����B
		activeNode = ActiveNodeInference();
	}
	// ���ݎ��s����m�[�h�������
	if (activeNode != nullptr)
	{
		// �r�w�C�r�A�c���[����m�[�h�����s�B
		activeNode = Run(activeNode);
	}
	return (activeNode == nullptr);
}


// �o�^���ꂽ�m�[�h��S�č폜����
void BTree::NodeAllClear(Node* delNode)
{
	size_t count = delNode->children.size();
	delNode->step = 0;
	if (count > 0)
	{
		for (Node* node : delNode->children)
		{
			node->step = 0;
			NodeAllClear(node);
		}
		delete delNode;
	}
	else
	{
		delete delNode;
	}
}


