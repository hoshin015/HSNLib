#pragma once

#include <vector>
#include <string>
#include "BehaviorTreeIF.h"
#include "BehaviorTree.h"

// ノード
class Node
{
public:
	Node(const int name, Node* parent, int priority, IBTree::RULE selectRule, IBTree* judgment, BTree* _behaviorTree) : kind(name), parent(parent), priority(priority), selectRule(selectRule), owner(judgment), children(NULL), behaviorTree(_behaviorTree) {}

	~Node();

	// 名前ゲッター
	int GetName() { return kind; }
	// 子ノードゲッター(末尾)
	Node* GetLastChild();
	// 子ノードゲッター(先頭)
	Node* GetTopChild();
	// 優先順位ゲッター
	int GetPriority() { return priority; }
	// 子ノードを追加
	void AddChild(Node* child) { children.push_back(child); }
	// 実行可否判定
	bool Judgment();
	// 優先順位選択
	Node* SelectPriority(std::vector<Node*>* list);
	// ランダム選択
	Node* SelectRandom(std::vector<Node*>* list);
	// シーケンス選択
	Node* SelectSequence(std::vector<Node*>* list);
	// ノード検索
	Node* SearchNode(const int searchName);
	// ノード推論
	Node* Inference();
	// 実行
	IBTree::STATE Run();

public:
	std::vector<Node*> children;	// 子ノード
	int step = 0;					// ステップ

protected:
	int				kind;			// 名前	
	IBTree::RULE	selectRule;		// 選択ルール
	IBTree*			owner;			// 判定クラス
	unsigned int	priority;		// 優先順位
	Node*			parent;			// 親ノード	
	BTree*			behaviorTree;	// ビヘイビアツリー
};