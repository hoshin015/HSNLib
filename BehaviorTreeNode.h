#pragma once

#include <vector>
#include <string>
#include "BehaviorTreeIF.h"
#include "BehaviorTree.h"

// �m�[�h
class Node
{
public:
	Node(const int name, Node* parent, int priority, IBTree::RULE selectRule, IBTree* judgment, BTree* _behaviorTree) : kind(name), parent(parent), priority(priority), selectRule(selectRule), owner(judgment), children(NULL), behaviorTree(_behaviorTree) {}

	~Node();

	// ���O�Q�b�^�[
	int GetName() { return kind; }
	// �q�m�[�h�Q�b�^�[(����)
	Node* GetLastChild();
	// �q�m�[�h�Q�b�^�[(�擪)
	Node* GetTopChild();
	// �D�揇�ʃQ�b�^�[
	int GetPriority() { return priority; }
	// �q�m�[�h��ǉ�
	void AddChild(Node* child) { children.push_back(child); }
	// ���s�۔���
	bool Judgment();
	// �D�揇�ʑI��
	Node* SelectPriority(std::vector<Node*>* list);
	// �����_���I��
	Node* SelectRandom(std::vector<Node*>* list);
	// �V�[�P���X�I��
	Node* SelectSequence(std::vector<Node*>* list);
	// �m�[�h����
	Node* SearchNode(const int searchName);
	// �m�[�h���_
	Node* Inference();
	// ���s
	IBTree::STATE Run();

public:
	std::vector<Node*> children;	// �q�m�[�h
	int step = 0;					// �X�e�b�v

protected:
	int				kind;			// ���O	
	IBTree::RULE	selectRule;		// �I�����[��
	IBTree*			owner;			// ����N���X
	unsigned int	priority;		// �D�揇��
	Node*			parent;			// �e�m�[�h	
	BTree*			behaviorTree;	// �r�w�C�r�A�c���[
};