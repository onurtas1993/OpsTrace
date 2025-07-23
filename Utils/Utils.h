#pragma once

#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <locale>
#include <codecvt>

#include <QtNetwork>

class Utils
{
public:
	static Utils &getInstance()
	{
		static Utils instance;
		return instance;
	}

private:
	Utils() {}

public:
	Utils(Utils const &) = delete;
	void operator=(Utils const &) = delete;

	QString jsonToString(QJsonObject json)
	{

		QJsonDocument Doc(json);
		QByteArray ba = Doc.toJson();
		return QString(ba);
	}

	QJsonObject bytearrayToJson(QByteArray in)
	{
		QJsonDocument document = QJsonDocument::fromJson(in);
		QJsonObject rootObj = document.object();
		return rootObj;
	}

	std::string ws2s(const std::wstring &wstr)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}

	std::string stringCleaner(const std::string &str)
	{
		std::istringstream tempStr(str);
		std::ostringstream cleanLine;
		std::unique_copy(std::istream_iterator<std::string>(tempStr),
						 std::istream_iterator<std::string>(),
						 std::ostream_iterator<std::string>(cleanLine, "\n"));
		return cleanLine.str();
	}

	std::string timeFormatter(int hour, int minute, int second)
	{
		std::string totalUpTime = (hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour)) +
								  ":" +
								  (minute < 10 ? "0" + std::to_string(minute) : std::to_string(minute)) +
								  ":" +
								  (second < 10 ? "0" + std::to_string(second) : std::to_string(second));
		return totalUpTime;
	}

	std::string timeFormatter(int seconds)
	{
		std::string totalUpTime = (seconds / 3600 < 10 ? "0" + std::to_string(seconds / 3600) : std::to_string(seconds / 3600)) +
								  ":" +
								  ((seconds % 3600) / 60 < 10 ? "0" + std::to_string((seconds % 3600) / 60) : std::to_string((seconds % 3600) / 60)) +
								  ":" +
								  (seconds % 60 < 10 ? "0" + std::to_string(seconds % 60) : std::to_string(seconds % 60));
		return totalUpTime;
	}
};
