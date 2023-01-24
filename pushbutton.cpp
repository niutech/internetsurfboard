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

#include "pushbutton.h"
#include <QtGui>

PushButton::PushButton(QWidget *parent) :
    QPushButton(parent)
{

}

PushButton::PushButton(const QString& text, QWidget* parent) :
        QPushButton(text, parent)
{

}

PushButton::PushButton(const QIcon& icon, const QString& text, QWidget* parent) :
        QPushButton(icon, text, parent)
{

}


void PushButton::mouseReleaseEvent(QMouseEvent* e)
{
    if(hitButton(e->pos()))
    {
        switch(e->button())
        {
        case Qt::LeftButton:
            emit lmb();
            break;
        case Qt::RightButton:
            emit rmb();
            break;
        case Qt::MiddleButton:
            emit mmb();
            break;
        }
    }
    QPushButton::mouseReleaseEvent(e);
}
