#pragma once
#include <string>
#include <stack>
#include "BehaviorTreeIF.h"

class Node;

// �r�w�C�r�A�c���[
class BTree
{
public:
	BTree(IBTree* _owner) : owner(_owner) {};
	~BTree();

	// ���s�m�[�h�𐄘_
	Node* ActiveNodeInference();
	// �V�[�P���X�m�[�h���琄�_�J�n
	Node* SequenceBack(Node* sequenceNode);
	// �m�[�h��ǉ�
	void AddNode(const int parentName, int entryName, int priority, IBTree::RULE selectRule, IBTree* jdgement);

	// ���s
	Node* Run(Node* actionNode);
	// �X�V
	bool Update();
	// ���s���钆�ԃm�[�h���X�^�b�N
	std::stack<Node*> sequenceStack;
	// �V�[�P���X�m�[�h���X�^�b�N������o��
	Node* PopSequenceNode();
	// �V�[�P���X�m�[�h�̃X�^�b�N�N���A
	void ClearSequenceNode();

private:
	// �m�[�h�S�폜
	void NodeAllClear(Node* delNode);
	// ���[�g�m�[�h
	Node* root = nullptr;
	IBTree* owner;
	// ���s���̃m�[�h
	Node* activeNode = nullptr;
};