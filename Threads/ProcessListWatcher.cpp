#include "ProcessListWatcher.h"
#include <qtextstream.h>
#include <map> 
#include <thread>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <fstream>


ProcessListWatcher::ProcessListWatcher() {
	stopped = false;
}

ProcessListWatcher::~ProcessListWatcher() {
}

void ProcessListWatcher::stop()
{
	mutex.lock();
	stopped = true;
	mutex.unlock();
}

std::string stringCleaner(const std::string& str)
{
	std::istringstream tempStr(str);
	std::ostringstream cleanLine;
	std::unique_copy(std::istream_iterator<std::string>(tempStr),
		std::istream_iterator<std::string>(),
		std::ostream_iterator<std::string>(cleanLine, "\n"));
	return cleanLine.str();
}

void ProcessListWatcher::process()
{
	while (!isStopped()) {
#if defined(Q_OS_WIN)
		QProcess process;
		connect(&process, &QProcess::errorOccurred, this, &ProcessListWatcher::processError);

		process.setReadChannel(QProcess::StandardOutput);
		process.setProcessChannelMode(QProcess::MergedChannels);
		//process.start("WMIC.exe /OUTPUT:STDOUT PROCESS get Caption");
		process.start("cmd", QStringList() << "/C" << "echo" << "process" << "get" << "caption" << "|" << "wmic");

		bool start_result = process.waitForStarted(1000);
		bool finish_result = process.waitForFinished(1000);
		QByteArray list = process.readAll();
		QString s_data = QString::fromStdString(list.data());
		QString filteredData = QString::fromStdString(stringCleaner(s_data.QString::toStdString()));
		QTextStream in(&filteredData);
		QString filter = "";
		in.readLine();
		while (!in.atEnd())
		{
			QString process_name = in.readLine();
			if (filter.contains(process_name, Qt::CaseInsensitive))
				continue;
			else
				filter.append(process_name);
			emit NewProcessArrived(process_name);
		}
#elif defined(Q_OS_LINUX)
		int status = std::system("top -bn 1 | grep \"^ \" | awk '{ printf(\"%-8s\\n\", $12); }' > top.txt"); // execute the UNIX command "ls -l >test.txt"
		std::ifstream in_file("top.txt");
		std::string s_data((std::istreambuf_iterator<char>(in_file)),
			std::istreambuf_iterator<char>());

		QString filteredData = QString::fromStdString(stringCleaner(s_data));
		QTextStream in(&filteredData);
		QString filter = "";
		in.readLine();
		while (!in.atEnd())
		{
			QString process_name = in.readLine();
			if (filter.contains(process_name, Qt::CaseInsensitive))
				continue;
			else
				filter.append(process_name);
			emit NewProcessArrived(process_name);
		}

#endif
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void ProcessListWatcher::processError(QProcess::ProcessError error)
{
	std::cout << "error enum val = " << error << std::endl;
}

bool ProcessListWatcher::isStopped()
{
	bool stopped;
	mutex.lock();
	stopped = this->stopped;
	mutex.unlock();
	return stopped;
}
