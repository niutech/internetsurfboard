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

#include "settings.h"
#include "networkaccessmanager.h"
#include <QtNetwork>
#include <QDesktopServices>

Settings::Settings(QObject *parent) :
        QSettings(QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).absoluteFilePath("settings.ini"), QSettings::IniFormat, parent)
{
    if(value("general/home").toString().isEmpty())
        setValue("general/home", "http://google.com");
}

Settings* xxTheStaticSettings = 0;
Settings* Settings::getStatic()
{
    if( xxTheStaticSettings == 0 )
        xxTheStaticSettings = new Settings();
    return xxTheStaticSettings;
}


void Settings::apply()
{
    QWebSettings* s = QWebSettings::globalSettings();
    s->setAttribute(QWebSettings::JavascriptEnabled, value("javascript/enabled", true).toBool());
    s->setAttribute(QWebSettings::PluginsEnabled, value("plugins/enabled", true).toBool());
    s->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    s->setAttribute(QWebSettings::ZoomTextOnly, false);

    QNetworkProxy proxy = QNetworkProxy::applicationProxy();
    proxy.setType((QNetworkProxy::ProxyType) value("proxy/type", (int)proxy.type()).toInt());
    proxy.setHostName(value("proxy/host", proxy.hostName()).toString());
    proxy.setPort((quint16)value("proxy/port", proxy.port()).toInt());
    proxy.setUser(value("proxy/user", proxy.user()).toString());
    proxy.setPassword(value("proxy/password", proxy.password()).toString());
    QNetworkProxy::setApplicationProxy(proxy);
}


void Settings::loadDefault()
{
    setValue("javascript/enabled", true);
    setValue("plugins/enabled", true);
    setValue("proxy/type", QNetworkProxy::DefaultProxy);
    setValue("general/home", "http://google.com");
}
