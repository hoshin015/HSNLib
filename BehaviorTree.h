#pragma once
#include <string>
#include <stack>
#include "BehaviorTreeIF.h"

class Node;

// ビヘイビアツリー
class BTree
{
public:
	BTree(IBTree* _owner) : owner(_owner) {};
	~BTree();

	// 実行ノードを推論
	Node* ActiveNodeInference();
	// シーケンスノードから推論開始
	Node* SequenceBack(Node* sequenceNode);
	// ノードを追加
	void AddNode(const int parentName, int entryName, int priority, IBTree::RULE selectRule, IBTree* jdgement);

	// 実行
	Node* Run(Node* actionNode);
	// 更新
	bool Update();
	// 実行する中間ノードをスタック
	std::stack<Node*> sequenceStack;
	// シーケンスノードをスタックから取り出す
	Node* PopSequenceNode();
	// シーケンスノードのスタッククリア
	void ClearSequenceNode();

private:
	// ノード全削除
	void NodeAllClear(Node* delNode);
	// ルートノード
	Node* root = nullptr;
	IBTree* owner;
	// 実行中のノード
	Node* activeNode = nullptr;
};