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

	// ���[�h
	void LoadData();

	// �Z�[�u
	void SaveData();
};