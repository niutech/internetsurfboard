/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > JavaScript Console
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


#ifndef JAVASCRIPTCONSOLE_H
#define JAVASCRIPTCONSOLE_H

#include <QMainWindow>

namespace Ui {
    class JavaScriptConsole;
}

class JavaScriptConsole : public QMainWindow
{
    Q_OBJECT

public:
    explicit JavaScriptConsole(QWidget *parent = 0);
    ~JavaScriptConsole();

    void log(const QString & message, int lineNumber, const QString & sourceID);

    static JavaScriptConsole* getStatic();

private:
    Ui::JavaScriptConsole *ui;

private slots:
    void on_actionClear_triggered();
};

#endif // JAVASCRIPTCONSOLE_H
