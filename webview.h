/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Web View Class
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


#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include <QWebHitTestResult>

class WebView : public QWebView
{
    Q_OBJECT

private:
    bool _open_link_in_new_tab;
    quint64 popupTicket;
    bool loading;
    QWebHitTestResult lastHitTestResult;
    QUrl lastHitImageUrl;

public:
    explicit WebView(QWidget *parent = 0);
    bool shouldOpenLinkInNewTab();
    QWidget* getMainWindow();
    void setTabTitle(QWidget* tab, QString title);
    bool isLoading();
    bool takePopupTicket();
    void showErrorPage(QString key);

protected:
    virtual void mousePressEvent(QMouseEvent* ev);
    virtual void mouseReleaseEvent(QMouseEvent* ev);
    virtual void mouseMoveEvent(QMouseEvent* ev);
    virtual QWebView* createWindow(QWebPage::WebWindowType type);
    void contextMenuEvent(QContextMenuEvent* ev);

public:
    void updateStatusTip(QMouseEvent* ev);

signals:

public slots:

private slots:
    void onTitleChanged(QString title);
    void onLoadFinished(bool ok);
    void onLoadStarted();
    void onLinkMb(QUrl u);
    void onContextMenuActionTriggered(QAction* a);
};

#endif // WEBVIEW_H
