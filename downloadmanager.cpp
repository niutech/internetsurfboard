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

#include "downloadmanager.h"
#include "ui_downloadmanager.h"
#include "desktopwindowmanager.h"
#include "networkaccessmanager.h"
#include "browserapplication.h"

#include <QtCore>
#include <QtGui>

DownloadManager::DownloadManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DownloadManager)
{
    ui->setupUi(this);


    QStringList headers;
    headers << tr("File") << tr("Progress") << tr("Source");
    ui->treeWidget->setHeaderLabels(headers);

    ui->treeWidget->setColumnWidth(0, 200);
    ui->treeWidget->setColumnWidth(1, 180);

    ui->treeWidget->setIconSize(QSize(64,64));

    //DesktopWindowManager::setBlurBehind(this, true);

    if(DesktopWindowManager::isSupported())
    {
        setObjectName("xdownloadmanager");
        ui->treeWidget->setObjectName("xdownloadlist");
    } else {
        setObjectName("downloadmanager");
        ui->treeWidget->setObjectName("downloadlist");
    }


    //setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::isSupported());

    DesktopWindowManager::setBlurBehind(this);


    empty = true;

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, tr("No files downloaded."));
    item->setFirstColumnSpanned(true);
    item->setDisabled(true);
    ui->treeWidget->addTopLevelItem(item);

    ui->treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

DownloadManager::~DownloadManager()
{
    delete ui;
}

void DownloadManager::addFile(QString fname, QUrl source, QString status)
{
    if(empty == true)
    {
        empty = false;
        ui->treeWidget->takeTopLevelItem(0);
        ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QFileInfo(fname).fileName());
    item->setText(1, status.isEmpty() ? "ok (downloaded)" : status);
    item->setText(2, source.toString());
    item->setData(0, Qt::UserRole, fname);
    QFileIconProvider provider;
    item->setIcon(0, provider.icon(QFileInfo(fname)));
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->scrollToItem(item);

    ui->treeWidget->clearSelection();
    item->setSelected(true);
}

void DownloadManager::download(QNetworkReply* reply, QString destination)
{
    if(empty == true)
    {
        empty = false;
        ui->treeWidget->takeTopLevelItem(0);
        ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, destination);
    ui->treeWidget->addTopLevelItem(item);

    new DownloadItem(reply, destination, item);

    ui->treeWidget->scrollToItem(item);
}


DownloadItem::DownloadItem(QNetworkReply* reply, QString fname, QTreeWidgetItem* item)
{
    this->reply = reply;
    this->fname = fname;
    this->item = item;

    item->setText(0, QFileInfo(fname).fileName());
    item->setText(1, "");
    item->setText(2, reply->url().toString());

    item->setData(1, Qt::UserRole, (qlonglong)this);

    //item->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    QFileIconProvider provider;
    item->setIcon(0, provider.icon(QFileIconProvider::File));

    file = new QFile(fname);
    if(!file->open(QIODevice::WriteOnly))
    {
        item->setText(1, "error");
        return;
    }

    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
}

void DownloadItem::cancelDownload()
{
    if(item)
    {
        reply->abort();
        item->setText(1, "canceled");
    }
}


void DownloadItem::onReadyRead()
{
    file->write(reply->readAll());
}

void DownloadItem::onFinished()
{
    item->setText(1, tr("ok (downloaded)"));
    item->setData(0, Qt::UserRole, fname);
    file->close();

    QFileIconProvider provider;
    item->setIcon(0, provider.icon(QFileInfo(fname)));


}

void DownloadItem::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    item->setText(1, QString::number((bytesReceived * 1000 / bytesTotal) * 0.1, 'f', 1) + "% "
                  + "(" + QString::number(bytesReceived / 1024 * 0.001, 'f', 3) + " / " + QString::number(bytesTotal / 1024 * 0.001, 'f', 3) + " mb)");
}



void DownloadManager::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    QString fname = item->data(0, Qt::UserRole).toString();
    if(!fname.isEmpty())
    {
        QString suffix = QFileInfo(fname).suffix();
        if(suffix == "htm" || suffix == "html")
            BrowserApplication::getStatic()->openUrl(QUrl("file:///" + fname));
        else
            QDesktopServices::openUrl("file:///" + fname);
    } else {
        //QMessageBox::information(this, tr("Can't open file ..."), tr("Can't open file. Download not yet finished."));

        QMessageBox msgbox;
        msgbox.setWindowTitle(tr("Download not yet finished."));
        msgbox.setText("Download not yet finished. Can't open file.\n\nPress 'OK' to continue.\nPress 'Abort' to cancel download.");
        msgbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        msgbox.setDefaultButton(QMessageBox::Ok);
        msgbox.setIcon(QMessageBox::Information);
        if(msgbox.exec() == QMessageBox::Abort)
        {
            qlonglong l = item->data(1, Qt::UserRole).toLongLong();
            if(l != 0)
            {
                DownloadItem* item = (DownloadItem*)l;
                item->cancelDownload();
            }
        }
    }
}


void DownloadManager::showX()
{
    show();
    raise();
    activateWindow();
}



DownloadManager* _xxxdlmngTheDownloadManager = 0;

DownloadManager* DownloadManager::getStatic()
{
    if(_xxxdlmngTheDownloadManager == 0)
        _xxxdlmngTheDownloadManager = new DownloadManager();
    return _xxxdlmngTheDownloadManager;
}




void DownloadManager::download(QUrl source)
{
    QString f = QFileInfo(source.path()).fileName();
    if(f.isEmpty())
        f = source.host() + ".htm";

    if(QFileInfo(f).suffix().isEmpty())
        f += ".htm";

    f = QDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).absoluteFilePath(f);
    QString fname = QFileDialog::getSaveFileName(0, "Save file as ...", f);
    if(fname.isNull())
        return;

    if(QFileInfo(fname).suffix().isEmpty())
        fname += ".htm";

    download(NetworkAccessManager::getDefault()->get(QNetworkRequest(source)), fname);

    showX();
}
