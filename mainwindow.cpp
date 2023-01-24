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

#include "mainwindow.h"

#include "lineedit.h"
#include "tabbar.h"
#include "bookmarks.h"
#include "trafficlog.h"
#include "menu.h"
#include "javascriptconsole.h"
#include "sourceview.h"
#include "browserapplication.h"
#include "desktopwindowmanager.h"
#include "downloadmanager.h"
#include "settingsdialog.h"
#include "settings.h"
#include "gripwidget.h"
#include "urlshortcutresolver.h"

#include <QtWebKit>
#include <QWebInspector>
#include <QtSvg>


/*#ifdef Q_WS_WIN
#include <windows.h>
#endif*/



typedef long __stdcall dwmExtendFrameIntoClientAreaFunction(void* hWnd,const int* margins);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(BrowserApplication::getStatic()->applicationName());

    /*QPixmap iconpixmap(16,16);
    iconpixmap.fill(QColor(0,0,0,0));
    setWindowIcon(QIcon(iconpixmap));*/

    //setWindowIcon(QIcon(":/icon.png"));

    // create layout tree
    mainLayout = new QVBoxLayout();
    tabBarLayout = new QHBoxLayout();
    navigationBarLayout = new QHBoxLayout();
    pageStackLayout = new QStackedLayout();
    findLayout = new QHBoxLayout();

    tabContainer = new QWidget();
    tabContainer->setLayout(tabBarLayout);
    mainLayout->addWidget(tabContainer);

    naviWidget = new QWidget();
    naviWidget->setLayout(navigationBarLayout);
    mainLayout->addWidget(naviWidget);

    pageStackContainer = new QWidget();
    QHBoxLayout* psl = new QHBoxLayout();
    psl->setContentsMargins(0,0,0,0);psl->setSpacing(0);
    psl->addLayout(pageStackLayout);
    pageStackContainer->setLayout(psl);
    mainLayout->addWidget(pageStackContainer);

    findWidget = new QWidget();
    findWidget->setLayout(findLayout);
    mainLayout->addWidget(findWidget);

    mainLayout->setSpacing(0);
    tabBarLayout->setSpacing(0);
    navigationBarLayout->setSpacing(0);
    pageStackLayout->setSpacing(0);
    findLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    tabBarLayout->setContentsMargins(0,0,0,0);
    navigationBarLayout->setContentsMargins(0,0,0,0);
    pageStackLayout->setContentsMargins(0,0,0,0);
    findLayout->setContentsMargins(0,0,0,0);


    // create tab bar
    tabBar = new TabBar();
    tabButton = new PushButton(tr(""));
    tabBarLayout->addWidget(tabBar);
    tabBarLayout->addWidget(tabButton);
    tabBarLayout->addWidget(new GripWidget());
    tabBar->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));
    tabBar->setExpanding(true);
    tabBar->setTabsClosable(true);
    tabBar->setMovable(true);
    tabBar->setDrawBase(false);
    tabButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabChanged(int)));
    connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabButton, SIGNAL(clicked()), this, SLOT(appendTab()));
    connect(tabButton, SIGNAL(mmb()), this, SLOT(appendTab()));
    connect(tabButton, SIGNAL(rmb()), this, SLOT(appendTab()));
    connect(tabBar, SIGNAL(tabDoubleClicked(int)), this, SLOT(dockTabOff(int)));
    connect(tabBar, SIGNAL(tabMbClicked(int)), this, SLOT(dublicateTab(int)));
    connect(tabBar, SIGNAL(tabRightClicked(int, QPoint)), this, SLOT(onTabRightClick(int, QPoint)));

    // create url bar
    urlBox = new UrlLineEdit();
    bwdButton = new PushButton(tr(""));
    fwdButton = new PushButton(tr(""));
    rldButton = new PushButton(tr(""));
    favButton = new PushButton(tr(""));
    goButton = new PushButton(tr(""));
    xButton = new PushButton(tr(""));

    // create find bar
    findClose = new QPushButton(tr(""));
    findInput = new LineEdit();
    findNext = new QPushButton(tr(""));
    findPrevious = new QPushButton(tr(""));
    findStatus = new QLineEdit(tr(""));
    findLayout->addWidget(findClose);
    findLayout->addWidget(findInput);
    findLayout->addWidget(findPrevious);
    findLayout->addWidget(findNext);
    findLayout->addWidget(findStatus);
    findStatus->setReadOnly(true);
    findNext->setShortcut(QKeySequence::keyBindings(QKeySequence::FindNext).first());
    findPrevious->setShortcut(QKeySequence::keyBindings(QKeySequence::FindPrevious).first());
    connect(findClose, SIGNAL(clicked()), this, SLOT(onFindClose()));
    connect(findInput, SIGNAL(textChanged(QString)), this, SLOT(onFindChanged(QString)));
    connect(findNext, SIGNAL(clicked()), this, SLOT(onFindNext()));
    connect(findPrevious, SIGNAL(clicked()), this, SLOT(onFindPrevious()));
    connect(findInput, SIGNAL(returnPressed()), this, SLOT(onFindNext()));
    connect(findInput, SIGNAL(escapePressed()), this, SLOT(onFindClose()));
    connect(favButton, SIGNAL(mmb()), favButton, SLOT(showMenu()));
    connect(favButton, SIGNAL(rmb()), favButton, SLOT(showMenu()));
    connect(xButton, SIGNAL(mmb()), xButton, SLOT(showMenu()));
    connect(xButton, SIGNAL(rmb()), xButton, SLOT(showMenu()));

    // bookmark menu
    bookmarkMenu = new BookmarkMenu(this);

    // x menu
    xMenu = new ExtendedMenu(this);

    { QAction* a = new QAction(tr("New Window"), this); connect(a, SIGNAL(triggered()), this, SLOT(newWindow())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::New)); xMenu->addAction(a); }
    { QAction* a = new QAction(tr("Find Text"), this); connect(a, SIGNAL(triggered()), this, SLOT(findText())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Find)); xMenu->addAction(a); }

    xMenu->addSeparator();

    {
        QMenu* m = new ExtendedMenu(xMenu);
        m->setTitle(tr("File"));
        xMenu->addMenu(m);
        { QAction* a = new QAction(tr("Open"), this); connect(a, SIGNAL(triggered()), urlBox, SLOT(setFocus())); connect(a, SIGNAL(triggered()), urlBox, SLOT(selectAll())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Open)); m->addAction(a); }
        { QAction* a = new QAction(tr("Open File"), this); connect(a, SIGNAL(triggered()), this, SLOT(browseFileOpen())); m->addAction(a); }
        m->addSeparator();
        { QAction* a = new QAction(tr("Save as ..."), this); connect(a, SIGNAL(triggered()), this, SLOT(downloadCurrentPage())); m->addAction(a); }
        {
            QMenu* m2 = new ExtendedMenu(m);
            m2->setTitle(tr("Export"));
            m->addMenu(m2);
            { QAction* a = new QAction(tr("Image"), this); connect(a, SIGNAL(triggered()), this, SLOT(exportImage())); m2->addAction(a); }
            { QAction* a = new QAction(tr("PDF / Postscript"), this); connect(a, SIGNAL(triggered()), this, SLOT(exportPDF())); m2->addAction(a); }
            { QAction* a = new QAction(tr("SVG"), this); connect(a, SIGNAL(triggered()), this, SLOT(exportSVG())); m2->addAction(a); }
        }
        m->addSeparator();
        { QAction* a = new QAction(tr("Print"), this); connect(a, SIGNAL(triggered()), this, SLOT(print())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Print)); m->addAction(a); }
        m->addSeparator();
        { QAction* a = new QAction(tr("Back"), this); connect(a, SIGNAL(triggered()), this, SLOT(back())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Back)); m->addAction(a); }
        { QAction* a = new QAction(tr("Forward"), this); connect(a, SIGNAL(triggered()), this, SLOT(forward())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Forward)); m->addAction(a); }
        m->addSeparator();
        { QAction* a = new QAction(tr("Reload"), this); connect(a, SIGNAL(triggered()), this, SLOT(reload())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Refresh)); m->addAction(a); }
    }
    {
        QMenu* m = new ExtendedMenu(xMenu);
        m->setTitle(tr("Edit"));
        xMenu->addMenu(m);
        { QAction* a = new QAction(tr("Undo"), this); connect(a, SIGNAL(triggered()), this, SLOT(undo())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Undo)); m->addAction(a); }
        { QAction* a = new QAction(tr("Redo"), this); connect(a, SIGNAL(triggered()), this, SLOT(redo())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Redo)); m->addAction(a); }
        m->addSeparator();
        { QAction* a = new QAction(tr("Cut"), this); connect(a, SIGNAL(triggered()), this, SLOT(cut())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Cut)); m->addAction(a); }
        { QAction* a = new QAction(tr("Copy"), this); connect(a, SIGNAL(triggered()), this, SLOT(copy())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Copy)); m->addAction(a); }
        { QAction* a = new QAction(tr("Paste"), this); connect(a, SIGNAL(triggered()), this, SLOT(paste())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Paste)); m->addAction(a); }
    }
    {
        QMenu* m = new ExtendedMenu(xMenu);
        m->setTitle(tr("Tab"));
        xMenu->addMenu(m);
        { QAction* a = new QAction(tr("New Tab"), this); connect(a, SIGNAL(triggered()), this, SLOT(addTab())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::AddTab)); m->addAction(a); }
        { QAction* a = new QAction(tr("Next Tab"), this); connect(a, SIGNAL(triggered()), this, SLOT(selectNextTab())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::NextChild)); m->addAction(a); }
        { QAction* a = new QAction(tr("Previous Tab"), this); connect(a, SIGNAL(triggered()), this, SLOT(selectPreviousTab())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::PreviousChild)); m->addAction(a); }
    }
    {
        QMenu* m = new ExtendedMenu(xMenu);
        m->setTitle(tr("Zoom"));
        xMenu->addMenu(m);
        { QAction* a = new QAction(tr("Zoom In"), this); connect(a, SIGNAL(triggered()), this, SLOT(zoomIn())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::ZoomIn)); m->addAction(a); }
        { QAction* a = new QAction(tr("Zoom Out"), this); connect(a, SIGNAL(triggered()), this, SLOT(zoomOut())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::ZoomOut)); m->addAction(a); }
        { QAction* a = new QAction(tr("Reset Zoom"), this); connect(a, SIGNAL(triggered()), this, SLOT(resetZoom())); a->setShortcut(QKeySequence(tr("Ctrl+0"))); m->addAction(a); }
    }
    {
        QMenu* m = new ExtendedMenu(xMenu);
        m->setTitle(tr("Extras"));
        xMenu->addMenu(m);
        { QAction* a = new QAction(tr("View Source"), this); connect(a, SIGNAL(triggered()), this, SLOT(viewSource())); m->addAction(a); }
        { QAction* a = new QAction(tr("JS Debugger"), this); connect(a, SIGNAL(triggered()), this, SLOT(showJavaScriptConsole())); m->addAction(a); }
        { QAction* a = new QAction(tr("Traffic Log"), this); connect(a, SIGNAL(triggered()), this, SLOT(showTrafficLog())); m->addAction(a); }
        { QAction* a = new QAction(tr("Render Tree"), this); connect(a, SIGNAL(triggered()), this, SLOT(renderTreeDump())); m->addAction(a); }
        { QAction* a = new QAction(tr("Node Tree"), this); connect(a, SIGNAL(triggered()), this, SLOT(dumpNodeTree())); m->addAction(a); }
        m->addSeparator();
        { QAction* a = new QAction(tr("Inspector"), this); connect(a, SIGNAL(triggered()), this, SLOT(inspect())); m->addAction(a); }
    }

    xMenu->addSeparator();

    { QAction* a = new QAction(tr("Download Manager"), this); connect(a, SIGNAL(triggered()), DownloadManager::getStatic(), SLOT(showX())); xMenu->addAction(a); }
    { QAction* a = new QAction(tr("Settings"), this); connect(a, SIGNAL(triggered()), this, SLOT(showSettingsDialog())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Preferences)); xMenu->addAction(a); }
    { QAction* a = new QAction(tr("About"), this); connect(a, SIGNAL(triggered()), this, SLOT(showAboutBox())); xMenu->addAction(a); }


    // setup url bar
    favButton->setMenu(bookmarkMenu);
    xButton->setMenu(xMenu);
    navigationBarLayout->addWidget(bwdButton);
    navigationBarLayout->addWidget(rldButton);
    navigationBarLayout->addWidget(fwdButton);
    navigationBarLayout->addWidget(favButton);
    navigationBarLayout->addWidget(urlBox);
    navigationBarLayout->addWidget(goButton);
    navigationBarLayout->addWidget(xButton);
    connect(urlBox, SIGNAL(returnPressed()), this, SLOT(go()));
    connect(goButton, SIGNAL(clicked()), this, SLOT(go()));
    connect(fwdButton, SIGNAL(clicked()), this, SLOT(forward()));
    connect(bwdButton, SIGNAL(clicked()), this, SLOT(back()));
    connect(rldButton, SIGNAL(clicked()), this, SLOT(reload()));
    connect(urlBox, SIGNAL(escapePressed()), this, SLOT(onUrlEscape()));
    connect(rldButton, SIGNAL(mmb()), this, SLOT(dublicateCurrentTab()));
    connect(fwdButton, SIGNAL(mmb()), this, SLOT(goForwardInNewTab()));
    connect(bwdButton, SIGNAL(mmb()), this, SLOT(goBackInNewTab()));
    connect(goButton, SIGNAL(mmb()), this, SLOT(goInNewTab()));
    connect(bwdButton, SIGNAL(rmb()), this, SLOT(showBackMenu()));
    connect(fwdButton, SIGNAL(rmb()), this, SLOT(showForwardMenu()));

    // create central widget
    centralWidget = new QWidget();
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // status label
    statusLabel = new QLabel(this);
    statusLabel->setVisible(false);

    // assign names
    menuBar()->setObjectName("menu");
    setObjectName("mainwindow");
    tabBar->setObjectName("tabbar");
    tabButton->setObjectName("tabbutton");
    urlBox->setObjectName("urlbox");
    bwdButton->setObjectName("back");
    fwdButton->setObjectName("forward");
    rldButton->setObjectName("reload");
    favButton->setObjectName("bookmark");
    goButton->setObjectName("go");
    naviWidget->setObjectName("navibar");
    centralWidget->setObjectName("central");
    tabContainer->setObjectName("tabcontainer");
    xButton->setObjectName("xbutton");
    pageStackContainer->setObjectName("pagestack");
    statusBar()->setObjectName("statusbar");
    statusLabel->setObjectName("statuslabel");
    findWidget->setObjectName("findbar");
    findClose->setObjectName("findclose");
    findInput->setObjectName("findinput");
    findNext->setObjectName("findnext");
    findPrevious->setObjectName("findprevious");
    findStatus->setObjectName("findstatus");

    urlBox->setStatusTip(tr("URL Box"));
    bwdButton->setStatusTip(tr("History Back"));
    fwdButton->setStatusTip(tr("History Forward"));
    rldButton->setStatusTip(tr("Reload Page"));
    favButton->setStatusTip(tr("Bookmarks"));
    goButton->setStatusTip(tr("Open this URL"));
    xButton->setStatusTip(tr("Application Menu"));

    statusBar()->hide();


    // transparency
    if(DesktopWindowManager::extendFrameIntoClientArea(this, 64, 2, 2 /*+ statusBar()->height()*/, 2))
    {
        psl->setContentsMargins(1,0,1,1);
        setAttribute(Qt::WA_TranslucentBackground);
        tabBar->setAttribute(Qt::WA_TranslucentBackground);
    }

    // start
    addTab();
    findWidget->hide();

    // delete on close
    //setAttribute(Qt::WA_DeleteOnClose, true);
}

MainWindow::~MainWindow()
{
}


void MainWindow::appinit()
{
    loadPage(Settings::getStatic()->value("general/home").toString());
}

void MainWindow::showBackMenu()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    ExtendedMenu* menu = new ExtendedMenu();
    {
        QAction* a = menu->addAction(wv->title());
        a->setToolTip(wv->url().toString());
        menu->addSeparator();
    }
    QList<QWebHistoryItem> l = wv->history()->backItems(10);
    for(int i = l.length() - 1; i >= 0; i--)
    {
        QAction* a = menu->addAction(l[i].title());
        a->setToolTip(l[i].url().toString());
    }

    connect(menu, SIGNAL(lmbToolTip(QString)), this, SLOT(loadPage(QString)));
    connect(menu, SIGNAL(mmbToolTip(QString)), this, SLOT(loadPageInNewTab(QString)));

    menu->exec(bwdButton->mapToGlobal(bwdButton->rect().bottomLeft()));
}

void MainWindow::showForwardMenu()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    ExtendedMenu* menu = new ExtendedMenu();
    {
        QAction* a = menu->addAction(wv->title());
        a->setToolTip(wv->url().toString());
        menu->addSeparator();
    }
    QList<QWebHistoryItem> l = wv->history()->forwardItems(10);
    for(int i = l.length() - 1; i >= 0; i--)
    {
        QAction* a = menu->addAction(l[i].title());
        a->setToolTip(l[i].url().toString());
    }

    connect(menu, SIGNAL(lmbToolTip(QString)), this, SLOT(loadPage(QString)));
    connect(menu, SIGNAL(mmbToolTip(QString)), this, SLOT(loadPageInNewTab(QString)));

    menu->exec(fwdButton->mapToGlobal(fwdButton->rect().bottomLeft()));
}

void MainWindow::goBackInNewTab()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    loadPageInNewTab(wv->history()->backItem().url().toString());
}

void MainWindow::goForwardInNewTab()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    loadPageInNewTab(wv->history()->forwardItem().url().toString());
}

void MainWindow::goInNewTab()
{
    loadPageInNewTab(urlBox->text());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for(int i = 0; i < pageStackLayout->count(); i++)
    {
        WebView* wv = qobject_cast<WebView*>(pageStackLayout->widget(i));
        if(wv)
        {
            wv->deleteLater();
        }

    }
    for(int i = 0; i < pageStackLayout->count(); i++)
        pageStackLayout->removeWidget(pageStackLayout->widget(i));
    while(tabBar->count())
        tabBar->removeTab(0);
    event->accept();
    //this->deleteLater();
}

WebView* MainWindow::getCurrentWebView()
{
    WebView* c = qobject_cast<WebView*>(pageStackLayout->currentWidget());
    return c;
}


void MainWindow::loadPageInNewTab(QString url)
{
    addTab();
    tabBar->setTabText(tabBar->currentIndex(), tr("New Tab"));
    loadPage(url);
}

void MainWindow::loadPageBackground(QString url)
{
    int i2 = tabBar->currentIndex();
    //int i = tabBar->addTab(tr("New Tab"));
    int i = tabBar->insertTab(i2+1, tr("New Tab"));
    WebView* wv = new WebView(this);
    WebPage* wp = new WebPage(wv);
    wv->setPage(wp);
    pageStackLayout->addWidget(wv);
    tabBar->setTabData(i, (qlonglong)wv);
    connect(wv, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
    //
    loadPage(url, wv);

    //tabBar->setCurrentIndex(i2);
    //urlBox->setFocus();
    updateNavigationBar();
}


void MainWindow::dublicateTab(int i)
{
    WebView* wv = (WebView*)tabBar->tabData(i).toLongLong();
    if(wv == 0)
        return;

    loadPageInNewTab(wv->url().toString());
}
void MainWindow::dublicateCurrentTab()
{
    dublicateTab(tabBar->currentIndex());
}

void MainWindow::dockTabOff(int i)
{
    WebView* wv = (WebView*)tabBar->tabData(i).toLongLong();
    if(wv == 0)
        return;

    MainWindow* nmw = new MainWindow();
    nmw->loadPage(wv->url().toString());
    nmw->show();

    /*QString txt = tabBar->tabText(i);
    tabBar->removeTab(i);
    //pageStackLayout->removeWidget(wv);

    MainWindow* mw = new MainWindow();
    mw->show();
    mw->pageStackLayout->addWidget(wv);
    int j = mw->tabBar->addTab("");
    mw->tabBar->setTabData(j, (qlonglong)wv);
    mw->closeTab(0);
    mw->updateNavigationBar();
    mw->tabBar->setTabText(0, txt);

    if(tabBar->count() == 0)
        addTab();

    updateNavigationBar();*/
}
void MainWindow::dockCurrentTabOff()
{
    dockTabOff(tabBar->currentIndex());
}

void MainWindow::onTabRightClick(int i, QPoint pos)
{
    QRect r = tabBar->tabRect(i);
    if(!r.isNull())
    {
        tabBar->setCurrentIndex(i);
        ExtendedMenu* menu = new ExtendedMenu();
        menu->addAction(tr("New Tab"), this, SLOT(addTab()));
        menu->addSeparator();
        menu->addAction(tr("Clone Tab"), this, SLOT(dublicateCurrentTab()));
        menu->addAction(tr("Reload Tab"), this, SLOT(reload()));
        menu->addAction(tr("Close Tab"), this, SLOT(closeCurrentTab()));
        menu->addSeparator();
        menu->addAction(tr("Open in New Window"), this, SLOT(dockCurrentTabOff()));
        menu->exec(/*tabBar->mapToGlobal(r.bottomLeft())*/ pos);
    }
}

void MainWindow::addTab()
{
    //int i = tabBar->addTab(tr("New Tab"));
    int i2 = tabBar->currentIndex();
    //int i = tabBar->addTab(tr("New Tab"));
    int i = tabBar->insertTab(i2+1, tr("New Tab"));

    WebView* wv = new WebView(this);
    WebPage* wp = new WebPage(wv);
    wv->setPage(wp);
    pageStackLayout->addWidget(wv);
    tabBar->setTabData(i, (qlonglong)wv);
    tabBar->setCurrentIndex(i);
    connect(wv, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
    urlBox->setFocus();
}

void MainWindow::appendTab()
{
    int i = tabBar->addTab(tr("New Tab"));
    WebView* wv = new WebView(this);
    WebPage* wp = new WebPage(wv);
    wv->setPage(wp);
    pageStackLayout->addWidget(wv);
    tabBar->setTabData(i, (qlonglong)wv);
    tabBar->setCurrentIndex(i);
    connect(wv, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
    urlBox->setFocus();
}

void MainWindow::onCurrentTabChanged(int i)
{
    QWidget* w = (QWidget*)tabBar->tabData(i).toLongLong();
    if(w == 0)
        return;

    //updateNavigationBar();

    //QApplication::processEvents();



    pageStackLayout->setCurrentWidget(w);

    WebView* wv = qobject_cast<WebView*>(w);
    if(wv)
    {
        if(wv->url().isEmpty() && urlBox)
        {
            urlBox->setFocus();
            urlBox->setText("");
        }
        else
            w->setFocus();
    } else {
        w->setFocus();
    }

    updateNavigationBar();
}
void MainWindow::onUrlChanged(QUrl)
{
    updateNavigationBar();
}

void MainWindow::go()
{
    this->setFocus();
    loadPage(urlBox->text());
}
void MainWindow::back()
{
    WebView* c = getCurrentWebView();
    if(c)
        c->back();
}

void MainWindow::forward()
{
    WebView* c = getCurrentWebView();
    if(c)
        c->forward();
}

void MainWindow::reload()
{
    WebView* c = getCurrentWebView();
    if(c)
    {
        if(c->isLoading())
            c->stop();
        else
            c->reload();
    }
}

void MainWindow::abort()
{
    WebView* c = getCurrentWebView();
    if(c)
    {
        c->stop();
    }
}

void MainWindow::loadPage(QString _url, WebView* wv)
{
    QString url = _url;



    WebView* c = wv ? wv : getCurrentWebView();
    if(c == 0)
        return;

    if(url.simplified().toLower() == "home")
        url = Settings::getStatic()->value("general/home").toString();



    /*QMap<QString, QString> quickMap;

    quickMap["home"] = Settings::getStatic()->value("general/home").toString();

    quickMap["google"] = "http://google.com/search?q=%1";
    quickMap["g"] = "http://google.com/search?q=%1";
    quickMap["search"] = "http://google.com/search?q=%1";
    quickMap["find"] = "http://google.com/search?q=%1";

    quickMap["img"] = "http://google.com/images?q=%1";
    quickMap["images"] = "http://google.com/images?q=%1";

    quickMap["googlemaps"] = "http://google.com/maps?q=%1";
    quickMap["map"] = "http://google.com/maps?q=%1";
    quickMap["maps"] = "http://google.com/maps?q=%1";
    quickMap["sat"] = "http://google.com/maps?q=%1&t=h";


    quickMap["go"] = "http://www.google.com/search?btnI&q=%1";
    quickMap["goto"] = "http://www.google.com/search?btnI&q=%1";

    //quickMap["facebook"] = "http://www.google.com/search?btnI&q=site:facebook.com+%1";
    //quickMap["fb"] = "http://www.google.com/search?btnI&q=site:facebook.com+%1";

    quickMap["facebook"] = "http://www.facebook.com/srch.php?nm=%1";
    quickMap["fb"] = "http://www.facebook.com/srch.php?nm=%1";

    quickMap["myspace"] = "http://www.google.com/search?btnI&q=site:myspace.com+%1";

    quickMap["youtube"] = "http://youtube.com/results?search_query=%1";
    quickMap["yt"] = "http://youtube.com/results?search_query=%1";

    quickMap["wiki"] = "http://wikipedia.org/wiki/index.php?search=%1";
    quickMap["wikipedia"] = "http://wikipedia.org/wiki/index.php?search=%1";*/

    /*QStringList l = url.simplified().split(' ');
    if(l.length() > 0)
    {
        QString key = l.at(0).toLower();
        if(quickMap.contains(key))
        {
            l.removeFirst();
            url = quickMap.value(key).arg(QUrl::fromPunycode(QUrl::toPercentEncoding(QUrl::toPunycode(l.join(" ")))).replace("%20", "+"));
        }
    }*/

    //if(!QUrl(url).isValid())
    //{
        QString rslv = UrlShortcutResolver::instance()->resolve(url);
        if(rslv.isNull())
        {
            /*if(!url.contains("http") && !url.contains(' ') && !url.startsWith("javascript:") && !url.contains("//"))
                url = "http://" + url;*/

            /*if(!url.contains(".") && !url.contains("/") && !url.contains(":"))
            {
                rslv = UrlShortcutResolver::instance()->resolve("search " + url);
                if(!rslv.isNull())
                    url = rslv;
            }*/
        } else {
            url = rslv;
        }
    //}



    if(url.startsWith("javascript:"))
    {
        if(c->page())
        {
            if(c->page()->mainFrame())
            {
                url = url.right(url.length() - QString("javascript:").length());
                while(url.startsWith('/'))
                {
                    url = url.right(url.length() - 1);
                }
                c->page()->mainFrame()->evaluateJavaScript(url);
                c->setFocus();
                updateNavigationBar();
                return;
            }
        }
    }



    QUrl xurl = QUrl::fromUserInput(url);

    if(xurl.isValid())
    {
        if(c->url().isEmpty() && url.contains("#"))
        {
            //c->setUrl(QUrl(url));
            //c->load(QUrl(url));
            c->setUrl(QUrl(xurl));
            c->load(QUrl(xurl));
        } else {
            //c->setUrl(QUrl(url));
            c->setUrl(QUrl(xurl));
        }

        c->setFocus();

        QApplication::processEvents();

        urlBox->setText(url);

    } else {
        //wv->showErrorPage("invalidurl");
        updateNavigationBar();
    }

}
void MainWindow::updateNavigationBar()
{
    WebView* c = getCurrentWebView();
    if(c == 0)
    {
        if(!urlBox->hasFocus())
            urlBox->setText("");
        bwdButton->setEnabled(false);
        fwdButton->setEnabled(false);
        rldButton->setEnabled(false);
        rldButton->setObjectName("reload");

        rldButton->setStyleSheet("*{}");
        rldButton->setStyleSheet("");

        //setWindowTitle(BrowserApplication::getStatic()->applicationName());

        return;
    } else {
        if(!urlBox->hasFocus())
        {
            QString su = c->url().toString();
            su.truncate(1000);
            urlBox->setText(su);
        }
        bwdButton->setEnabled(c->history()->canGoBack());
        fwdButton->setEnabled(c->history()->canGoForward());
        rldButton->setEnabled(!c->url().isEmpty());

        c->isLoading() ? qDebug("loading") : qDebug("ready");

        if(c->isLoading())
            rldButton->setObjectName("abort");
        else
            rldButton->setObjectName("reload");

        rldButton->setStyleSheet("*{}");
        rldButton->setStyleSheet("");

        //setWindowTitle(BrowserApplication::getStatic()->applicationName() + " - " + c->title());

        //setWindowIcon(c->icon());

        //tabBar->setTabIcon(tabBar->currentIndex(), c->icon());


    }



}

void MainWindow::closeTab(int i)
{
    QWidget* wdg = (QWidget*)tabBar->tabData(i).toLongLong();
    if(wdg == 0)
        return;

    tabBar->removeTab(i);
    pageStackLayout->removeWidget(wdg);

    //delete wdg;
    wdg->deleteLater();

    updateNavigationBar();

    if(tabBar->count() == 0)
        addTab();
}
void MainWindow::closeCurrentTab()
{
    closeTab(tabBar->currentIndex());
}

QTabBar* MainWindow::documentTabBar()
{
    return tabBar;
}

void MainWindow::setTabTitle(QWidget* tab, QString title)
{
    for(int i = 0; i < tabBar->count(); i++)
    {
        if(tabBar->tabData(i).toLongLong() == (qlonglong)tab)
        {
            tabBar->setTabText(i, title);
        }
    }
    updateNavigationBar();
}

void MainWindow::bookmark()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    if(wv->url().isEmpty())
        return;
    BookmarkList::globalBookmarkList()->add(wv->url().toString().left(4000), wv->title().left(40));
}



void MainWindow::newWindow()
{
    MainWindow* w = new MainWindow();
    w->show();
}

void MainWindow::triggerAction(QWebPage::WebAction action, bool checked)
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    QWebPage* wp = wv->page();
    if(wp == 0)
        return;
    wp->triggerAction(action, checked);
}


void MainWindow::copy()
{
    triggerAction(QWebPage::Copy);
}

void MainWindow::paste()
{
    triggerAction(QWebPage::Paste);
}
void MainWindow::cut()
{
    triggerAction(QWebPage::Cut);
}

QWebInspector* theGlobalXXXInspector = 0;
void MainWindow::inspect()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    QWebPage* wp = wv->page();
    if(wp == 0)
        return;

    if(theGlobalXXXInspector == 0)
        theGlobalXXXInspector = new QWebInspector();
    if( theGlobalXXXInspector->page() != wp )
        theGlobalXXXInspector->setPage(wp);
    theGlobalXXXInspector->setVisible(true);
}

void MainWindow::download()
{
    //triggerAction(QWebPage::saveFrameStateRequested());
}

void MainWindow::undo()
{
    triggerAction(QWebPage::Undo);
}

void MainWindow::redo()
{
    triggerAction(QWebPage::Redo);
}

void MainWindow::selectNextTab()
{
    tabBar->setCurrentIndex((tabBar->currentIndex() + 1) % tabBar->count());
}
void MainWindow::selectPreviousTab()
{
    tabBar->setCurrentIndex(tabBar->currentIndex() == 0 ? tabBar->count() - 1 : tabBar->currentIndex() - 1);
}

void MainWindow::zoomIn()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    wv->setZoomFactor(wv->zoomFactor() * sqrt(sqrt(2)));
}
void MainWindow::zoomOut()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    wv->setZoomFactor(wv->zoomFactor() / sqrt(sqrt(2)));
}
void MainWindow::resetZoom()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    wv->setZoomFactor(1);
}


TrafficLog* theGlobalTrafficLogXXX = 0;
void MainWindow::showTrafficLog()
{
    if( theGlobalTrafficLogXXX == 0 )
        theGlobalTrafficLogXXX = new TrafficLog();
    theGlobalTrafficLogXXX->show();
}

void MainWindow::showJavaScriptConsole()
{
    JavaScriptConsole::getStatic()->show();
}


void MainWindow::viewSource()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    if(wv->page() == 0)
        return;
    if(wv->page()->mainFrame() == 0)
        return;
    SourceView* sv = new SourceView();
    sv->setCode(wv->page()->mainFrame()->toHtml());
    sv->setWindowTitle("Source: " + wv->page()->mainFrame()->url().toString());
    sv->show();
}




void MainWindow::setStatus(QString stat)
{
    if(stat.isEmpty())
    {
        statusLabel->setVisible(false);
    } else {
        WebView* wv = getCurrentWebView();
        statusLabel->setParent(this);
        statusLabel->setTextFormat(Qt::PlainText);
        statusLabel->setText(stat);

        int off = 0;
        if(wv->page())
        {
            if(wv->page()->mainFrame())
            {
                off = wv->page()->mainFrame()->scrollBarGeometry(Qt::Horizontal).height();
            }
        }
        statusLabel->setGeometry(1, height() - statusLabel->sizeHint().height() - 1 - off, statusLabel->sizeHint().width(), statusLabel->sizeHint().height());
        statusLabel->setVisible(true);
    }
}



void MainWindow::print()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    static QPrinter printer;
    QPrintPreviewDialog diag(&printer);
    connect(&diag, SIGNAL(paintRequested(QPrinter*)), wv, SLOT(print(QPrinter*)));
    diag.exec();
}
void MainWindow::exportPDF()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    QString fname = QFileDialog::getSaveFileName(this, tr("Export PDF / Postscript ..."), QDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).absoluteFilePath(/*QFileInfo(wv->url().path()).baseName()*/wv->title() + ".pdf"),  "PDF or PostScript (*.pdf *.ps);;PDF Files (*.pdf);;PostScript Files (*.ps);;All Files (*.*)", 0);
    if(fname.isEmpty())
        return;

    QPrinter printer;
    printer.setOutputFileName(fname);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageOrder(QPrinter::FirstPageFirst);
    wv->print(&printer);

    /*MainWindow* mw = new MainWindow();
    mw->loadPage("file:///" + fname);
    mw->showNormal();*/

    DownloadManager::getStatic()->showX();
    DownloadManager::getStatic()->addFile(fname, wv->url(), tr("ok (pdf export)"));
}
void MainWindow::exportSVG()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    if(wv->page() == 0)
        return;
    if(wv->page()->mainFrame() == 0)
        return;

    QString fname = QFileDialog::getSaveFileName(this, tr("Export SVG ..."), QDir(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation)).absoluteFilePath(/*QFileInfo(wv->url().path()).baseName()*/wv->title() + ".svg"),  "Scalable Vector Graphics (*.svg);;All Files (*.*)", 0);
    if(fname.isEmpty())
        return;

    QSize defVPSize = wv->page()->viewportSize();

    QSize contentsSize = wv->page()->mainFrame()->contentsSize();
    wv->page()->setViewportSize(contentsSize);

    QSvgGenerator generator;
    generator.setFileName(fname);
    generator.setTitle(wv->title());

    QPainter painter;
    generator.setSize(contentsSize);
    generator.setViewBox(QRect(0, 0, contentsSize.width(), contentsSize.height()));
    painter.begin(&generator);
    wv->page()->mainFrame()->render(&painter, QWebFrame::ContentsLayer);
    painter.end();

    wv->page()->setViewportSize(defVPSize);

    /*MainWindow* mw = new MainWindow();
    mw->loadPage("file:///" + fname);
    mw->showNormal();*/

    DownloadManager::getStatic()->showX();
    DownloadManager::getStatic()->addFile(fname, wv->url(), tr("ok (svg export)"));
}
void MainWindow::exportImage()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    if(wv->page() == 0)
        return;
    if(wv->page()->mainFrame() == 0)
        return;

    QList<QByteArray> supportedFormats = QImageWriter::supportedImageFormats();

    QString extfmt = "Image Files (";
    foreach(QByteArray a, supportedFormats)
    {
        extfmt += "*." + QString::fromAscii(a).toLower() + " ";
    }
    extfmt = extfmt.trimmed();
    extfmt += ");;All Files (*.*)";

    QString fname = QFileDialog::getSaveFileName(this, tr("Export Image ..."), QDir(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation)).absoluteFilePath(/*QFileInfo(wv->url().path()).baseName()*/wv->title() + ".jpg"), extfmt, 0);
    if(fname.isEmpty())
        return;

    QSize defVPSize = wv->page()->viewportSize();

    QSize contentsSize = wv->page()->mainFrame()->contentsSize();
    contentsSize.setHeight(contentsSize.height() + 25);
    contentsSize.setWidth(contentsSize.width() + 25);
    wv->page()->setViewportSize(contentsSize);

    QImage image(contentsSize, QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    wv->page()->mainFrame()->render(&painter);
    painter.end();
    image.save(fname, 0, 93);

    wv->page()->setViewportSize(defVPSize);

    /*MainWindow* mw = new MainWindow();
    mw->loadPage("file:///" + fname);
    mw->showNormal();*/

    DownloadManager::getStatic()->showX();
    DownloadManager::getStatic()->addFile(fname, wv->url(), tr("ok (image export)"));
}
void MainWindow::browseFileOpen()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open file ...");
    if(!fname.isEmpty())
    {
        loadPage("file:///" + fname);
    }
}
void MainWindow::renderTreeDump()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    if(wv->page() == 0)
        return;
    if(wv->page()->mainFrame() == 0)
        return;

    SourceView* sv = new SourceView();
    sv->setCode("Render Tree Dump\n" + wv->url().toString() + "\n\n" + wv->page()->mainFrame()->renderTreeDump());
    sv->setWindowTitle("Render Tree Dump");
    sv->showNormal();
}


void MainWindow::onFindChanged(QString s)
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    wv->findText("", QWebPage::HighlightAllOccurrences);

    if(wv->findText(s, QWebPage::HighlightAllOccurrences))
        findStatus->setText(tr("Found."));
    else
        findStatus->setText(tr("Not found."));

    if(s.isEmpty())
        findStatus->setText("");

    /*wv->findText("", QWebPage::HighlightAllOccurrences);

    int count = 0;

    if(wv->findText(s, QWebPage::HighlightAllOccurrences))
    {
        wv->findText("");
        while(wv->findText(s) && (count < 1000))
        {
            count ++;
        }
        wv->findText("");
    }

    if(count > 0)
        findStatus->setText(tr("%1 occurences found.").arg(count));
    else
        findStatus->setText(tr("No occurences found."));*/
}

void MainWindow::onFindNext()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    if(wv->findText(findInput->text(), QWebPage::HighlightAllOccurrences))
        findStatus->setText(tr("Found."));
    else
        findStatus->setText(tr("Not found."));

    if(findInput->text().isEmpty())
        findStatus->setText("");

    wv->findText(findInput->text(), QWebPage::FindWrapsAroundDocument);
}

void MainWindow::onFindPrevious()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    if(wv->findText(findInput->text(), QWebPage::HighlightAllOccurrences))
        findStatus->setText(tr("Found."));
    else
        findStatus->setText(tr("Not found."));

    if(findInput->text().isEmpty())
        findStatus->setText("");

    wv->findText(findInput->text(), QWebPage::FindWrapsAroundDocument | QWebPage::FindBackward);
}

void MainWindow::onFindClose()
{
    findWidget->hide();
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    wv->findText("", QWebPage::HighlightAllOccurrences);
}

void MainWindow::findText()
{
    if(findWidget->isVisible())
    {
        onFindClose();
    } else {
        findWidget->show();
        //wv->findText(QString::null);
        //wv->findText(findInput->text(), QWebPage::HighlightAllOccurrences);
        emit onFindChanged(findInput->text());
        findInput->setFocus();
        findInput->selectAll();
    }
}



SettingsDialog* xxTheSettingsDialog = 0;
void MainWindow::showSettingsDialog()
{
    if(xxTheSettingsDialog == 0)
        xxTheSettingsDialog = new SettingsDialog();
    xxTheSettingsDialog->show();
    xxTheSettingsDialog->raise();
}


void MainWindow::showAboutBox()
{


    QString text;
    text += "<h2>" + Qt::escape(BrowserApplication::applicationName() + " " + BrowserApplication::applicationVersion()) + "</h2>";
    // text += "<p><b>Version: " + Qt::escape(BrowserApplication::applicationVersion()) + "</b></p>";

    text += "<p>Qt Version: " + Qt::escape(qVersion()) + "</p>";

    text += "<p>Webkit Version: " + Qt::escape(qWebKitVersion()) + "</p>";


    /*{
        QStringList supportedFormats2;
        QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
        foreach(QByteArray a, supportedFormats)
        {
            supportedFormats2 << QString::fromAscii(a).toLower();
        }
        text += "<p>Image Decoders: " + supportedFormats2.join(", ") + "</p>";
    }

    {
        QStringList supportedFormats2;
        QList<QByteArray> supportedFormats = QImageWriter::supportedImageFormats();
        foreach(QByteArray a, supportedFormats)
        {
            supportedFormats2 << QString::fromAscii(a).toLower();
        }
        text += "<p>Image Encoders: " + supportedFormats2.join(", ") + "</p>";
    }*/


    text += "<p>&copy; 2010 Philipp Ruppel</p>";

    text += "<p>";
    text += "<a href=\"http://ruppelmedia.com\">ruppelmedia</a> ";
    text += "<a href=\"http://weltweitimnetz.de\">weltweitimnetz</a> ";
    text += "<a href=\"http://qt.nokia.com\">qt</a> ";
    text += "<a href=\"http://webkit.org/\">webkit</a> ";
    text += "</p>";



    QMessageBox msgBox;
    msgBox.setWindowTitle("About " + BrowserApplication::applicationName());
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setIconPixmap(QPixmap(":/icon.png").scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    msgBox.exec();
}



QString MainWindow::_dumpElement(const QWebElement& xe, int indent)
{
    QString ret = "";
    for(QWebElement e = xe; !e.isNull(); e = e.nextSibling())
    {
        ret += QString(indent, ' ');
        ret += e.tagName();

        QStringList attrnames = e.attributeNames();
        foreach(QString s, attrnames)
        {
            ret += " " + s + "=\"" + e.attribute(s).replace("\"", "\\\"") + "\"";
        }
        ret += "\n";

        if(!e.firstChild().isNull())
            ret += _dumpElement(e.firstChild(), indent + 1);
    }
    return ret;
}

void MainWindow::dumpNodeTree()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    if(wv->page() == 0)
        return;
    if(wv->page()->mainFrame() == 0)
        return;
    if(wv->page()->mainFrame()->page() == 0)
        return;

    QString s = "";

    /*QMultiMap<QString, QString> m = wv->page()->mainFrame()->metaData();
    for(int i = 0; i < m.count(); i++)
    {
        m.value()
    }*/

    /*for(QWebElement e = wv->page()->mainFrame()->documentElement(); !e.isNull(); )
    {
        if(!e.firstChild().isNull())
    }*/

    s = _dumpElement(wv->page()->mainFrame()->documentElement());

    SourceView* sv = new SourceView();
    sv->setCode(s);
    wv->setWindowTitle("Node Tree: " + wv->url().toString());
    sv->show();
}

void MainWindow::onUrlEscape()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;
    urlBox->setText(wv->url().toString());
    setFocus();
}


void MainWindow::downloadCurrentPage()
{
    WebView* wv = getCurrentWebView();
    if(wv == 0)
        return;

    DownloadManager::getStatic()->download(wv->url());
}
