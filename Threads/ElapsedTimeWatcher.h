#pragma once

#include <QObject>
#include <QMutex>

class ElapsedTimeWatcher : public QObject {
	Q_OBJECT
private:
	QMutex mutex;
	bool stopped;
	bool isStopped();
public:
	ElapsedTimeWatcher();
	~ElapsedTimeWatcher();
public slots:
	void process();
	void stop();
signals:
	void exited(int ospid, int code);
	void Tick();
};