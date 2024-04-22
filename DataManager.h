#pragma once

class DataManager
{
private:
	DataManager() {}
	~DataManager() {}

public:
	static DataManager& Instance()
	{
		static DataManager instance;
		return instance;
	}

	// ロード
	void LoadData();

	// セーブ
	void SaveData();
};