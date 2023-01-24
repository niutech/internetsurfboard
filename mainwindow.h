/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Mainwindow Class
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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "webview.h"
#include "webpage.h"
#include "lineedit.h"
#include "tabbar.h"
#include "pushbutton.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    WebView* getCurrentWebView();
    void setTabTitle(QWidget* tab, QString title);
    QTabBar* documentTabBar();

private slots:
    void onCurrentTabChanged(int);
    void onUrlChanged(QUrl);

    void onFindChanged(QString);
    void onFindNext();
    void onFindPrevious();
    void onFindClose();

    void onUrlEscape();

    void onTabRightClick(int i, QPoint pos);


    QString _dumpElement(const QWebElement& e, int indent = 0);

public slots:
    void triggerAction(QWebPage::WebAction action, bool checked = false);
    void addTab();
    void appendTab();
    void go();
    void back();
    void forward();
    void reload();
    void abort();
    void loadPage(QString url, WebView* wv = 0);
    void loadPageBackground(QString url);
    void loadPageInNewTab(QString url);
    void updateNavigationBar();
    void closeTab(int);
    void closeCurrentTab();
    void bookmark();
    void newWindow();
    void copy();
    void paste();
    void cut();
    void inspect();
    void undo();
    void redo();
    void selectNextTab();
    void selectPreviousTab();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void showTrafficLog();
    void showJavaScriptConsole();
    void viewSource();
    void setStatus(QString stat);
    void print();
    void exportPDF();
    void exportSVG();
    void exportImage();
    void browseFileOpen();
    void renderTreeDump();
    void download();
    void findText();
    void downloadCurrentPage();
    void showSettingsDialog();
    void showAboutBox();
    void dockTabOff(int i);
    void dockCurrentTabOff();
    void dublicateTab(int i);
    void dublicateCurrentTab();
    void dumpNodeTree();
    void goBackInNewTab();
    void goForwardInNewTab();
    void goInNewTab();
    void showBackMenu();
    void showForwardMenu();
    void appinit();

protected:
    void closeEvent(QCloseEvent *event);

private:
    QVBoxLayout* mainLayout;
    QHBoxLayout* tabBarLayout;
    QHBoxLayout* navigationBarLayout;
    QStackedLayout* pageStackLayout;

    TabBar* tabBar;
    PushButton* tabButton;

    LineEdit* urlBox;
    PushButton* bwdButton;
    PushButton* fwdButton;
    PushButton* rldButton;
    PushButton* favButton;
    PushButton* goButton;
    PushButton* xButton;

    QWidget* centralWidget;

    QMenu* bookmarkMenu;
    QMenu* xMenu;

    QWidget* naviWidget;
    QWidget* tabContainer;
    QWidget* pageStackContainer;

    QLabel* statusLabel;

    QWidget* findWidget;
    QHBoxLayout* findLayout;
    QPushButton* findClose;
    LineEdit* findInput;
    QPushButton* findNext;
    QPushButton* findPrevious;
    QLineEdit* findStatus;
};

#endif // MAINWINDOW_H
