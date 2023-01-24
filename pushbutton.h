/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Extended Pushbutton
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


#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>
#include <QIcon>
#include <QWidget>

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButton(QWidget *parent = 0);
    PushButton(const QString& text, QWidget* parent = 0);
    PushButton(const QIcon& icon, const QString& text, QWidget* parent = 0);

signals:
    void lmb();
    void rmb();
    void mmb();

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent* e);
};

#endif // PUSHBUTTON_H
