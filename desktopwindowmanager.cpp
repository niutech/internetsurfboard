/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**  Copyright (C) 2010  Philipp Ruppel
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "desktopwindowmanager.h"

#include <QLibrary>

DesktopWindowManager::DesktopWindowManager()
{
}

struct xxdwmextBlurBehindFlags
{
  unsigned long dwFlags;
  int  fEnable;
  void*  hRgnBlur;
  int  fTransitionOnMaximized;
};

typedef long __stdcall xxdwmextExtendFrameIntoClientAreaFunction(void* hWnd,const int* margins);
typedef long __stdcall xxdwmextEnableBlurBehindWindowFunction(void* hWnd, void* pBlurBehind);

bool xxdwmextInited = false;
xxdwmextExtendFrameIntoClientAreaFunction* xxdwmextExtendFrameIntoClientArea = 0;
xxdwmextEnableBlurBehindWindowFunction* xxdwmextEnableBlurBehindWindow = 0;

bool DesktopWindowManager::init()
{
    if(!xxdwmextInited)
    {
        xxdwmextInited = true;

        QLibrary dwmapi("dwmapi");
        if(dwmapi.load())
        {
            qDebug("dwmapi.dll found and opened");
            xxdwmextExtendFrameIntoClientArea = (xxdwmextExtendFrameIntoClientAreaFunction*)dwmapi.resolve("DwmExtendFrameIntoClientArea");
            xxdwmextEnableBlurBehindWindow = (xxdwmextEnableBlurBehindWindowFunction*)dwmapi.resolve("DwmEnableBlurBehindWindow");
        } else {
            qDebug("dwmapi.dll not availible");
        }
    }
    return xxdwmextExtendFrameIntoClientArea != 0;
}

bool DesktopWindowManager::isSupported()
{
    init();
    return xxdwmextExtendFrameIntoClientArea != 0;
}

bool DesktopWindowManager::extendFrameIntoClientArea(QWidget* w, int top, int right, int bottom, int left)
{
    init();
    if(xxdwmextExtendFrameIntoClientArea)
    {
        int margins[4] = { left,right,top,bottom };
        xxdwmextExtendFrameIntoClientArea(w->winId(), margins);
        return true;
    } else {
        return false;
    }
}

bool DesktopWindowManager::makeGlass(QWidget* w)
{
    init();
    if(xxdwmextExtendFrameIntoClientArea)
    {
        int margins[4] = { -1, -1, -1, -1 };
        xxdwmextExtendFrameIntoClientArea(w->winId(), margins);
        return true;
    } else {
        return false;
    }
}

bool DesktopWindowManager::setBlurBehind(QWidget* w, bool enable)
{
    init();
    if(xxdwmextEnableBlurBehindWindow)
    {
        xxdwmextBlurBehindFlags flgs;
        flgs.dwFlags = 0x00000001;
        flgs.fEnable = enable;
        flgs.fTransitionOnMaximized = FALSE;
        flgs.hRgnBlur = 0;

        xxdwmextEnableBlurBehindWindow(w->winId(), &flgs);

        return true;
    } else {
        return false;
    }
}
