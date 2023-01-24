/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Network Access Manager
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


#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QtWebKit>
#include <QtCore>

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit NetworkAccessManager(QObject *parent = 0);

    static NetworkAccessManager* getDefault();

signals:
    void started(QNetworkReply* req);

public slots:
    void onSslErrors(QNetworkReply* reply, const QList<QSslError>& errors);
    void onAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);;

protected:
    QNetworkReply* createRequest(Operation op, const QNetworkRequest& req, QIODevice* outgoingData = 0);

protected:
    QStringList sslExceptions;
};

#endif // NETWORKACCESSMANAGER_H
