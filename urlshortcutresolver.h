/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > URL Shortcut Resolver
**      - resolves things like 'wikipedia car' typed into the url bar
**      - manages url context menu items
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


#ifndef URLSHORTCUTRESOLVER_H
#define URLSHORTCUTRESOLVER_H

#include <QObject>
#include <QHash>
#include <QPair>
#include <QString>
#include <QStringList>

class UrlShortcutResolver : public QObject
{
    Q_OBJECT
public:
    explicit UrlShortcutResolver(QObject *parent = 0);

    static UrlShortcutResolver* instance();

    QString resolve(QString s);

signals:

public slots:

    const QStringList& suggestionTitles();
    const QStringList& suggestions();

private:
    QHash<QString, QPair<QString, QString> > map;
    QStringList _suggestions;
    QStringList _suggestionTitles;
};

#endif // URLSHORTCUTRESOLVER_H
