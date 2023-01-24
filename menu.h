/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Menu Classes (Extended Menu, Bookmark Menu, Bookmark Contextmenu)
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


#ifndef MENU_H
#define MENU_H

#include <QtGui>

class ExtendedMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ExtendedMenu(QWidget *parent = 0);
    QWidget* getMainWindow();
    Qt::MouseButtons mouseButtons();

protected:
    void leaveEvent(QEvent* e);

private slots:
    void onShow();
    void onHideTimeOut();

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

private:
    Qt::MouseButtons _mouseButtons;

signals:
    void lmb();
    void rmb();
    void mmb();
    void lmbToolTip(QString toolTop);
    void rmbToolTip(QString toolTop);
    void mmbToolTip(QString toolTop);
};



class BookmarkMenu : public ExtendedMenu
{
    Q_OBJECT
public:
    explicit BookmarkMenu(QWidget *parent = 0);

signals:

public slots:
    void updateItems();
    void onClose();

protected:
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

private:
    QPoint lastMousePos;
    bool ignoreButtonRelease;
    bool nodrag;
    QAction* addBookmarkAction;
};



class BookmarkEntryMenu : public ExtendedMenu
{
    Q_OBJECT

public:
    BookmarkEntryMenu(QString url, QWidget *parent = 0);

private:
    QString bookmarkUrl;

protected slots:
    void onOpen();
    void onOpenNewTab();
    void onOpenNewWindow();
    void onRemove();
};

/*
class BookmarkMenu : public QMenu
{
    Q_OBJECT
public:
    explicit BookmarkMenu(QWidget *parent = 0);
    QWidget* getMainWindow();

signals:

public slots:
    void onAboutToShow();
    void onAboutToHide();

protected:

private:
    QVBoxLayout* layout;
    QListWidget* bookmarkList;
    QPushButton* addButton;
};
*/
#endif // MENU_H
