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

#include "gripwidget.h"

GripWidget::GripWidget(QWidget *parent) :
    QWidget(parent)
{

}


void GripWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() == Qt::LeftButton && ((window()->windowState() & Qt::WindowMaximized) == 0))
    {
        window()->move(event->globalPos().x() + _offset_x, event->globalPos().y() + _offset_y);
    }
    QWidget::mouseMoveEvent(event);
}

void GripWidget::mousePressEvent(QMouseEvent* event)
{
    _offset_x = window()->x() - event->globalPos().x();
    _offset_y = window()->y() - event->globalPos().y();
    QWidget::mousePressEvent(event);
}

void GripWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    window()->setWindowState(window()->windowState() ^ Qt::WindowMaximized);
    QWidget::mouseDoubleClickEvent(event);
}
