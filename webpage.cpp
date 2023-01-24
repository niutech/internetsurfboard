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

#include "webpage.h"
#include <QtGui>
#include <QtWebKit>
#include "networkaccessmanager.h"
#include "javascriptconsole.h"
#include "downloadmanager.h"
#include "webview.h"

WebPage::WebPage(QObject * parent) : QWebPage(parent)
{
    _ignore_alerts = false;
    _ignore_confirms = false;
    setNetworkAccessManager(NetworkAccessManager::getDefault());
    connect(this, SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(onDownloadRequested(QNetworkRequest)));
    connect(this, SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(download(QNetworkReply*)));
    setForwardUnsupportedContent(true);
}

QString WebPage::userAgentForUrl(const QUrl& url) const
{
    return QWebPage::userAgentForUrl(url);
    //return QWebPage::userAgentForUrl(url) + " " + QApplication::applicationName() /*" Weltweitimnetz Browser"*/;
}




void WebPage::javaScriptAlert ( QWebFrame * frame, const QString & msg )
{
    if(_ignore_alerts)
        return;
    QMessageBox msgBox;
    //msgBox.setWindowTitle("Message from " + frame->url().host().left(100));
    msgBox.setWindowTitle(frame->url().host().left(100));

    //QString m = "Message from " + frame->url().host().left(100) + ":<br>";
    //m += "<i>" + Qt::escape(msg) + "</i>";

    QString m = msg;

    msgBox.setText(m);
    msgBox.setTextFormat(Qt::PlainText);
    //msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.addButton(QMessageBox::NoToAll);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    msgBox.setIconPixmap(QPixmap(":/message.png"));
    //msgBox.setDetailedText(tr("A website has requested the browser to show an 'alert' box.\n\nPress 'OK' to continue.\n\nPress 'Ignore' to ignore all further alert boxes requested by this page."));
    if( msgBox.exec() == QMessageBox::NoToAll )
    {
        _ignore_alerts = true;
    }
}

bool WebPage::javaScriptConfirm ( QWebFrame * frame, const QString & msg )
{
    if(_ignore_confirms)
        return false;
    QMessageBox msgBox;
    //msgBox.setWindowTitle(frame->url().toString().left(100));
    msgBox.setWindowTitle(frame->url().host().left(100));
    msgBox.setText(msg);
    //msgBox.setIcon(QMessageBox::Question);
    msgBox.setTextFormat(Qt::PlainText);

    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.addButton(QMessageBox::NoToAll);

    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setEscapeButton(QMessageBox::No);

    msgBox.setIconPixmap(QPixmap(":/message.png"));

    //msgBox.setDetailedText(tr("A website has requested the browser to show a 'confirm' box.\nPress 'Yes' or 'No' to answer it or press 'Ignore' to ignore this and all further 'confirm' boxes requested by this page."));
    int rs = msgBox.exec();
    switch(rs)
    {
    case QMessageBox::NoToAll:
        _ignore_confirms = true;
        return false;
    case QMessageBox::Yes:
        return true;
    case QMessageBox::No:
        return false;
    }
    return false;
}

void WebPage::javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID )
{
    //qDebug(QString("js: " + message + " [" + sourceID + "]").toAscii());
    JavaScriptConsole::getStatic()->log(message, lineNumber, sourceID);
}

bool WebPage::javaScriptPrompt ( QWebFrame * frame, const QString & msg, const QString & defaultValue, QString * result )
{
    return false;
}


bool WebPage::shouldInterruptJavaScript()
{
    QMessageBox msgBox;
    msgBox.setText(tr("A script on this page has hung up. Do you want to stop it?"));
    msgBox.setWindowTitle(this->mainFrame() ? this->mainFrame()->url().toString().left(100) : "error");
    msgBox.setIcon(QMessageBox::Question);

    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);

    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setEscapeButton(QMessageBox::No);

    //msgBox.setDetailedText(tr("An endless loop condition may be either caused by a design error on the current website or by a malicious attempt to freeze your webbrowser or computer."));

    return msgBox.exec() == QMessageBox::Yes;
}

QString WebPage::chooseFile(QWebFrame* parentFrame, const QString& suggestedFile)
{
    WebView* view = qobject_cast<WebView*>(this->view());
    if(view == 0)
        return "";
    if(view->takePopupTicket() == false)
        return "";
    return QFileDialog::getOpenFileName(0, "Upload file to " + parentFrame->url().host() + " ...", suggestedFile);
}



void WebPage::onDownloadRequested(const QNetworkRequest& request)
{
    // NYI
    // QMessageBox::information(0, "Download requested", request.url().toString());

    //request.
}

 void WebPage::download(QNetworkReply* reply)
 {
     QString path = reply->url().path();

     QFileInfo info(path);
     QString baseName = info.completeBaseName();
     QString endName = info.suffix().trimmed();

     /*if(baseName.isEmpty() && endName.isEmpty())
         baseName = reply->url().host();*/

     /*qDebug(reply->header(QNetworkRequest::ContentTypeHeader).toString().toAscii());
     if(endName.isEmpty() && reply->header(QNetworkRequest::ContentTypeHeader).toString().toLower().contains("html"))
         endName = "htm";*/

     if (baseName.isEmpty() || endName.isEmpty())
     {
         reply->close();
         return;
     }


     QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();


     QString headers;
     QList<QPair<QByteArray, QByteArray> > rawHeaderPairs = reply->rawHeaderPairs();
     QPair<QByteArray, QByteArray> p;
     foreach(p, rawHeaderPairs)
     {
         headers.append(QString::fromAscii(p.first) + ": " + QString::fromAscii(p.second) + "\n");
     }
     headers = headers.trimmed();


     QString justthefilename = baseName + QLatin1Char('.') + endName;


     QMessageBox msgBox;
     msgBox.setWindowTitle("Download requested ...");
     msgBox.setText("<i><b>" + Qt::escape(justthefilename) + "</b><br>(" + Qt::escape(reply->url().toString().left(200)) + ")</i><br><br>Do you want to download this file?");
     msgBox.setIcon(QMessageBox::Question);
     msgBox.addButton(QMessageBox::Yes);
     msgBox.addButton(QMessageBox::No);
     msgBox.setDefaultButton(QMessageBox::Yes);
     msgBox.setEscapeButton(QMessageBox::No);
     msgBox.setDetailedText("URL\n" + reply->url().toString() + "\n\nHeaders\n" + headers);
     if(msgBox.exec() != QMessageBox::Yes)
         return;




     QString directory;
     if(contentType.contains("image"))
         directory = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
     else if(contentType.contains("video"))
         directory = QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);
     else if(contentType.contains("audio"))
         directory = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
     else
         directory = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
     if(!directory.endsWith('/') && !directory.endsWith('\\'))
        directory += "/";
     QString name = directory + baseName + QLatin1Char('.') + endName;
     if (QFile::exists(name))
     {
         int i = 1;
         do
         {
             name = directory + baseName + QLatin1Char('-') + QString::number(i++) + QLatin1Char('.') + endName;
         } while (QFile::exists(name));
     }


     QString downloadpath = QFileDialog::getSaveFileName(0, tr("Download file ..."), name);
     if(downloadpath.isEmpty())
         return;


     DownloadManager::getStatic()->showX();
     DownloadManager::getStatic()->download(reply, downloadpath);
 }


 bool WebPage::supportsExtension(QWebPage::Extension extension) const
 {
     if(extension == QWebPage::ErrorPageExtension)
     {
         return true;
     }

     return QWebPage::supportsExtension(extension);
 }
 bool WebPage::extension(QWebPage::Extension extension, const QWebPage::ExtensionOption* option, QWebPage::ExtensionReturn* output)
 {
     if(extension == QWebPage::ErrorPageExtension)
     {
         ErrorPageExtensionOption* xo = (ErrorPageExtensionOption*)option;

         if(xo->frame->parentFrame() == 0 && xo->error != 102 && xo->error != 200)
         {
             ErrorPageExtensionReturn* ret = (ErrorPageExtensionReturn*)output;
             ret->baseUrl = xo->url;
             //ret->content = xo->errorString.toAscii();
             ret->contentType = "text/html";





             QFile f(":/error.htm");
             f.open(QFile::ReadOnly);

             QString s = QString::fromUtf8(f.readAll());

             s.replace(QString("{error}"), QString::number(xo->error));
             s.replace(QString("{errorstring}"), xo->errorString);
             s.replace(QString("{url}"), xo->url.toString());
             s.replace(QString("{signature}"), QApplication::applicationName() + " " + QApplication::applicationVersion());

             ret->content = s.toUtf8();


             return true;
         }
     }

     return QWebPage::extension(extension, option, output);
 }
