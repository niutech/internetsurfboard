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

#include "networkaccessmanager.h"
#include <Qt>
#include <QMessageBox>
#include <QTextDocument>
#include <QtGui>

NetworkAccessManager::NetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
    connect(this, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(onSslErrors(QNetworkReply*,QList<QSslError>)));
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(onAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
}


NetworkAccessManager* defAccessManager = 0;
NetworkAccessManager* NetworkAccessManager::getDefault()
{
    if(!defAccessManager)
        defAccessManager = new NetworkAccessManager();
    return defAccessManager;
}


void NetworkAccessManager::onAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{
    QDialog diag;
    QFormLayout layout;
    diag.setLayout(&layout);
    QLabel l("Login: " + reply->url().host() /*authenticator->realm()*/);
    layout.addRow(&l);

    QLineEdit username;
    QLineEdit password;
    password.setEchoMode(QLineEdit::Password);
    layout.addRow(tr("User Name"), &username);
    layout.addRow(tr("Password"), &password);

    QDialogButtonBox buttonbox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    layout.addRow(&buttonbox);

    connect(&buttonbox, SIGNAL(accepted()), &diag, SLOT(accept()));
    connect(&buttonbox, SIGNAL(rejected()), &diag, SLOT(reject()));

    if(diag.exec() == QDialog::Accepted)
    {
        authenticator->setUser(username.text());
        authenticator->setPassword(password.text());
    }
}

QNetworkReply* NetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest& req, QIODevice* outgoingData)
{
    QNetworkReply* reply = QNetworkAccessManager::createRequest(op, req, outgoingData);
    if(reply)
        emit started(reply);
    return reply;
}


void NetworkAccessManager::onSslErrors(QNetworkReply* reply, const QList<QSslError>& _errors)
{
    QList<QSslError> errorsToIgnore;

    QList<QSslError> errors = _errors;

    for(int i = 0; i < errors.length(); )
    {
        QString errorkey = reply->url().host() + "-" + QString::number((int)errors.at(i).error());
        if(sslExceptions.contains(errorkey))
        {
            errorsToIgnore.append(errors.at(i));
            errors.removeAt(i);
        }
        else
            i++;
    }

    if(errors.length() > 0)
    {
        QString msg = tr("<b>Secure connection to <u>%1</u> failed.</b><br>").arg(Qt::escape(reply->url().host()));

        msg += "<font color=darkred>";
        foreach(QSslError err, errors)
        {
            msg += "<nobr>" + tr("Error") + " #" + Qt::escape(QString::number((int)err.error())) + ": " + Qt::escape(err.errorString()) + ".</nobr><br>";
        }
        msg += "</font>";

        msg += "Click 'Cancel' to stop loading.<br>Click 'Ignore' to continue loading.";

        QMessageBox msgbox;
        msgbox.setText(msg);
        msgbox.setTextFormat(Qt::RichText);
        msgbox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ignore);
        msgbox.setDefaultButton(QMessageBox::Cancel);
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setWindowTitle("SSL Error");
        if(msgbox.exec() == QMessageBox::Ignore)
        {
            foreach(QSslError error, errors)
            {
                //reply->ignoreSslErrors(errors);
                errorsToIgnore.append(error);

                QString errorkey = reply->url().host() + "-" + QString::number((int)error.error());
                sslExceptions.append(errorkey);
            }
        }
    }

    reply->ignoreSslErrors(errorsToIgnore);
}


