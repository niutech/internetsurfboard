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

#include "tabbar.h"

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
}

QSize TabBar::sizeHint() const
{
    QSize s = QTabBar::sizeHint();
    //s.setWidth(2);
    return s;
}


void TabBar::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        int i = tabAt(event->pos());
        if(i >= 0)
        {
            emit tabRightClicked(i);
            emit tabRightClicked(i, event->globalPos());
        }
    } else if(event->button() == Qt::MidButton)
    {
        int i = tabAt(event->pos());
        if(i >= 0)
            emit tabMbClicked(i);
    }
    QTabBar::mousePressEvent(event);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        int i = tabAt(event->pos());
        if(i >= 0)
            emit tabDoubleClicked(i);
    }
    QTabBar::mouseDoubleClickEvent(event);
}
