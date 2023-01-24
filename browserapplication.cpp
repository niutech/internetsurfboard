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

#include "browserapplication.h"
#include "mainwindow.h"
#include <QtGui>
#include <QtNetwork>
#include "networkaccessmanager.h"

BrowserApplication::BrowserApplication(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    //addPluginDirectory()

    updateReply = 0;

    setApplicationName("InternetSurfboard");
    setApplicationVersion("0.3");  // 0.002

    QDesktopServices::setUrlHandler("http", this, "openUrl");
    QDesktopServices::setUrlHandler("https", this, "openUrl");

    QString localservername = "internetsurfboard";

    {
        QLocalSocket localsocket;
        localsocket.connectToServer(localservername);
        if(localsocket.waitForConnected(4000))
        {
            QTextStream stream(&localsocket);
            if(argc > 1)
            {
                stream << argv[1];
                //localsocket.write(argv[1]);
                //localsocket.flush();
                localsocket.waitForBytesWritten(4000);
            } else {
                stream << QString();
            }
            stream.flush();
            localsocket.waitForBytesWritten();
            localsocket.disconnectFromServer();
            localsocket.waitForDisconnected();
            ::exit(0);
        }
    }

    localServer = new QLocalServer();
    connect(localServer, SIGNAL(newConnection()), this, SLOT(onLocalServerConnection()));
    localServer->listen(localservername);

    systemTrayIcon = new QSystemTrayIcon(QIcon(":/icon.png"));
    connect(systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    systemTrayIcon->show();

}

void BrowserApplication::checkForUpdates()
{
    QNetworkAccessManager *manager = NetworkAccessManager::getDefault();
    connect(manager, SIGNAL(finished(QNetworkReply*)),
         this, SLOT(updateRequestFinished(QNetworkReply*)));

    updateReply = manager->get(QNetworkRequest(QUrl("http://isurfupdate.ruppelmedia.com/" + QApplication::applicationVersion() + ".txt")));

    connect(updateReply, SIGNAL(finished()), this, SLOT(updateRequestFinished()));
}

void BrowserApplication::updateRequestFinished()
{
    QByteArray contents = updateReply->readAll().left(10000);
    QStringList tokens = QString::fromUtf8(contents).simplified().split(" ");
    if(tokens.length() > 2)
    {
        if(tokens.at(0) == "internetsurfboardnotify")
        {
            if(tokens.at(1) == "open")
            {
                openUrl(QUrl(tokens.at(2)));
            }
        }
    }
    qDebug(contents);
}

void BrowserApplication::onLocalServerConnection()
{
    QLocalSocket* socket = localServer->nextPendingConnection();
    if(socket == 0)
        return;
    //socket->waitForDisconnected(4000);
    //QByteArray a = socket->readAll();
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    //QString s = QString::fromAscii(a);
    //s = s.simplified();
    QString s = stream.readAll();
    //stream >> s;
    delete socket;

    //QUrl url(s);
    //if(url.isValid())
    if(s.length() > 0)
    {
        MainWindow* mw = topLevelBrowserWindow();
        mw->loadPageInNewTab(s);
        mw->show();
        mw->activateWindow();
        mw->raise();
    }
    else
    {
        MainWindow* mw = new MainWindow();
        mw->show();
        mw->appinit();
        mw->activateWindow();
        mw->raise();
    }
}

void BrowserApplication::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    MainWindow* mw = 0;
    switch(reason)
    {
    case QSystemTrayIcon::MiddleClick:
        mw = topLevelBrowserWindow();
        mw->addTab();
        mw->show();
        mw->activateWindow();
        mw->raise();
        break;
    case QSystemTrayIcon::DoubleClick:
        mw = new MainWindow();
        mw->show();
        mw->activateWindow();
        mw->raise();
        break;
    default:
        mw = topLevelBrowserWindow();
        mw->show();
        mw->activateWindow();
        mw->raise();
        break;
    }

}

BrowserApplication* theStaticBrowserApplication__xxx = 0;
BrowserApplication* BrowserApplication::getStatic()
{
    //if(theStaticBrowserApplication__xxx == 0)
    //    theStaticBrowserApplication__xxx = new BrowserApplication(0,0);
    return theStaticBrowserApplication__xxx;
}

BrowserApplication* BrowserApplication::createStatic(int & argc, char ** argv)
{
    if(theStaticBrowserApplication__xxx == 0)
        theStaticBrowserApplication__xxx = new BrowserApplication(argc, argv);
    return theStaticBrowserApplication__xxx;
}

MainWindow* BrowserApplication::topLevelBrowserWindow()
{
    MainWindow* mw = qobject_cast<MainWindow*>(QApplication::activeWindow());

    if(mw == 0)
    {
        foreach (QWidget *widget, QApplication::allWidgets())
        {
            MainWindow* mwx = qobject_cast<MainWindow*>(widget);
            if(mwx)
                mw = mwx;
        }
    }

    if(mw == 0)
        mw = new MainWindow();

    return mw;
}

void BrowserApplication::openUrl(const QUrl& url)
{
    topLevelBrowserWindow()->loadPageInNewTab(url.toString());
}
