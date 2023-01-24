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

#include "styleloader.h"
#include <QtGui>

StyleLoader::StyleLoader(QApplication* app)
{
    fsw = new QFileSystemWatcher(this);
    c = QString::null;
    connect(fsw, SIGNAL(fileChanged(QString)), this, SLOT(loadIt(QString)));
    a = app;
}


void StyleLoader::loadStyleSheet(QString s)
{
    if(!c.isEmpty())
        fsw->removePath(c);

    c = s;

    fsw->addPath(c);

    loadIt(c);
}


void StyleLoader::loadIt(QString s)
{
    QFile f(c);
    f.open(QFile::ReadOnly);
    a->setStyleSheet(QString::fromUtf8(f.readAll()));
    f.close();
}
