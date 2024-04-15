#include "BehaviorTreeNode.h"
#include "BehaviorTree.h"



// デストラクタ
Node::~Node()
{
}
// 子ノードゲッター(末尾)
Node* Node::GetLastChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}

	return children.at(children.size() - 1);
}

// 子ノードゲッター(先頭)
Node* Node::GetTopChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}
	return children.at(0);
}


// ノード検索
Node* Node::SearchNode(const int searchName)
{
	// 名前が一致
	if (kind == searchName)
	{
		return this;
	}
	else {
		// 子ノードで検索
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

// ノード推論
Node* Node::Inference()
{
	std::vector<Node*> list;
	Node* result = nullptr;

	// childrenの数だけループを行う。
	for (int i = 0; i < children.size(); i++)
	{
		// children.at(i)->judgmentがnullptrでなければ
		if (children.at(i)->owner != nullptr)
		{
			if (children.at(i)->owner->GetBTreeJudge(children.at(i)->kind)) {
				list.emplace_back(children.at(i));
			}
		}
		else {
			//判定クラスがなければ無条件に追加
			list.emplace_back(children.at(i));

		}
	}

	// 選択ルールでノード決め
	switch (selectRule)
	{
		// 優先順位
	case IBTree::RULE::Priority:
		result = SelectPriority(&list);
		break;
		// ランダム
	case IBTree::RULE::Random:
		result = SelectRandom(&list);
		break;
		// シーケンス
	case IBTree::RULE::Sequence:
		// シーケンスループ
	case IBTree::RULE::SequentialLooping:
		result = SelectSequence(&list);
		break;
	}

	if (result != nullptr)
	{
		// 行動があれば終了
		if (result->GetTopChild() == nullptr) {//末端ノード（実行可能ノード）
			return result;
		}
		else {
			// 決まったノードで推論開始
			result = result->Inference();
		}
	}
	return result;
}



// 優先順位でノード選択
Node* Node::SelectPriority(std::vector<Node*>* list)
{
	Node* selectNode = nullptr;
	int priority = INT_MAX;
	//listに登録されたノードの中で一番優先順位が高いものを探してselectNodeに格納(数が小さい程優先は高い）
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		if ((*itr)->GetPriority() < priority) {
			priority = (*itr)->GetPriority();

			selectNode = (*itr);
		}
	}

	return selectNode;
}


// ランダムでノード選択
Node* Node::SelectRandom(std::vector<Node*>* list)
{
	int selectNo = 0;
	selectNo = rand() % list->size();
	// listのselectNo番目の実態をリターン
	return (*list).at(selectNo);
}

// シーケンス・シーケンシャルルーピングでノード選択
Node* Node::SelectSequence(std::vector<Node*>* list)
{
	// 中間ノードに登録されているノード数以上の場合、
	if (step >= list->size())
	{
		step = 0;
		if (this->selectRule == IBTree::RULE::Sequence) return nullptr;
	}

	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		// 子ノードが実行可能リストに含まれているか
		if (list->at(step)->GetName() == (*itr)->GetName())
		{
			behaviorTree->sequenceStack.push(this);
			return list->at(step++);
		}
	}

	// 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
	step = 0;
	return nullptr;
}

// 判定
bool Node::Judgment()
{
	if (owner != nullptr) {
		//judgmentがあるか判断。あればメンバ関数Judgment()実行した結果をリターン。
		return owner->GetBTreeJudge(kind);
	}
	return true;
}

// ノード実行
IBTree::STATE Node::Run()
{
	if (owner != nullptr) {
		return owner->ActBTree(kind);
	}

	return IBTree::STATE::Failed;
}
