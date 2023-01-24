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

#include "urlshortcutresolver.h"

#include <QtCore>
#include <QtGui>

UrlShortcutResolver::UrlShortcutResolver(QObject *parent) :
    QObject(parent)
{

    //QFile f(QDir(QApplication::applicationDirPath()).absoluteFilePath("shortcuts.cfg"));

    {
        QFile f(QDir(QApplication::applicationDirPath()).absoluteFilePath("shortcuts.cfg"));

        f.open(QFile::ReadOnly);

        QByteArray all = f.readAll();

        //qDebug(all);

        QStringList lines = QString::fromUtf8(all).split("\n");

        foreach(QString s, lines)
        {
            QStringList l = s.simplified().split(" ");
            if(l.count() >= 3)
            {
                map.insert(l[0], qMakePair(l[1], l[2]));

                qDebug(l[0].toAscii() + " " + l[1].toAscii() + " " + l[2].toAscii());
            }
        }

        f.close();
    }

    {
        QFile f(QDir(QApplication::applicationDirPath()).absoluteFilePath("suggestions.cfg"));
        f.open(QFile::ReadOnly);

        //_completions = f.readAll().simplified().split("\n");

        foreach(QString s, f.readAll().split('\n'))
        {
            QStringList l = s.simplified().split(" ");
            if(l.length() > 1)
            {
                _suggestions << l.first();
                l.removeFirst();
                _suggestionTitles << l.join(" ");
            }
        }

        f.close();
    }
}

const QStringList& UrlShortcutResolver::suggestions()
{
    return _suggestions;
}
const QStringList& UrlShortcutResolver::suggestionTitles()
{
    return _suggestionTitles;
}

QString UrlShortcutResolver::resolve(QString s)
{
    QStringList parts = s.simplified().split(" ");
    if(parts.count() == 1)
        return map.value(parts.at(0).toLower()).first;
    else if(parts.count() > 1)
    {
        QString s = map.value(parts.at(0).toLower()).second;
        parts.removeFirst();
        QString x = parts.join(" ");
        s.replace("{puny}", QString::fromAscii(QUrl::toPunycode(x).toPercentEncoding()));
        s.replace("{ascii}", QString::fromAscii(x.toAscii().toPercentEncoding()));
        s.replace("{utf8}", QString::fromAscii(x.toUtf8().toPercentEncoding()));
        s.replace("{latin1}", QString::fromAscii(x.toLatin1().toPercentEncoding()));
        QLocale l;
        //s.replace("{lang}", QLocale::languageToString(l.language()));
        //s.replace("{language}", QLocale::languageToString(l.language()));
        //s.replace("{country}", QLocale::countryToString(l.country()));
        s.replace("{lang}", l.name().left(2).toLower());
        s.replace("{language}", l.name().left(2).toLower());
        s.replace("{country}", l.name().right(2).toLower());
        return s;
    }
    return QString::null;
}

UrlShortcutResolver* ___xxx_theUrlShortcutResolverInstance = 0;
UrlShortcutResolver* UrlShortcutResolver::instance()
{
    if(___xxx_theUrlShortcutResolverInstance == 0)
        ___xxx_theUrlShortcutResolverInstance = new UrlShortcutResolver();
    return ___xxx_theUrlShortcutResolverInstance;
}
