/*
 * idle_x11.cpp - detect desktop idle time
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "idle.h"

// gajim uses 'org.gnome.Mutter.IdleMonitor'.
// we can also use https://blog.sleeplessbeastie.eu/2013/02/26/how-to-automate-kde-using-d-bus/
//   in short qdbus org.kde.screensaver /ScreenSaver GetSessionIdleTime
//   but on kde it returns in ms while gnome is in secs https://bugs.kde.org/show_bug.cgi?id=313571

#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>

static XErrorHandler old_handler = 0;
extern "C" int       xerrhandler(Display *dpy, XErrorEvent *err)
{
    if (err->error_code == BadDrawable)
        return 0;

    return (*old_handler)(dpy, err);
}

class IdlePlatform::Private {
public:
    Private() { }

    XScreenSaverInfo *ss_info = nullptr;
};

IdlePlatform::IdlePlatform() { d = new Private; }

IdlePlatform::~IdlePlatform()
{
    if (d->ss_info)
        XFree(d->ss_info);
    if (old_handler) {
        XSetErrorHandler(old_handler);
        old_handler = 0;
    }
    delete d;
}

bool IdlePlatform::init()
{
    if (!QX11Info::isPlatformX11())
        return false;

    if (d->ss_info)
        return true;

    old_handler = XSetErrorHandler(xerrhandler);

    int event_base, error_base;
#if !defined(LIMIT_X11_USAGE)
    if (XScreenSaverQueryExtension(QX11Info::display(), &event_base, &error_base)) {
        d->ss_info = XScreenSaverAllocInfo();
        return true;
    }
#endif
    return false;
}

int IdlePlatform::secondsIdle()
{
    if (!d->ss_info)
        return 0;
    if (!XScreenSaverQueryInfo(QX11Info::display(), QX11Info::appRootWindow(), d->ss_info))
        return 0;
    return d->ss_info->idle / 1000;
}
