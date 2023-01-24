/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Windows Vista/7 DWM (Desktop Window Manager) Wrapper
**   > will just do nothing on other platforms
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


#ifndef DESKTOPWINDOWMANAGER_H
#define DESKTOPWINDOWMANAGER_H

#include <QWidget>

class DesktopWindowManager
{
    DesktopWindowManager();

public:
    static bool init();
    static bool isSupported();
    static bool extendFrameIntoClientArea(QWidget* w, int top, int right, int bottom, int left);
    static bool makeGlass(QWidget* w);
    static bool setBlurBehind(QWidget* w, bool enable = true);
};

#endif // DESKTOPWINDOWMANAGER_H
