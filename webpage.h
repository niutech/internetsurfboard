/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Web Page Class
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


#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>

class WebPage : public QWebPage
{
    Q_OBJECT

private:
    bool _ignore_alerts;
    bool _ignore_confirms;

public:
    WebPage(QObject * parent = 0);
    virtual QString userAgentForUrl(const QUrl& url) const;

protected:
    virtual void javaScriptAlert ( QWebFrame * frame, const QString & msg );
    virtual bool javaScriptConfirm ( QWebFrame * frame, const QString & msg );
    virtual void javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID );
    virtual bool javaScriptPrompt ( QWebFrame * frame, const QString & msg, const QString & defaultValue, QString * result );
    virtual QString chooseFile(QWebFrame* parentFrame, const QString& suggestedFile);

    bool extension(QWebPage::Extension extension, const QWebPage::ExtensionOption* option = 0, QWebPage::ExtensionReturn* output = 0);
    bool supportsExtension(QWebPage::Extension extension) const;

signals:

protected slots:
    bool shouldInterruptJavaScript();
    void onDownloadRequested(const QNetworkRequest& request);

public slots:
    void download(QNetworkReply* reply);
};

#endif // WEBPAGE_H
