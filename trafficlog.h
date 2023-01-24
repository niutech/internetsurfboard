/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Traffic Log
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


#ifndef TRAFFICLOG_H
#define TRAFFICLOG_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QModelIndex>

namespace Ui {
    class TrafficLog;
}

class TrafficLog : public QMainWindow
{
    Q_OBJECT

public:
    explicit TrafficLog(QWidget *parent = 0);
    ~TrafficLog();

private:
    Ui::TrafficLog *ui;

private slots:
    void on_actionTopmost_triggered();
    void on_tableWidget_doubleClicked(QModelIndex index);
    void on_actionClear_triggered();
    void onReplyFinished(QNetworkReply * reply);
    void onReplyStarted(QNetworkReply * reply);
};

#endif // TRAFFICLOG_H
