/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Download Manager
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


#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QTreeWidgetItem>
#include <QFile>

namespace Ui {
    class DownloadManager;
}

class DownloadManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit DownloadManager(QWidget *parent = 0);
    ~DownloadManager();

    static DownloadManager* getStatic();

    void download(QNetworkReply* reply, QString destination);

    void addFile(QString fileName, QUrl source = QUrl(), QString status = QString());

    void download(QUrl source);

public slots:

    void showX();

private:
    Ui::DownloadManager *ui;
    bool empty;

private slots:
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column);
};

class DownloadItem : public QObject
{
    Q_OBJECT

private:
    QTreeWidgetItem* item;
    QNetworkReply* reply;
    QString fname;
    QFile* file;

public:
    DownloadItem(QNetworkReply* reply, QString fname, QTreeWidgetItem* item);

    void cancelDownload();

protected slots:
    void onReadyRead();
    void onFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // DOWNLOADMANAGER_H
