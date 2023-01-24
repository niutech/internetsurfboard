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

#include <QtGui/QApplication>
#include <QtWebKit>
#include "mainwindow.h"
#include "bookmarks.h"
#include "styleloader.h"
#include "browserapplication.h"
#include "downloadmanager.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    BrowserApplication* a = BrowserApplication::createStatic(argc, argv);
    a->setQuitOnLastWindowClosed(true);

    //a->setOrganizationName("weltweitimnetz");

    /*QWebSettings* s = QWebSettings::globalSettings();
    s->setAttribute(QWebSettings::JavascriptEnabled, true);
    s->setAttribute(QWebSettings::PluginsEnabled, true);
    s->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    s->setAttribute(QWebSettings::ZoomTextOnly, false);*/
    //s->setAttribute(QWebSettings::XSSAuditingEnabled, true);

    /*QWebSettings* s = QWebSettings::globalSettings();
    QDir().mkdir("cache");
    s->setIconDatabasePath("cache");
    s->setLocalStoragePath("cache");
    s->setOfflineStoragePath("cache");
    s->setOfflineWebApplicationCachePath("cache");*/

    Settings::getStatic()->apply();


    StyleLoader l(a);
    l.loadStyleSheet(":/style/stylesheet.css");

    MainWindow w;
    w.show();
    w.appinit();
    //w.loadPage("google.com");

    a->checkForUpdates();

    if(argc > 0)
    {
        MainWindow* mw = a->topLevelBrowserWindow();
        mw->loadPage(QString::fromAscii(argv[1]));
    }

    int ret = a->exec();
    DownloadManager::getStatic()->close();
    BookmarkList::globalBookmarkList()->save();



    return ret;
}
