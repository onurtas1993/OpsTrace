#include "KeyboardMouseWatcher.h"
#include <thread>
#include <iostream>

#if defined(Q_OS_WIN)
#include "qt_windows.h"
#elif defined(Q_OS_MAC)
#elif defined(Q_OS_LINUX)
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

KeyboardMouseWatcher::KeyboardMouseWatcher() {
	stopped = false;

#if defined(Q_OS_LINUX)
	ip.init();
#endif
}

KeyboardMouseWatcher::~KeyboardMouseWatcher() {
}

void KeyboardMouseWatcher::stop()
{
	mutex.lock();
	stopped = true;
	mutex.unlock();
}

void KeyboardMouseWatcher::process()
{
	while (!isStopped()) {

#if defined(Q_OS_WIN)
		if (GetKeyState('1') & 0x8000 || GetKeyState('2') & 0x8000 || GetKeyState('3') & 0x8000 || GetKeyState('4') & 0x8000 || GetKeyState('5') & 0x8000 || GetKeyState('6') & 0x8000 || GetKeyState('7') & 0x8000 || GetKeyState('8') & 0x8000 || GetKeyState('9') & 0x8000 || GetKeyState('0') & 0x8000)
		{
			emit KeystrokeDetected(keyboardSleepTime = 0);
		}
		else if (GetKeyState('A') & 0x8000 || GetKeyState('B') & 0x8000 || GetKeyState('C') & 0x8000 || GetKeyState('D') & 0x8000 || GetKeyState('E') & 0x8000 || GetKeyState('F') & 0x8000 || GetKeyState('G') & 0x8000 || GetKeyState('H') & 0x8000 || GetKeyState('I') & 0x8000 || GetKeyState('J') & 0x8000 || GetKeyState('K') & 0x8000 || GetKeyState('L') & 0x8000 || GetKeyState('M') & 0x8000 || GetKeyState('N') & 0x8000 || GetKeyState('O') & 0x8000 || GetKeyState('P') & 0x8000 || GetKeyState('R') & 0x8000 || GetKeyState('S') & 0x8000 || GetKeyState('T') & 0x8000 || GetKeyState('U') & 0x8000 || GetKeyState('V') & 0x8000 || GetKeyState('Y') & 0x8000 || GetKeyState('Z') & 0x8000)
		{
			emit KeystrokeDetected(keyboardSleepTime = 0);
		}
		else if (GetKeyState(VK_LBUTTON) & 0x8000 || GetKeyState(VK_RBUTTON) & 0x8000)
		{
			emit MouseclickDetected(mouseSleepTime = 0);
		}
#elif defined(Q_OS_LINUX)

		auto isPressed = [](KeySym key)
			{
				Display* dpy = XOpenDisplay(":0");
				char keys_return[32];
				XQueryKeymap(dpy, keys_return);
				KeyCode kc2 = XKeysymToKeycode(dpy, key);
				bool pressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
				XCloseDisplay(dpy);
				return pressed;
			};
		if (isPressed(XK_1) || isPressed(XK_2) || isPressed(XK_3) || isPressed(XK_4) || isPressed(XK_5) || isPressed(XK_6) || isPressed(XK_7) || isPressed(XK_8) || isPressed(XK_9) || isPressed(XK_0))
		{
			emit KeystrokeDetected(keyboardSleepTime = 0);
		}
		else if (isPressed(XK_a) || isPressed(XK_b) || isPressed(XK_c) || isPressed(XK_d) || isPressed(XK_e) || isPressed(XK_f) || isPressed(XK_g) || isPressed(XK_h) || isPressed(XK_i) || isPressed(XK_j) || isPressed(XK_k) || isPressed(XK_l) || isPressed(XK_m) || isPressed(XK_n) || isPressed(XK_o) || isPressed(XK_p) || isPressed(XK_r) || isPressed(XK_s) || isPressed(XK_t) || isPressed(XK_u) || isPressed(XK_v) || isPressed(XK_y) || isPressed(XK_w) || isPressed(XK_z))
		{
			emit KeystrokeDetected(keyboardSleepTime = 0);
		}

		int a = ip.secondsIdle();
		if (a == 0)
			emit MouseclickDetected(mouseSleepTime = 0);

#endif
		++keyboardSleepTime;
		++mouseSleepTime;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

bool KeyboardMouseWatcher::isStopped()
{
	bool stopped;
	mutex.lock();
	stopped = this->stopped;
	mutex.unlock();
	return stopped;
}
