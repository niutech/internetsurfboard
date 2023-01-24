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

#include "trafficlog.h"
#include "ui_trafficlog.h"
#include <QtNetwork>
#include "networkaccessmanager.h"
#include <QtGui>
#include "mainwindow.h"
#include "desktopwindowmanager.h"

TrafficLog::TrafficLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TrafficLog)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(6);

    QStringList headers;
    headers << tr("ID");
    headers << tr("Date/Time");
    headers << tr("Method");
    headers << tr("Status");
    headers << tr("Content Type");
    headers << tr("URL");
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    ui->tableWidget->setColumnWidth(0, 30);
    ui->tableWidget->setColumnWidth(1, 130);
    ui->tableWidget->setColumnWidth(2, 60);
    ui->tableWidget->setColumnWidth(3, 80);
    ui->tableWidget->setColumnWidth(4, 120);
    ui->tableWidget->setColumnWidth(5, 300);

    connect(NetworkAccessManager::getDefault(), SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    connect(NetworkAccessManager::getDefault(), SIGNAL(started(QNetworkReply*)), this, SLOT(onReplyStarted(QNetworkReply*)));

    ui->actionEnableCapture->setChecked(true);

    ui->actionAutoScroll->setChecked(true);


    //setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    setWindowFlags(/*Qt::Tool |*/ windowFlags() | Qt::WindowStaysOnTopHint);
    //setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::extendFrameIntoClientArea(this, 50, 20, 20, 20));
    //setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::setBlurBehind(this));

    qDebug("TRAFFIC LOGGER INSTALLED");
}

TrafficLog::~TrafficLog()
{
    delete ui;
}


void TrafficLog::onReplyStarted(QNetworkReply * reply)
{
    if(ui->actionEnableCapture->isChecked() && isVisible())
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(row + 1);
        QTableWidgetItem* item;

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setText(QString::number((long)reply));
        ui->tableWidget->setItem(row, 0, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setText(QDateTime::currentDateTime().toString(Qt::ISODate));
        ui->tableWidget->setItem(row, 1, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        switch(reply->operation())
        {
        case QNetworkAccessManager::HeadOperation:
            item->setText("HEAD");
            break;
        case QNetworkAccessManager::GetOperation:
            item->setText("GET");
            break;
        case QNetworkAccessManager::PutOperation:
            item->setText("PUT");
            break;
        case QNetworkAccessManager::PostOperation:
            item->setText("POST");
            break;
        case QNetworkAccessManager::DeleteOperation:
            item->setText("DELETE");
            break;
        }
        ui->tableWidget->setItem(row, 2, item);


        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setText(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() + " " + reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        ui->tableWidget->setItem(row, 3, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setText(reply->header(QNetworkRequest::ContentTypeHeader).toString());
        ui->tableWidget->setItem(row, 4, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setText(reply->url().toString());
        ui->tableWidget->setItem(row, 5, item);

        if(ui->actionAutoScroll->isChecked())
        {
            ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->maximum());
        }
    }
}

void TrafficLog::onReplyFinished(QNetworkReply * reply)
{
    if(ui->actionEnableCapture->isChecked() && isVisible())
    {
        int row = -1;

        QString t = QString::number((long)reply);
        for(int i = ui->tableWidget->rowCount() - 1; i >= 0; i--)
        {
            if(ui->tableWidget->item(i, 0)->text() == t)
                row = i;
        }

        if(row < 0)
            return;

        QTableWidgetItem* item;

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setText(QString::number((long)reply));
        ui->tableWidget->setItem(row, 0, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setText(QDateTime::currentDateTime().toString(Qt::ISODate));
        ui->tableWidget->setItem(row, 1, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        switch(reply->operation())
        {
        case QNetworkAccessManager::HeadOperation:
            item->setText("HEAD");
            break;
        case QNetworkAccessManager::GetOperation:
            item->setText("GET");
            break;
        case QNetworkAccessManager::PutOperation:
            item->setText("PUT");
            break;
        case QNetworkAccessManager::PostOperation:
            item->setText("POST");
            break;
        case QNetworkAccessManager::DeleteOperation:
            item->setText("DELETE");
            break;
        }
        ui->tableWidget->setItem(row, 2, item);


        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setText(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() + " " + reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        ui->tableWidget->setItem(row, 3, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setText(reply->header(QNetworkRequest::ContentTypeHeader).toString());
        ui->tableWidget->setItem(row, 4, item);

        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setText(reply->url().toString());
        ui->tableWidget->setItem(row, 5, item);

        if(ui->actionAutoScroll->isChecked())
        {
            ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->maximum());
        }
    }
}

void TrafficLog::on_actionClear_triggered()
{
    ui->tableWidget->setRowCount(0);
}

void TrafficLog::on_tableWidget_doubleClicked(QModelIndex index)
{
    if(!index.isValid())
        return;

    MainWindow* mwnd = 0;
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        if (!widget->isHidden())
        {
            MainWindow* m = qobject_cast<MainWindow*>(widget);
            if(m)
            {
                mwnd = m;
                break;
            }
        }
    }

    QString url = ui->tableWidget->item(index.row(), 5)->text();
    if(!url.isEmpty())
    {
        mwnd->loadPage(url);
        ui->actionAutoScroll->setChecked(false);
    }
}

void TrafficLog::on_actionTopmost_triggered()
{
    setWindowFlags(ui->actionTopmost->isChecked() ? (windowFlags() | Qt::WindowStaysOnTopHint) : (Qt::WindowType)(~(windowFlags() | (~Qt::WindowStaysOnTopHint))));
    show();
}
