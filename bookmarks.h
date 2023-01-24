/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Bookmark Manager
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


#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QtCore>
#include <QAbstractItemModel>
#include <QAbstractListModel>

class BookmarkList : public QObject
{
    Q_OBJECT

private:
    QList<QPair<QString, QString> > l;
    QString fpath;

public:
    explicit BookmarkList(QObject *parent = 0);
    ~BookmarkList();

    void add(QString url, QString title);
    QString title(int i) const;
    QString url(int i) const;

    void swap(QString url1, QString url2);

    void remove(QString url);

    int count() const;

    static BookmarkList* globalBookmarkList();

    void save();

    void setData(const QList<QPair<QString, QString> >& d);

    /*virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data (const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    virtual bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    virtual bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
    virtual Qt::DropActions supportedDragActions() const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    virtual QStringList mimeTypes() const;*/

signals:

public slots:

};

#endif // BOOKMARKS_H
