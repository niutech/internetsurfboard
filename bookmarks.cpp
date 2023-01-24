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

#include "bookmarks.h"

#include <QDesktopServices>
#include <QApplication>

BookmarkList::BookmarkList(QObject *parent) :
    QObject(parent)
{
    QDir().mkdir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));

    fpath = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).absoluteFilePath("bookmarks.txt");

    if(!QFile::exists(fpath))
    {
        QFile::copy(QDir(QApplication::applicationDirPath()).absoluteFilePath("defaultbookmarks.txt"), fpath);
    }

    QFile f(fpath);

    f.open(QFile::ReadOnly);
    QList<QByteArray> lines = f.readAll().split('\n');
    f.close();

    for(int i = 0; i < lines.count(); i++)
    {
        QList<QByteArray> parts = lines.at(i).split(' ');
        if(parts.length() > 1)
        {
            l.append(QPair<QString, QString>(QUrl::fromPercentEncoding(parts[0]), QUrl::fromPercentEncoding(parts[1])));
        }
    }
}

BookmarkList::~BookmarkList()
{

}



void BookmarkList::swap(QString url1, QString url2)
{
    int p1 = -1;
    int p2 = -1;

    for(int i = 0; i < count(); i++)
    {
        if(url(i) == url1)
            p1 = i;
        if(url(i) == url2)
            p2 = i;
    }

    if(p1 >= 0 && p2 >= 0)
    {
        l.swap(p1, p2);
    }
}

void BookmarkList::remove(QString _url)
{
    for(int i = 0; i < count(); i++)
    {
        if(url(i) == _url)
        {
            l.removeAt(i);
            return;
        }
    }
}

void BookmarkList::save()
{
    QFile f(fpath);
    f.open(QFile::WriteOnly);

    for(int i = 0; i < l.count(); i++)
    {
        f.write(QUrl::toPercentEncoding(url(i)));
        f.write(" ");
        f.write(QUrl::toPercentEncoding(title(i)));
        f.write("\n");
    }

    f.close();
}

void BookmarkList::add(QString _url, QString title)
{
    for(int i = 0;i < count(); i++)
    {
        if(url(i).trimmed() == _url.trimmed())
        {
            l.removeAt(i);
        }
    }
    l.prepend(QPair<QString, QString>(_url.trimmed(), title.trimmed()));
}

QString BookmarkList::title(int i) const
{
    return l.at(i).second;
}

QString BookmarkList::url(int i) const
{
    return l.at(i).first;
}

int BookmarkList::count() const
{
    return l.count();
}


BookmarkList* _staticBookmarkList = 0;

BookmarkList* BookmarkList::globalBookmarkList()
{
   if(_staticBookmarkList == 0)
       _staticBookmarkList = new BookmarkList();
   return _staticBookmarkList;
}


void BookmarkList::setData(const QList<QPair<QString, QString> >& d)
{
    l = d;
}

/*
int BookmarkList::rowCount(const QModelIndex& parent) const
{
    return l.count();
}
QVariant BookmarkList::data (const QModelIndex& index, int role ) const
{
    //return title(index.row());

    if (!index.isValid())
        return QVariant();

    if (index.row() >= l.size())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return title(index.row());
    else if (role == Qt::ToolTipRole)
        return url(index.row());
    else
        return QVariant();
}

QVariant BookmarkList::headerData(int section, Qt::Orientation orientation,
                                      int role) const
 {
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal)
         return QString("Column %1").arg(section);
     else
         return QString("Row %1").arg(section);
 }


Qt::ItemFlags BookmarkList::flags ( const QModelIndex & index ) const
{

    if (index.isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
    else
        return Qt::NoItemFlags;
}
bool BookmarkList::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    if (index.row() >= l.size())
        return false;

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        l[index.row()].second = value.toString();
        return true;
    }
    else if (role == Qt::ToolTipRole)
    {
        l[index.row()].first = value.toString();
        return true;
    }
    return false;
}

bool BookmarkList::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
    {
        l.insert(position, QPair<QString, QString>("", ""));
    }

    endInsertRows();
    return true;
}

bool BookmarkList::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
    {
        l.removeAt(position);
    }

    endRemoveRows();
    return true;
}


Qt::DropActions BookmarkList::supportedDropActions() const
{
    return  Qt::MoveAction;
}

Qt::DropActions BookmarkList::supportedDragActions() const
{
    return  Qt::MoveAction;
}



QMimeData *BookmarkList::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    foreach (QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            if(index.row() > 0 && index.row() < l.count())
                encodedData += l[index.row()].first.toUtf8() + '\0' + l[index.row()].second.toUtf8() + '\0';
        }
    }

    mimeData->setData("application/x.text.list", encodedData);
    return mimeData;
}

bool BookmarkList::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
 {
     if (action == Qt::IgnoreAction)
         return true;

     if (!data->hasFormat("application/x.text.list"))
         return false;

     if (column > 0)
         return false;

     int beginRow = row;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());


    QByteArray encodedData = data->data("application/x.text.list");

    QList<QByteArray> parts = encodedData.split('\0');

    for(int i = 0; i < (parts.count() - 1) / 2; i++)
    {
        l.insert(beginRow, QPair<QString, QString>(QString::fromUtf8(parts[i*2]), QString::fromUtf8(parts[i*2+1])));
        beginRow++;
    }

    return true;
}


QStringList BookmarkList::mimeTypes() const
 {
     QStringList types;
     types << "application/x.text.list";
     return types;
 }
*/
