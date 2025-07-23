#pragma once

#include "../Threads/ProcessListWatcher.h"
#include "../Threads/ActiveProcessTracker.h"
#include "../Threads/ElapsedTimeWatcher.h"
#include "../Threads/KeyboardMouseWatcher.h"
#include "../Threads/NetworkThread.h"    
#include <QObject> 
#include <QList>

#define KEYBOARD_IDLE_THRESHOLD_IN_SECS 5
#define MOUSE_IDLE_THRESHOLD_IN_SECS 5

class MainView;
class Process;
class MainViewManager : public QObject
{
	Q_OBJECT
public:
	explicit MainViewManager(QObject* parent, MainView& view, QJsonObject data);
	~MainViewManager();

public slots:
	void onPauseButtonPressed();
	void onProcessArrived(QString);
	void onActiveWindowChanged(QString);
	void onKeystrokeDetected(int);
	void onMouseclickDetected(int);
	void onTimeTicked();
	void onRequestSent();
	void onApplicationClosing();
	void killApplication();

	void onWindowBlockStatusChanged(Process& p);

private:
	QString token;
	QJsonObject currentUserInfo;
	int totalTime = 0;
	int tempKeyboardIdleTimeInSeconds = 0;
	int totalKeyboardIdleTimeInSeconds = 0;// this will be updated after 1 minute idle threshold(60secs) passed.
	int tempMouseIdleTimeInSeconds = 0;
	int totalMouseIdleTimeInSeconds = 0;// this will be updated after 1 minute idle threshold(60secs) passed.
	MainView& view;

	QThread* processListThread;
	ProcessListWatcher* processListWorker;

	QThread* activeWindowTrackerThread;
	ActiveProcessTracker* activeWindowTrackerWorker;

	QThread* elapsedTimeWatcherThread;
	ElapsedTimeWatcher* elapsedTimeWatcherWorker;

	QThread* keyboardMouseWatcherThread;
	KeyboardMouseWatcher* keyboardMouseWatcherWorker;

	QThread* networkThread;
	NetworkThread* networkThreadWorker;

	QList<Process> processes;
	QList<Process> windows;
	//std::map<QString, int> activeWindowMap;

	void syncNewlyArrivedProcessInfo();

	explicit MainViewManager(const MainViewManager& rhs);
};
