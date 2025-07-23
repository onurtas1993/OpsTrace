#pragma once

#include <QObject>
#include <QProcess>
#include <QMutex>

class ProcessListWatcher : public QObject {
	Q_OBJECT
private:
	QMutex mutex;
	bool stopped;
	bool isStopped();
public:
	ProcessListWatcher();
	~ProcessListWatcher();
public slots:
	void process();
	void stop();
	void processError(QProcess::ProcessError error);

signals:
	void exited(int ospid, int code);
	void NewProcessArrived(QString process);
};