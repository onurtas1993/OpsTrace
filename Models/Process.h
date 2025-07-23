#pragma once

#include <string>
#include <stdlib.h>

class Process {
private:
	int _process_id;
	std::string _name;
	int uptime_in_seconds = 0;
	bool blocked = false;
public:
	Process(const std::string& name) : _name{ name }
	{
		_process_id = rand();
	}

	//Getters
	const std::string& name() const { return _name; }
	const int uptime() const { return uptime_in_seconds; }
	const int process_id() const { return _process_id; }
	const int isBlocked() const { return blocked; }

	//Setters
	void setName(const std::string& value) { _name = value; }
	void setUptime(const int value) { uptime_in_seconds = value; }
	void increaseUptime() { ++uptime_in_seconds; }
	void setBlockStatus(const bool value) { blocked = value; }

};