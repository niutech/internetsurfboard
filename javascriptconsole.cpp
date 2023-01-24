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

#include "javascriptconsole.h"
#include "ui_javascriptconsole.h"
#include <QtGui>
#include "desktopwindowmanager.h"

JavaScriptConsole::JavaScriptConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JavaScriptConsole)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(2);

    QStringList h;
    h << "Source";
    h << "Message";
    ui->tableWidget->setHorizontalHeaderLabels(h);

    ui->actionAutoScroll->setChecked(true);

    setWindowFlags(/*Qt::Tool |*/ windowFlags() | Qt::WindowStaysOnTopHint);
    //setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::extendFrameIntoClientArea(this, 50, 20, 20, 20));
}

JavaScriptConsole::~JavaScriptConsole()
{
    delete ui;
}

void JavaScriptConsole::log(const QString & message, int lineNumber, const QString & sourceID)
{
    if(this->isVisible())
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(row + 1);

        QTableWidgetItem* i = 0;

        i = new QTableWidgetItem();
        i->setText(sourceID + " [" + lineNumber + "]");
        ui->tableWidget->setItem(row, 0, i);

        i = new QTableWidgetItem();
        i->setText(message);
        ui->tableWidget->setItem(row, 1, i);

        if(ui->actionAutoScroll->isChecked())
            ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->maximum());
    }
}

JavaScriptConsole* XXTheJScriptConsole = 0;
JavaScriptConsole* JavaScriptConsole::getStatic()
{
    if(XXTheJScriptConsole == 0)
        XXTheJScriptConsole = new JavaScriptConsole();
    return XXTheJScriptConsole;
}

void JavaScriptConsole::on_actionClear_triggered()
{
    ui->tableWidget->setRowCount(0);
}
