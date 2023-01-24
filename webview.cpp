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

#include "webview.h"
#include <QtGui>
#include "mainwindow.h"
#include <QtWebKit>
#include "menu.h"
#include "sourceview.h"
#include "webpage.h"
#include "networkaccessmanager.h"
#include "downloadmanager.h"

WebView::WebView(QWidget *parent) :
    QWebView(parent)
{
    _open_link_in_new_tab = false;
    connect(this, SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    connect(this, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
    popupTicket = 0;
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkMb(QUrl)));
    loading = false;

    setContextMenuPolicy(Qt::DefaultContextMenu);

    //setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
}


bool WebView::isLoading()
{
    return loading;
}

void WebView::onLinkMb(QUrl u)
{
    MainWindow* mw = qobject_cast<MainWindow*>(getMainWindow());
    if(mw == 0)
        return;
    /*int i = mw->documentTabBar()->currentIndex();
    mw->addTab();
    mw->setFocus();
    mw->loadPage(u.toString());
    mw->documentTabBar()->setCurrentIndex(i);*/
    mw->loadPageBackground(u.toString());
}
void WebView::mousePressEvent(QMouseEvent* ev)
{
   _open_link_in_new_tab = ev->button() == Qt::MidButton;
   popupTicket = QDateTime::currentMSecsSinceEpoch() + 3000;
   QWebView::mousePressEvent(ev);

   if(page())
   {
       if(ev->button() == Qt::LeftButton)
           page()->setLinkDelegationPolicy(QWebPage::DontDelegateLinks);
       else if(ev->button() == Qt::MidButton)
           page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    }

   updateStatusTip(ev);
}

void WebView::mouseReleaseEvent(QMouseEvent* ev)
{
   _open_link_in_new_tab = ev->button() == Qt::MidButton;
   popupTicket = QDateTime::currentMSecsSinceEpoch() + 3000;
   //if(ev->button() != Qt::MidButton)
   QWebView::mouseReleaseEvent(ev);

   updateStatusTip(ev);
}


bool WebView::shouldOpenLinkInNewTab()
{
   return _open_link_in_new_tab;
}

QWidget* WebView::getMainWindow()
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

void WebView::onTitleChanged(QString title)
{
    if(title.isEmpty())
        title = url().toString();
    title.truncate(40);
    MainWindow* mainwindow = qobject_cast<MainWindow*>(getMainWindow());
    if(mainwindow)
    {
        mainwindow->setTabTitle(this, title);
    }
}

void WebView::showErrorPage(QString key)
{
    QFile f(key + ".htm");

    f.open(QFile::ReadOnly);

    QString s = QString::fromUtf8(f.readAll());
    if(page())
    {
        if(page()->mainFrame())
        {
            s.replace(QString("{url}"), page()->mainFrame()->requestedUrl().toString());
            s.replace(QString("{search}"), page()->mainFrame()->requestedUrl().toString(QUrl::RemoveScheme | QUrl::StripTrailingSlash));
        }
    }

    this->setHtml(s, url());
}

void WebView::onLoadFinished(bool ok)
{
    /*if(!ok && page()->totalBytes() == 0)
    {
        //this->setHtml("<html><head><title>Not Found.</title><head><body><h1>Not Found.</h1></body></html>", url());


        showErrorPage("notfound");
    }*/
    loading = false;
    MainWindow* mainwindow = qobject_cast<MainWindow*>(getMainWindow());
    if(mainwindow)
        mainwindow->updateNavigationBar();
}
void WebView::onLoadStarted()
{
    loading = true;
    MainWindow* mainwindow = qobject_cast<MainWindow*>(getMainWindow());
    if(mainwindow)
        mainwindow->updateNavigationBar();
}

QWebView* WebView::createWindow(QWebPage::WebWindowType type)
{
    if(QDateTime::currentMSecsSinceEpoch() > popupTicket)
        return 0;
    popupTicket = 0;
    MainWindow* mainwindow = qobject_cast<MainWindow*>(getMainWindow());
    if(mainwindow)
    {
        mainwindow->addTab();
        mainwindow->setFocus();
        return mainwindow->getCurrentWebView();
    }
    return 0;
}

bool WebView::takePopupTicket()
{
    if(QDateTime::currentMSecsSinceEpoch() > popupTicket)
        return false;
    popupTicket = 0;
    return true;
}

void WebView::onContextMenuActionTriggered(QAction* a)
{
    MainWindow* mainwindow = qobject_cast<MainWindow*>(getMainWindow());
    if(mainwindow == 0)
        return;

    switch(a->data().toInt())
    {
    case 1:
        /*if(menu->mouseButtons() == Qt::MiddleButton)
        {
            int i = mainwindow->documentTabBar()->currentIndex();
            mainwindow->addTab();
            mainwindow->setFocus();
            mainwindow->loadPage(lastHitTestResult.linkUrl().toString());
            mainwindow->documentTabBar()->setCurrentIndex(i);
        } else*/ {
            triggerPageAction(QWebPage::OpenLink);
        }
        break;
    case 2:
        {
            int i = mainwindow->documentTabBar()->currentIndex();
            mainwindow->addTab();
            mainwindow->setFocus();
            mainwindow->loadPage(lastHitTestResult.linkUrl().toString());
            mainwindow->documentTabBar()->setCurrentIndex(i);
        }
        break;
    case 3:
        mainwindow = new MainWindow();
        mainwindow->show();
        mainwindow->loadPage(lastHitTestResult.linkUrl().toString());
        break;

    case 4:
        /*if(menu->mouseButtons() == Qt::MiddleButton)
        {
            int i = mainwindow->documentTabBar()->currentIndex();
            mainwindow->addTab();
            mainwindow->setFocus();
            mainwindow->loadPage(lastHitImageUrl.toString());
            mainwindow->documentTabBar()->setCurrentIndex(i);
        } else*/ {
            mainwindow->loadPage(lastHitImageUrl.toString());
        }
        break;
    case 5:
        {
            int i = mainwindow->documentTabBar()->currentIndex();
            mainwindow->addTab();
            mainwindow->setFocus();
            mainwindow->loadPage(lastHitImageUrl.toString());
            mainwindow->documentTabBar()->setCurrentIndex(i);
        }
        break;
    case 6:
        mainwindow = new MainWindow();
        mainwindow->show();
        mainwindow->loadPage(lastHitImageUrl.toString());
        break;
    case 7:
        QApplication::clipboard()->setText(lastHitTestResult.imageUrl().toString());
        QApplication::clipboard()->setImage(lastHitTestResult.pixmap().toImage());
        break;
    case 8:
        QApplication::clipboard()->setText(lastHitTestResult.linkUrl().toString());
        break;

    case 10:
        mainwindow->addTab();
        mainwindow->setFocus();
        mainwindow->loadPage("search " + selectedText());
        break;

    case 11:
        mainwindow->addTab();
        mainwindow->setFocus();
        mainwindow->loadPage("goto " + selectedText());
        break;

    case 12:
        mainwindow->addTab();
        mainwindow->setFocus();
        mainwindow->loadPage("wiki " + selectedText());
        break;

    case 101:
        //if(!lastHitTestResult.element().isNull())
        {
            QWebElement e = lastHitTestResult.element();
            if(e.isNull() || lastHitTestResult.isContentSelected())
                e = lastHitTestResult.enclosingBlockElement();

            QString info = "<big><b>" + Qt::escape("<" + e.tagName() + ">") + "</b></big>";

            info += "<br><br><b>Attributes</b>";

            info += "<table>";
            QStringList attrnm = e.attributeNames();
            for(int i = 0; i < attrnm.length(); i++)
                info += "<tr><td align='right'>" + Qt::escape(attrnm[i]) + ":</td><td>" + Qt::escape(e.attribute(attrnm[i])) + "</td></tr>";
            info += "</table>";

            if(!e.toPlainText().isEmpty())
            {
                info += "<br><br><b>Text</b><br>";

                QString t = e.toPlainText().replace('\n', ' ');
                if(t.length() > 200)
                    t = t.left(170) + "...";

                info += "<i>" + Qt::escape(t) + "</i>";
            }

            info += "<br><br><b>Code</b><br>";
            QString c = e.toOuterXml().replace('\n', ' ');
            if(c.length() > 200)
                c = c.left(170) + "...";
            info += "<i>" + Qt::escape(c) + "</i>";

            info += "<br><br>(click 'Details' for full html code)";

            QMessageBox msgBox;

            if(!lastHitTestResult.pixmap().isNull())
            {
                QPixmap pm = lastHitTestResult.pixmap();
                if(pm.width() > 0)
                {
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setIconPixmap(pm.scaled(100, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            } /*else {
                if(e.webFrame())
                {
                    QPixmap image(e.webFrame()->contentsSize());
                    image.fill();
                    {
                        QPainter painter(&image);
                        e.webFrame()->render(&painter);
                        painter.end();
                    }
                    image = image.copy(e.geometry());
                    int w = 150;
                    int h = 150;
                    if(image.height() < h)
                        h = image.height();
                    image = image.scaled(150, h, Qt::KeepAspectRatioByExpanding);
                    image = image.copy(0, 0, 150, h);
                    {
                        QPainter painter(&image);
                        painter.setPen(QColor(100,100,100));
                        painter.drawRect(0, 0, image.width()-1, image.height()-1);
                    }
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setIconPixmap(image);
                }
            }*/


            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(info);
            msgBox.setDetailedText(e.toOuterXml());
            msgBox.setWindowTitle("Element Info");
            msgBox.exec();
        }
        break;
    case 102:
        {
            QWebElement e = lastHitTestResult.element();
            if(e.isNull())
                e = lastHitTestResult.enclosingBlockElement();

            QString idt = Qt::escape("<" + e.tagName().toLower() + ">");
            if(!e.attribute("name").isEmpty())
                idt += Qt::escape(" '" + e.attribute("name") + "'");
            else if(!e.attribute("id").isEmpty())
                idt += Qt::escape(" '" + e.attribute("id") + "'");

            QString code = QInputDialog::getText(mainwindow, "Execute JavaScript", "Code to evaluate on " + idt + ":");

            if(!code.isEmpty())
            {
                QVariant result = e.evaluateJavaScript(code);

                QString resultMessage = "JavaScript '<i>" + Qt::escape(code) + "</i>' evaluated.<br><br>Result: <i>'" + Qt::escape(result.toString()) + "'</i>";

                QMessageBox msgBox;
                msgBox.setTextFormat(Qt::RichText);
                msgBox.setText(resultMessage);
                msgBox.setWindowTitle("Execute JavaScript: Result");
                msgBox.exec();
            }
        }
        break;
    case 103:
        {
            QWebElement e = lastHitTestResult.element();
            if(e.isNull())
                e = lastHitTestResult.enclosingBlockElement();
            SourceView* sv = new SourceView();
            sv->setCode(e.toOuterXml());
            sv->setWindowTitle("Element Source");
            sv->show();
        }
        break;
    case 104:
        {
            /*QMessageBox msgBox;
            msgBox.setTextFormat(Qt::PlainText);
            msgBox.setText("Render Tree Dump\n\n" + page()->mainFrame()->renderTreeDump());
            msgBox.setWindowTitle("Render Tree Dump");
            msgBox.exec();*/
            SourceView* sv = new SourceView();
            sv->setCode("Render Tree\n" + this->url().toString() + "\n\n" + page()->mainFrame()->renderTreeDump());
            sv->setWindowTitle("Render Tree");
            sv->showNormal();
        }
        break;
    /*case 201:
        {
            WebPage* p = qobject_cast<WebPage*>(page());
            if(p)
            {
                QNetworkReply* reply = NetworkAccessManager::getDefault()->get(QNetworkRequest(url()));
                reply->waitForReadyRead(1000);
                p->download(reply);
            }
        }
        break;*/
    case 202:
        {
            if(!lastHitTestResult.pixmap().isNull())
            {
                QList<QByteArray> supportedFormats = QImageWriter::supportedImageFormats();

                QString extfmt = "Image Files (";
                foreach(QByteArray a, supportedFormats)
                {
                    extfmt += "*." + QString::fromAscii(a).toLower() + " ";
                }
                extfmt = extfmt.trimmed();
                extfmt += ");;All Files (*.*)";

                QString fname = QFileDialog::getSaveFileName(this, tr("Save Image as ..."), QDir(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation)).absoluteFilePath(QFileInfo(lastHitTestResult.imageUrl().path()).completeBaseName() + ".jpg"), extfmt, 0);
                if(!fname.isEmpty())
                {
                    lastHitTestResult.pixmap().save(fname, 0, 95);
                }

                DownloadManager::getStatic()->showX();
                DownloadManager::getStatic()->addFile(fname, lastHitTestResult.imageUrl());
            }
        }
        break;
    }
}

void WebView::contextMenuEvent(QContextMenuEvent* ev)
{
    if(page() == 0)
        return;

    //if(/*ev->modifiers() == Qt::NoModifier &&*/ QApplication::keyboardModifiers() == Qt::ShiftModifier && page()->swallowContextMenuEvent(ev))
    //    return;

    MainWindow* mainwindow = qobject_cast<MainWindow*>(getMainWindow());
    if(mainwindow == 0)
        return;

    if(page()->mainFrame() == 0)
        return;




    QMenu* menu = new ExtendedMenu(this);

    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onContextMenuActionTriggered(QAction*)));

    QWebHitTestResult hrs = page()->mainFrame()->hitTestContent(ev->pos());

    lastHitTestResult = hrs;


    if(!hrs.linkUrl().isEmpty())
    {
        { QAction* a = new QAction(tr("Open Link") + " " + hrs.linkUrl().toString().left(20) + "...", menu); a->setData(1); menu->addAction(a); }
        { QAction* a = new QAction(tr("Open Link in new Tab"), menu); a->setData(2); menu->addAction(a); }
        { QAction* a = new QAction(tr("Open Link in new Window"), menu); a->setData(3); menu->addAction(a); }
        { QAction* a = new QAction(tr("Copy Link to Clipboard"), menu); a->setData(8); menu->addAction(a); }
        menu->addSeparator();
    }


    if(pageAction(QWebPage::Copy)->isEnabled() || pageAction(QWebPage::Paste)->isEnabled())
    {
        menu->addAction(pageAction(QWebPage::Cut));
        menu->addAction(pageAction(QWebPage::Copy));
        menu->addAction(pageAction(QWebPage::Paste));
        menu->addSeparator();
    }

    if(lastHitTestResult.isContentSelected())
    {
        { QAction* a = new QAction(tr("Websearch") + " '" + selectedText().left(30) + "'", menu); a->setData(10); menu->addAction(a); }
        { QAction* a = new QAction(tr("Goto") + " '" + selectedText().left(30) + "'", menu); a->setData(11); menu->addAction(a); }
        { QAction* a = new QAction(tr("Wiki") + " '" + selectedText().left(30) + "'", menu); a->setData(12); menu->addAction(a); }
        menu->addSeparator();
    }

    if(pageAction(QWebPage::Undo)->isEnabled() || pageAction(QWebPage::Redo)->isEnabled())
    {
        menu->addAction(pageAction(QWebPage::Undo));
        menu->addAction(pageAction(QWebPage::Redo));
        menu->addSeparator();
    }




    bool imgshn = false;

    if(!hrs.imageUrl().isEmpty())
    {
        lastHitImageUrl = QUrl(hrs.imageUrl());
        { QAction* a = new QAction(tr("View Image") + " " + hrs.imageUrl().toString().left(20) + "...", menu); a->setData(4); menu->addAction(a); }
        { QAction* a = new QAction(tr("View Image in New Tab"), menu); a->setData(5); menu->addAction(a); }
        { QAction* a = new QAction(tr("View Image in New Window"), menu); a->setData(6); menu->addAction(a); }
        { QAction* a = new QAction(tr("Copy Image to Clipboard"), menu); a->setData(7); menu->addAction(a); }
        { QAction* a = new QAction(tr("Save Image to Disk ..."), menu); a->setData(202); menu->addAction(a); }
        menu->addSeparator();
        imgshn = true;
    }

    {
        QString bgimg = hrs.element().styleProperty("background-image", QWebElement::ComputedStyle);
        if(!bgimg.isEmpty() && bgimg.startsWith("url(") && !imgshn)
        {
            if(bgimg.startsWith("url("))
            {
                bgimg.remove("url(");
                bgimg.chop(1);
            }
            lastHitImageUrl = QUrl(bgimg);
            { QAction* a = new QAction(tr("View Background Image") + " " + bgimg.left(20) + "...", this); a->setData(4); menu->addAction(a); }
            { QAction* a = new QAction(tr("View Background Image in New Tab"), this); a->setData(5); menu->addAction(a); }
            { QAction* a = new QAction(tr("View Background Image in New Window"), this); a->setData(6); menu->addAction(a); }
            menu->addSeparator();
        }
    }

    bool expandextras = menu->actions().count() != 0;

    {
        QMenu* m;
        if(expandextras)
        {
            m = new ExtendedMenu(menu);
            m->setTitle(tr("Navigation"));
            menu->addMenu(m);
        } else {
            m = menu;
        }
        { QAction* a = new QAction(tr("Back"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(back())); m->addAction(a); }
        { QAction* a = new QAction(tr("Forward"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(forward())); m->addAction(a); }
        { QAction* a = new QAction(tr("Reload"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(reload())); m->addAction(a); }
        { QAction* a = new QAction(tr("Stop"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(abort())); m->addAction(a); }
    }

    if(!expandextras)
        menu->addSeparator();



    {
        QMenu* m = new ExtendedMenu(menu);
        m->setTitle(tr("File"));
        menu->addMenu(m);
        { QAction* a = new QAction(tr("Save as ..."), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(downloadCurrentPage())); m->addAction(a); }
        {
            QMenu* m2 = new ExtendedMenu(m);
            m2->setTitle(tr("Export"));
            m->addMenu(m2);
            { QAction* a = new QAction(tr("Image"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(exportImage())); m2->addAction(a); }
            { QAction* a = new QAction(tr("PDF / Postscript"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(exportPDF())); m2->addAction(a); }
            { QAction* a = new QAction(tr("SVG"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(exportSVG())); m2->addAction(a); }
        }
        m->addSeparator();
        { QAction* a = new QAction(tr("Print"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(print())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Print)); m->addAction(a); }
        m->addSeparator();
        { QAction* a = new QAction(tr("Back"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(back())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Back)); m->addAction(a); }
        { QAction* a = new QAction(tr("Forward"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(forward())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Forward)); m->addAction(a); }
        m->addSeparator();
        { QAction* a = new QAction(tr("Reload"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(reload())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::Refresh)); m->addAction(a); }
    }
    {
        QMenu* m = new ExtendedMenu(menu);
        m->setTitle(tr("Zoom"));
        menu->addMenu(m);
        { QAction* a = new QAction(tr("Zoom In"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(zoomIn())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::ZoomIn)); m->addAction(a); }
        { QAction* a = new QAction(tr("Zoom Out"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(zoomOut())); a->setShortcuts(QKeySequence::keyBindings(QKeySequence::ZoomOut)); m->addAction(a); }
        { QAction* a = new QAction(tr("Reset Zoom"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(resetZoom())); a->setShortcut(QKeySequence(tr("Ctrl+0"))); m->addAction(a); }
    }


    {
        QMenu* m;
        //if(expandextras)
        {
            m = new ExtendedMenu(menu);
            m->setTitle(tr("Extras"));
            menu->addMenu(m);
        }// else {
        //    m = menu;
        //}

        //if(!hrs.element().isNull())
        { QAction* a = new QAction(tr("Element Info"), this); a->setData(101); m->addAction(a); }
        { QAction* a = new QAction(tr("Element Source"), this); a->setData(103); m->addAction(a); }
        { QAction* a = new QAction(tr("Evaluate JavaScript"), this); a->setData(102); m->addAction(a); }

        m->addSeparator();

        { QAction* a = new QAction(tr("View Source"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(viewSource())); m->addAction(a); }
        { QAction* a = new QAction(tr("Traffic Log"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(showTrafficLog())); m->addAction(a); }
        { QAction* a = new QAction(tr("JS Debugger"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(showJavaScriptConsole())); m->addAction(a); }
        { QAction* a = new QAction(tr("Render Tree"), this); a->setData(104); m->addAction(a); }
        { QAction* a = new QAction(tr("Node Tree"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(dumpNodeTree())); m->addAction(a); }

        m->addSeparator();

        { QAction* a = new QAction(tr("Inspector"), this); connect(a, SIGNAL(triggered()), mainwindow, SLOT(inspect())); m->addAction(a); }
    }



    /*{
        QMenu* elementMenu = menu->addMenu(tr("Elements"));
        for(QWebElement e = hrs.element(); !e.isNull(); e = e.parent())
        {
            QString s = e.localName();

            if(!e.attribute("name").isEmpty())
                s += " '" + e.attribute("name") + "'";

            if(!e.attribute("id").isEmpty())
                s += " #" + e.attribute("id");

            elementMenu->addAction(s);

            e.
        }
    }*/



    menu->exec(ev->globalPos());
}


void WebView::mouseMoveEvent(QMouseEvent* ev)
{
    QWebView::mouseMoveEvent(ev);
    updateStatusTip(ev);
}

void WebView::updateStatusTip(QMouseEvent* ev)
{
    MainWindow* mainwindow = qobject_cast<MainWindow*>(getMainWindow());
    if(mainwindow == 0)
        return;

    /*if(mainwindow->statusBar()->isVisible() == false)
        return;*/

    if(page() == 0)
        return;

    if(page()->mainFrame() == 0)
        return;

    QWebHitTestResult hrs = page()->mainFrame()->hitTestContent(ev->pos());


    QString statusMessage = "";

    /*if(hrs.element().webFrame() != 0 && hrs.element().webFrame() != page()->mainFrame())
    {
        statusMessage = tr("Frame") + " " + hrs.element().webFrame()->url().toString();
    }
    else*/
    if(!hrs.linkUrl().isEmpty())
        statusMessage = (/*tr("Link") + " " +*/ hrs.linkUrl().toString());
    //else if(!hrs.imageUrl().isEmpty())
    //    statusMessage = (tr("Image") + " " + hrs.imageUrl().toString());
    /*else if(!hrs.element().isNull())
    {
        QWebElement element = hrs.element();
        QString tn = element.tagName().toLower();
        if(tn == "input")
        {
            QString t = element.attribute("type").toLower();
            if(t == "text")
                statusMessage = (tr("Textbox") + " '" + element.attribute("name") + "'");
            else if(t == "password")
                statusMessage = (tr("Password Field") + " '" + element.attribute("name") + "'");
            else if(t == "submit")
                statusMessage = (tr("Submit Button") + " '" + element.attribute("name") + "'");
            else
                statusMessage = (tr("Input") + " '" + element.attribute("name") + "'");
        } else if(tn == "button")
            statusMessage = (tr("Button") + " '" + element.attribute("name") + "'");
        else if(tn == "object")
            statusMessage = tr("Plugin");
        else if(tn == "embed")
            statusMessage = tr("Plugin") + " '" + element.attribute("src") + "'";
        else if(tn == "textarea")
            statusMessage = (tr("Textbox") + " '" + element.attribute("name") + "'");
    }*/

    QUrl source = page()->mainFrame()->url();
    if(hrs.element().webFrame() != 0)
        source = hrs.element().webFrame()->url();

    statusMessage = statusMessage.left(100);

    //if(statusMessage.isEmpty())
    //{
    /*QString src = source.host();
        if(src.startsWith("www."))
            src = src.mid(4);

        statusMessage = src + " " + statusMessage;*/
        //statusMessage = statusMessage;
    //} else {
    //    statusMessage = statusMessage + " (" + source.toString() + ")";
   // }

    statusMessage = statusMessage.left(200);



    //mainwindow->statusBar()->showMessage(statusMessage);

    mainwindow->setStatus(statusMessage);
    //qDebug(statusMessage.toAscii());
}
