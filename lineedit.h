/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Extended Line Edit Class
**   > URL Line Edit Class
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


#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include <QCompleter>

class LineEdit : public QLineEdit
{
    Q_OBJECT

private:
    bool selall;

public:
    explicit LineEdit(QWidget *parent = 0);

protected:
    void mouseReleaseEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void contextMenuEvent(QContextMenuEvent* ev);
    void keyPressEvent(QKeyEvent* event);

signals:
    void escapePressed();

public slots:

};

class UrlCompleter : public QCompleter
{
    Q_OBJECT

public:
    explicit UrlCompleter(QObject* parent = 0);

protected:
    QStringList splitPath (const QString& path) const;
    QString pathFromIndex ( const QModelIndex & index ) const;
};

class UrlLineEdit : public LineEdit
{
    Q_OBJECT

public:
    explicit UrlLineEdit(QWidget *parent = 0);

private:
    UrlCompleter* completer;

private slots:
    void onTextChanged(QString txt);
};

#endif // LINEEDIT_H
