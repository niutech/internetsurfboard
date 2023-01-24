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

#include "lineedit.h"
#include "menu.h"
#include "urlshortcutresolver.h"

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}


void LineEdit::mouseReleaseEvent(QMouseEvent* e)
{
    if(selectedText().length() == 0 && selall /*cursorPosition() < text().length()*/)
        selectAll();
    QLineEdit::mouseReleaseEvent(e);
}

void LineEdit::mousePressEvent(QMouseEvent* e)
{
    selall = selectedText().length() == 0;
    QLineEdit::mousePressEvent(e);
}


void LineEdit::contextMenuEvent(QContextMenuEvent* ev)
{
    ExtendedMenu* menu = new ExtendedMenu(this);
    //menu->addAction(/*actions()*/ "gaga");

    QMenu* stdmn = QLineEdit::createStandardContextMenu();
    menu->addActions(stdmn->actions());

    menu->popup(ev->globalPos());
}

void LineEdit::keyPressEvent(QKeyEvent* event)
{
    QLineEdit::keyPressEvent(event);
    if(event->key() == Qt::Key_Escape)
        emit escapePressed();
}









UrlLineEdit::UrlLineEdit(QWidget *parent) : LineEdit(parent)
{
    completer = new UrlCompleter(this);

    setPlaceholderText(tr("URL"));

    QStringList l;
    //l << "gaga" << "bla" << "blubb";
    l = UrlShortcutResolver::instance()->suggestionTitles();
    completer->setModel(new QStringListModel(l, this));
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(completer);

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    //connect(completer, SIGNAL(highlighted(QString)), this, SLOT(setText(QString)));
}

UrlCompleter::UrlCompleter(QObject* parent) : QCompleter(parent)
{

}

QStringList UrlCompleter::splitPath (const QString& path) const
{
    //return QCompleter::splitPath(path);
    //return path.split(" ");

    return QStringList();
}

QString UrlCompleter::pathFromIndex ( const QModelIndex & index ) const
{
    QString s = UrlShortcutResolver::instance()->suggestions().at(index.row());
    s += " " + completionPrefix();
    return s;
}

void UrlLineEdit::onTextChanged(QString txt)
{
    /*QStringList l;
    //if(!txt.endsWith(" gaga"))
    //    l << (txt + " gaga");
    if(!txt.startsWith("google"))
        l << "google " + txt;
    if(!txt.startsWith("wikipedia"))
        l << "wikipedia " + txt;
    completer->setModel(new QStringListModel(l, this));*/
}
