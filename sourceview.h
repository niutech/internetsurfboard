/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**   > Sourcecode Window
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


#ifndef SOURCEVIEW_H
#define SOURCEVIEW_H

#include <QMainWindow>
#include <QSyntaxHighlighter>

namespace Ui {
    class SourceView;
}

class SourceView : public QMainWindow
{
    Q_OBJECT

public:
    explicit SourceView(QWidget *parent = 0);
    ~SourceView();

    void setCode(const QString& code);

private:
    Ui::SourceView *ui;
};

class XmlSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit XmlSyntaxHighlighter(QObject* parent);
    explicit XmlSyntaxHighlighter(QTextDocument* parent);
    explicit XmlSyntaxHighlighter(QTextEdit* parent);

protected:

    void highlightBlock(const QString &text);
};

#endif // SOURCEVIEW_H
