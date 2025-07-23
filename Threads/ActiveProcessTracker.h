#pragma once

#include <QObject>
#include <QMutex>

class ActiveProcessTracker : public QObject {
	Q_OBJECT
private:
	QMutex mutex;
	bool stopped;
	bool isStopped();
public:
	ActiveProcessTracker();
	~ActiveProcessTracker();

	QString getActiveWindowTitleOnWindows();
	QString getActiveWindowTitleOnLinux();
public slots:
	void process();
	void stop();
signals:
	void exited(int ospid, int code);
	void FocusedWindowChanged(QString title);
};