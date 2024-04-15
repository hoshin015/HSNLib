#include "BehaviorTree.h"
#include "BehaviorTreeNode.h"


// デストラクタ
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

// シーケンスノードのポップ
Node* BTree::PopSequenceNode()
{
	// 空ならNULL
	if (sequenceStack.empty() != 0)
	{
		return nullptr;
	}
	Node* node = sequenceStack.top();
	if (node != nullptr)
	{
		// 取り出したデータを削除
		sequenceStack.pop();
	}
	return node;
}
// シーケンスノードのスタッククリア
void BTree::ClearSequenceNode()
{

	while (sequenceStack.empty() == 0)
	{
		Node* node = sequenceStack.top();
		// 取り出したデータを削除
		node->step = 0;
		sequenceStack.pop();
	}
	return;
}














// 推論
Node* BTree::ActiveNodeInference()
{
	// データをリセットして開始
//	data->Init();
	return root->Inference();
}

// シーケンスノードからの推論開始
Node* BTree::SequenceBack(Node* sequenceNode)
{
	return sequenceNode->Inference();
}

// ノード実行
Node* BTree::Run(Node* actionNode)
{
	// ノード実行
	IBTree::STATE state = actionNode->Run();

	// 正常終了
	if (state == IBTree::STATE::Complete)
	{
		// シーケンスの途中かを判断
		Node* sequenceNode = PopSequenceNode();

		// 途中じゃないなら終了
		if (sequenceNode == nullptr)
		{
			return nullptr;
		}
		else {
			// 途中ならそこから始める
			return SequenceBack(sequenceNode);
		}
		// 失敗は終了
	}
	else if (state == IBTree::STATE::Failed) {
		ClearSequenceNode();
		return nullptr;
	}

	// 現状維持
	return actionNode;
}

//実行
bool BTree::Update() {
	// 現在実行されているノードが無ければ
	if (activeNode == nullptr)
	{
		// 次に実行するノードを推論する。
		activeNode = ActiveNodeInference();
	}
	// 現在実行するノードがあれば
	if (activeNode != nullptr)
	{
		// ビヘイビアツリーからノードを実行。
		activeNode = Run(activeNode);
	}
	return (activeNode == nullptr);
}


// 登録されたノードを全て削除する
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


