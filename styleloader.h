/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Stylesheet Loader
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


#ifndef STYLELOADER_H
#define STYLELOADER_H

#include <QObject>
#include <QtCore>
#include <QtGui>

class StyleLoader : public QObject
{
    Q_OBJECT

private:
    QFileSystemWatcher* fsw;
    QString c;
    QApplication* a;

private slots:
    void loadIt(QString s);

public:
    explicit StyleLoader(QApplication* app);

signals:

public slots:
    void loadStyleSheet(QString s);
};

#endif // STYLELOADER_H
