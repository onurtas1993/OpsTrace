#include "ElapsedTimeWatcher.h"
#include <thread>


ElapsedTimeWatcher::ElapsedTimeWatcher() {
	stopped = false;
}

ElapsedTimeWatcher::~ElapsedTimeWatcher() {
}

void ElapsedTimeWatcher::stop()
{
	mutex.lock();
	stopped = true;
	mutex.unlock();
}

void ElapsedTimeWatcher::process()
{
	while (!isStopped()) {
		emit Tick();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

}


bool ElapsedTimeWatcher::isStopped()
{
	bool stopped;
	mutex.lock();
	stopped = this->stopped;
	mutex.unlock();
	return stopped;
}
