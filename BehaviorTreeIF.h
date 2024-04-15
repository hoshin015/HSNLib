#pragma once

class IBTree
{
public:
	// 選択ルール
	enum class RULE
	{
		Non,				// 末端ノード
		Priority,			// 優先順位
		Sequence,			// シーケンス
		SequentialLooping,	// シーケンシャルルーピング
		Random,				// ランダム
	};

	// 実行情報
	enum class STATE
	{
		Run,		// 実行中
		Failed,		// 実行失敗
		Complete,	// 実行成功	
	};

	IBTree() {}
	virtual ~IBTree() {}
	// BehaviorTree 関係	
	virtual bool GetBTreeJudge(const int kind) = 0;
	virtual STATE ActBTree(const int kind) = 0;
};