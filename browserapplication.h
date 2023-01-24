/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Application Main Class
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


#ifndef BROWSERAPPLICATION_H
#define BROWSERAPPLICATION_H

#include <QApplication>
#include <QUrl>
#include <QSystemTrayIcon>
#include "mainwindow.h"
#include <QLocalServer>

class BrowserApplication : public QApplication
{
    Q_OBJECT
public:
    explicit BrowserApplication(int& argc, char** argv);

    static BrowserApplication* createStatic(int& argc, char** argv);
    static BrowserApplication* getStatic();

    MainWindow* topLevelBrowserWindow();

    void checkForUpdates();

signals:

public slots:
    void openUrl(const QUrl& url);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);


protected slots:
    void onLocalServerConnection();
    void updateRequestFinished();

private:
    QSystemTrayIcon* systemTrayIcon;
    QLocalServer* localServer;
    QNetworkReply* updateReply;
};

#endif // BROWSERAPPLICATION_H
