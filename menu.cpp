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

#include "menu.h"
#include "bookmarks.h"
#include "mainwindow.h"
#include "desktopwindowmanager.h"
#include "settings.h"
#include <QtGui>



ExtendedMenu::ExtendedMenu(QWidget *parent) : QMenu(parent)
{
    connect(this, SIGNAL(aboutToShow()), this, SLOT(onShow()));
}

void ExtendedMenu::onShow()
{
    //setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::makeGlass(this));

    bool blurbehind = DesktopWindowManager::setBlurBehind(this, true);
    setAttribute(Qt::WA_TranslucentBackground, blurbehind);
    setProperty("blurbehind", blurbehind);
}
void ExtendedMenu::leaveEvent(QEvent* e)
{
    QMenu::leaveEvent(e);
    //QTimer::singleShot(350, this, SLOT(onHideTimeOut()));
}
void ExtendedMenu::onHideTimeOut()
{
    if(!underMouse())
        hide();
}
QWidget* ExtendedMenu::getMainWindow()
{
    MainWindow* mw = 0;
    for(QWidget* w = this; w; w = w->parentWidget())
    {
        MainWindow* mw2 = qobject_cast<MainWindow*>(w);
        if(mw2)
            mw = mw2;
    }
    return mw;
}



void ExtendedMenu::mousePressEvent(QMouseEvent* e)
{
    QMenu::mousePressEvent(e);
    _mouseButtons = e->buttons();
}

void ExtendedMenu::mouseReleaseEvent(QMouseEvent* e)
{
    QAction* a = actionAt(e->pos());
    if(a)
    {
        switch(e->button())
        {
        case Qt::LeftButton:
            emit lmb();
            emit lmbToolTip(a->toolTip());
            break;
        case Qt::RightButton:
            emit rmb();
            emit rmbToolTip(a->toolTip());
            break;
        case Qt::MidButton:
            emit mmb();
            emit mmbToolTip(a->toolTip());
            break;
        }
    }

    QMenu::mouseReleaseEvent(e);
    _mouseButtons = e->buttons();


}
Qt::MouseButtons ExtendedMenu::mouseButtons()
{
    return _mouseButtons;
}





BookmarkEntryMenu::BookmarkEntryMenu(QString url, QWidget *parent) : ExtendedMenu(parent)
{
    bookmarkUrl = url;
    addAction(tr("Open") + " " + url.left(20) + "...", this, SLOT(onOpen()));
    addAction(tr("Open in New Tab"), this, SLOT(onOpenNewTab()));
    addAction(tr("Open in New Window"), this, SLOT(onOpenNewWindow()));
    addSeparator();
    addAction(tr("Remove"), this, SLOT(onRemove()));
}


void BookmarkEntryMenu::onOpen()
{
    MainWindow* mw = qobject_cast<MainWindow*>(getMainWindow());
    if(mw)
        mw->loadPage(bookmarkUrl);
    QMenu* m = qobject_cast<QMenu*>(parentWidget());
    if(m)
        m->close();
}

void BookmarkEntryMenu::onOpenNewTab()
{
    MainWindow* mw = qobject_cast<MainWindow*>(getMainWindow());
    if(mw)
        mw->loadPageBackground(bookmarkUrl);
    /*QMenu* m = qobject_cast<QMenu*>(parentWidget());
    if(m)
        m->close();*/
}

void BookmarkEntryMenu::onOpenNewWindow()
{
    MainWindow* mw = new MainWindow();
    mw->show();
    mw->loadPage(bookmarkUrl);
    QMenu* m = qobject_cast<QMenu*>(parentWidget());
    if(m)
        m->close();
}

void BookmarkEntryMenu::onRemove()
{
    BookmarkList::globalBookmarkList()->remove(bookmarkUrl);

    BookmarkMenu* m = qobject_cast<BookmarkMenu*>(parentWidget());
    if(m)
    {
        m->updateItems();
        QTimer::singleShot(600, m, SLOT(hide()));
    }
}




BookmarkMenu::BookmarkMenu(QWidget *parent) :
    ExtendedMenu(parent)
{
    connect(this, SIGNAL(aboutToShow()), this, SLOT(updateItems()));
    setSeparatorsCollapsible(true);

    addBookmarkAction = 0;
    ignoreButtonRelease = true;

    connect(this, SIGNAL(aboutToHide()), this, SLOT(onClose()));


}

void BookmarkMenu::onClose()
{
    ignoreButtonRelease = true;
}



void BookmarkMenu::mouseMoveEvent(QMouseEvent* e)
{
    {
        QAction* a = actionAt(lastMousePos);
        QAction* b = actionAt(e->pos());

        /*MainWindow* mw = qobject_cast<MainWindow*>(getMainWindow());
        if(mw)
        {
            mw->statusBar()->showMessage(b->toolTip());
        }*/

        if(a && b && (a != b))
        {
            if(e->buttons() == Qt::LeftButton)
            {
                if(!a->data().isNull() && !b->data().isNull() && nodrag == false)
                {

                        ignoreButtonRelease = true;

                        qDebug(a->data().toString().toAscii());
                        qDebug(b->data().toString().toAscii());

                        BookmarkList* l = BookmarkList::globalBookmarkList();
                        l->swap(a->data().toString(), b->data().toString());

                        QVariant d = a->data();
                        QString s = a->text();
                        QString t = a->statusTip();
                        QString t2 = a->toolTip();

                        a->setText(b->text());
                        a->setData(b->data());
                        a->setStatusTip(b->statusTip());
                        a->setToolTip(b->toolTip());

                        b->setData(d);
                        b->setText(s);
                        b->setStatusTip(t);
                        b->setToolTip(t2);
                } else {
                    nodrag = true;
                    ignoreButtonRelease = true;
                }
            }
        }
    }
    lastMousePos = e->pos();
    QMenu::mouseMoveEvent(e);
}

void BookmarkMenu::mousePressEvent(QMouseEvent* e)
{
    lastMousePos = e->pos();
    ExtendedMenu::mousePressEvent(e);
    ignoreButtonRelease = false;
    nodrag = false;
}

void BookmarkMenu::mouseReleaseEvent(QMouseEvent* e)
{
    lastMousePos = e->pos();

    if(!ignoreButtonRelease)
    {
        MainWindow* mw = qobject_cast<MainWindow*>(getMainWindow());
        if(mw)
        {
            QAction* a = actionAt(e->pos());
            if(a == addBookmarkAction)
            {
                mw->bookmark();
                updateItems();
                QTimer::singleShot(600, this, SLOT(hide()));
                return;
            } else if(a)
            {
                QString url = a->data().toString();
                if(!url.isEmpty())
                {
                    if(e->button() == Qt::LeftButton)
                    {
                        mw->loadPage(url);
                        close();
                        return;
                    }
                    else if(e->button() == Qt::MidButton)
                    {
                        mw->addTab();
                        mw->loadPage(url);
                        close();
                        return;
                    } else if(e->button() == Qt::RightButton)
                    {
                        BookmarkEntryMenu* m = new BookmarkEntryMenu(url, this);
                        m->popup(e->globalPos());
                    }
                }
            }
        }

        ExtendedMenu::mouseReleaseEvent(e);

        ignoreButtonRelease = true;
    }
}

void BookmarkMenu::updateItems()
{
    clear();

    {
        QAction* a = new QAction(this);
        a->setText("Home");
        a->setData(Settings::getStatic()->value("general/home").toString());
        a->setStatusTip(Settings::getStatic()->value("general/home").toString());
        a->setToolTip(Settings::getStatic()->value("general/home").toString());
        addAction(a);
    }

    addBookmarkAction = new QAction(tr("Add Bookmark"), this);
    addBookmarkAction->setStatusTip(tr("Bookmark current page ..."));
    addAction(addBookmarkAction);
    addSeparator();

    BookmarkList* l = BookmarkList::globalBookmarkList();
    for(int i = 0; i < l->count(); i++)
    {
        QAction* a = new QAction(this);
        a->setText(l->title(i));
        a->setData(l->url(i));
        a->setStatusTip(l->url(i));
        a->setToolTip(l->url(i));
        addAction(a);
    }

    ignoreButtonRelease = true;
}



































/*
BookmarkMenu::BookmarkMenu(QWidget *parent) :
    QMenu(parent)
{
    clear();
    layout = new QVBoxLayout(this);
    bookmarkList = new QListWidget(this);
    addButton = new QPushButton("Add Bookmark", this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(bookmarkList);
    layout->addWidget(addButton);
    setLayout(layout);
    this->setFixedSize(270, 300);
    bookmarkList->setSelectionMode(QAbstractItemView::SingleSelection);
    bookmarkList->setDragEnabled(true);
    bookmarkList->viewport()->setAcceptDrops(true);
    bookmarkList->setDropIndicatorShown(true);
    bookmarkList->setDragDropMode(QAbstractItemView::InternalMove);

    connect(this, SIGNAL(aboutToShow()), this, SLOT(onAboutToShow()));
    connect(this, SIGNAL(aboutToHide()), this, SLOT(onAboutToHide()));
}

void BookmarkMenu::onAboutToShow()
{
    bookmarkList->clear();

    BookmarkList* l = BookmarkList::globalBookmarkList();

    for(int i = 0; i < l->count(); i++)
    {
        QListWidgetItem* it = new QListWidgetItem(bookmarkList);
        it->setText(l->title(i));
        it->setToolTip(l->url(i));
    }
}

void BookmarkMenu::onAboutToHide()
{
    QList<QPair<QString, QString> > l;

    for(int i = 0; i < bookmarkList->count(); i++)
    {
        l.append(QPair<QString, QString>(bookmarkList->item(i)->toolTip(), bookmarkList->item(i)->text()));
    }

    BookmarkList::globalBookmarkList()->setData(l);
}
*/
