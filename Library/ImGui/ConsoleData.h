#pragma once
#include <vector>
#include <string>


class ConsoleData
{
private:
	ConsoleData() {}
	~ConsoleData() {}
public:
	static ConsoleData& Instance()
	{
		static ConsoleData instance;
		return instance;
	}

public:
	std::vector<std::string> logs;
};
