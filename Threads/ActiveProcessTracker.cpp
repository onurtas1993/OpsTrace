#include "ActiveProcessTracker.h"
#include "../Utils/Utils.h"
#include <thread>
#if defined(Q_OS_WIN)
#include "qt_windows.h"
#elif defined(Q_OS_MAC)
#elif defined(Q_OS_LINUX)
#include <X11/Xlib.h>
#include "X11/keysym.h"
#endif

ActiveProcessTracker::ActiveProcessTracker()
{
	stopped = false;
}

ActiveProcessTracker::~ActiveProcessTracker()
{
}

void ActiveProcessTracker::stop()
{
	mutex.lock();
	stopped = true;
	mutex.unlock();
}

QString ActiveProcessTracker::getActiveWindowTitleOnWindows()
{
	HWND hwnd = GetForegroundWindow();
	if (!hwnd)
		return QString();

	wchar_t buf[256];
	if (GetWindowTextW(hwnd, buf, sizeof(buf) / sizeof(wchar_t)) > 0)
		return QString::fromWCharArray(buf);

	return QString();
}

QString ActiveProcessTracker::getActiveWindowTitleOnLinux()
{
	QProcess process(this);
	process.setProgram("xdotool");
	process.setArguments(QStringList() << "getwindowfocus" << "getwindowname");
	process.start();

	// Wait for the process to finish
	while (process.state() != QProcess::NotRunning) {
		qApp->processEvents();
	}

	QString title = process.readAll();
	return title;
}

void ActiveProcessTracker::process()
{
	while (true) {

		QString title;
#if defined(Q_OS_WIN) 
		title = getActiveWindowTitleOnWindows();
#elif defined(Q_OS_LINUX)
		title = getActiveWindowTitleLinux();
#endif
		if (!title.isEmpty())
			emit FocusedWindowChanged(title);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

bool ActiveProcessTracker::isStopped()
{
	bool stopped;
	mutex.lock();
	stopped = this->stopped;
	mutex.unlock();
	return stopped;
}
