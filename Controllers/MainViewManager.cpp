#include "MainViewManager.h"
#include "../Models/Process.h"

#include "../Views/MainView.h"
#include "../Utils/Utils.h"

#include <QtNetwork>

MainViewManager::MainViewManager(QObject* parent, MainView& view, QJsonObject data) :
	QObject(parent),
	view(view),
	currentUserInfo(data)
	//session_id(_session_id)
{

	this->token = token;
	processListThread = new QThread;
	processListWorker = new ProcessListWatcher();
	processListWorker->moveToThread(processListThread);
	connect(processListThread, SIGNAL(started()), processListWorker, SLOT(process()));
	connect(processListWorker, &ProcessListWatcher::NewProcessArrived,
		this, &MainViewManager::onProcessArrived);
	processListThread->start();


	activeWindowTrackerThread = new QThread;
	activeWindowTrackerThread->setObjectName("ActiveProcessTrackerThread");
	activeWindowTrackerWorker = new ActiveProcessTracker();
	connect(activeWindowTrackerThread, SIGNAL(started()), activeWindowTrackerWorker, SLOT(process()));
	connect(activeWindowTrackerWorker, &ActiveProcessTracker::FocusedWindowChanged, this, &MainViewManager::onActiveWindowChanged);
	//connect(activeWindowTrackerWorker, SIGNAL(finished), activeWindowTrackerWorker, SLOT(deleteLater()));
	activeWindowTrackerWorker->moveToThread(activeWindowTrackerThread);
	activeWindowTrackerThread->start();


	elapsedTimeWatcherThread = new QThread;
	elapsedTimeWatcherWorker = new ElapsedTimeWatcher();
	elapsedTimeWatcherWorker->moveToThread(elapsedTimeWatcherThread);
	connect(elapsedTimeWatcherThread, SIGNAL(started()), elapsedTimeWatcherWorker, SLOT(process()));
	connect(elapsedTimeWatcherWorker, &ElapsedTimeWatcher::Tick,
		this, &MainViewManager::onTimeTicked);
	elapsedTimeWatcherThread->start();


	keyboardMouseWatcherThread = new QThread;
	keyboardMouseWatcherWorker = new KeyboardMouseWatcher();
	keyboardMouseWatcherWorker->moveToThread(keyboardMouseWatcherThread);
	connect(keyboardMouseWatcherThread, SIGNAL(started()), keyboardMouseWatcherWorker, SLOT(process()));
	connect(keyboardMouseWatcherWorker, &KeyboardMouseWatcher::KeystrokeDetected,
		this, &MainViewManager::onKeystrokeDetected);
	connect(keyboardMouseWatcherWorker, &KeyboardMouseWatcher::MouseclickDetected,
		this, &MainViewManager::onMouseclickDetected);
	keyboardMouseWatcherThread->start();



	networkThread = new QThread;
	networkThreadWorker = new NetworkThread();
	networkThreadWorker->moveToThread(networkThread);
	connect(networkThread, SIGNAL(started()), networkThreadWorker, SLOT(process()));
	connect(networkThreadWorker, &NetworkThread::RequestSent,
		this, &MainViewManager::onRequestSent);
	networkThread->start();

	connect(&view, &MainView::NotifyWindowBlockStatusChanged,
		this, &MainViewManager::onWindowBlockStatusChanged);

	connect(&view, &MainView::NotifyApplicationClosing,
		this, &MainViewManager::onApplicationClosing);

	connect(networkThreadWorker, &NetworkThread::SessionTerminated,
		this, &MainViewManager::killApplication);
}

MainViewManager::~MainViewManager()
{

}

void MainViewManager::onPauseButtonPressed()
{

}


void MainViewManager::onProcessArrived(QString process)
{
	bool exists = false;
	for (auto& proc : processes)
		if (proc.name() == process.QString::toStdString())
		{
			proc.increaseUptime();
			exists = true;
		}
	if (!exists)
		processes.push_back(process.QString::toStdString());
	view.UpdateAllProcessesTable(processes);

}

void MainViewManager::onActiveWindowChanged(QString title)
{
	QString time = QString::number(0);
	bool exists = false;
	for (auto& window : windows)
		if (window.name() == title.QString::toStdString())
		{
			window.increaseUptime();
			exists = true;
			time = QString::fromStdString(Utils::getInstance().timeFormatter(window.uptime()));
		}
	if (!exists)
		windows.push_back(title.QString::toStdString());
	view.UpdateActiveWindowsTable(windows);

	QString activeWindowUpTime = time;
	view.setActiveWindowLabel(title);
	view.setActiveWindowTimeSpent(activeWindowUpTime);

}

void MainViewManager::onKeystrokeDetected(int sleeptime)
{
	tempKeyboardIdleTimeInSeconds = 0;
}

void MainViewManager::onMouseclickDetected(int)
{
	tempMouseIdleTimeInSeconds = 0;
}

void MainViewManager::onTimeTicked()
{
	if (++tempKeyboardIdleTimeInSeconds > KEYBOARD_IDLE_THRESHOLD_IN_SECS)
	{
		QString inactiveKeyboardTime = QString::fromStdString(Utils::getInstance().timeFormatter(++totalKeyboardIdleTimeInSeconds));
		view.setKeyboardIdleTime(inactiveKeyboardTime);
	}
	if (++tempMouseIdleTimeInSeconds > MOUSE_IDLE_THRESHOLD_IN_SECS)
	{
		QString inactiveMouseTime = QString::fromStdString(Utils::getInstance().timeFormatter(++totalMouseIdleTimeInSeconds));
		view.setMouseIdleTime(inactiveMouseTime);
	}
	QString totalUpTime = QString::fromStdString(Utils::getInstance().timeFormatter(++totalTime));
	view.setTotalTimeSpent(totalUpTime);
}

void MainViewManager::onRequestSent()
{
	QJsonObject json;
	//json.insert("token", token);
	//json.insert("session_id", session_id);
	json.insert("total_mouse_freeze_time_in_seconds", totalMouseIdleTimeInSeconds);
	json.insert("total_no_keyboard_store_time_in_seconds", totalKeyboardIdleTimeInSeconds);

	QJsonArray plot_array;

	QString id_str("process_id");
	QString process_name_str("process_name");
	QString process_uptime_str("uptime_in_seconds");
	QString is_hidden("is_hidden");
	for (auto process_item : processes)
	{
		QJsonObject item_data;

		item_data.insert(id_str, QJsonValue(process_item.process_id()));
		item_data.insert(process_name_str, QJsonValue(process_item.name().c_str()));
		item_data.insert(process_uptime_str, QJsonValue(process_item.uptime()));
		item_data.insert(is_hidden, QJsonValue(process_item.isBlocked()));

		plot_array.push_back(QJsonValue(item_data));
	}

	json.insert(QString("process_list_history"), QJsonValue(plot_array));

	QJsonArray plot_array2;

	QString window_id_str("active_window_id");
	QString active_window_str("active_window_name");
	QString active_window_uptime_str("focused_time_in_seconds");
	for (auto window : windows)
	{
		QJsonObject item_data;

		item_data.insert(window_id_str, QJsonValue(window.process_id()));
		item_data.insert(active_window_str, QJsonValue(window.name().c_str()));
		item_data.insert(active_window_uptime_str, QJsonValue(window.uptime()));
		item_data.insert(is_hidden, QJsonValue(window.isBlocked()));

		plot_array2.push_back(QJsonValue(item_data));
	}

	json.insert(QString("active_window_history"), QJsonValue(plot_array2));

	QString str = Utils::getInstance().jsonToString(json);
	networkThreadWorker->feedUpdatedData(json, currentUserInfo);

}

void MainViewManager::onApplicationClosing()
{
	networkThreadWorker->endSession(currentUserInfo);
}

void MainViewManager::killApplication()
{
	QApplication::exit(0);
}

void MainViewManager::onWindowBlockStatusChanged(Process& p)
{
	for (auto& window : windows)
		if (window.name() == p.name())
			window.setBlockStatus(!window.isBlocked());
}

void MainViewManager::syncNewlyArrivedProcessInfo()
{
}
