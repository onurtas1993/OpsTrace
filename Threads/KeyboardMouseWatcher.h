#pragma once

#include "../Models/Process.h"
#include <QList>
#include <QObject>
#include <QMutex>
#if defined(Q_OS_LINUX)
#include "../Utils/idle.h"
#endif

class KeyboardMouseWatcher : public QObject {
	Q_OBJECT
private:
	int keyboardSleepTime = 0;
	int mouseSleepTime = 0;
	QMutex mutex;
#if defined(Q_OS_LINUX)
	IdlePlatform ip;
#endif
	bool stopped;
	bool isStopped();
public:
	KeyboardMouseWatcher();
	~KeyboardMouseWatcher();
public slots:
	void process();
	void stop();
signals:
	void exited(int ospid, int code);
	void KeystrokeDetected(int sleeptime);
	void MouseclickDetected(int sleeptime);
};
