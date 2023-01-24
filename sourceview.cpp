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

#include "sourceview.h"
#include "ui_sourceview.h"
#include "desktopwindowmanager.h"

SourceView::SourceView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SourceView)
{
    ui->setupUi(this);
    XmlSyntaxHighlighter *highlighter = new XmlSyntaxHighlighter(ui->textEdit->document());
}

SourceView::~SourceView()
{
    delete ui;
}

void SourceView::setCode(const QString& code)
{
    ui->textEdit->setPlainText(code);
}



void XmlSyntaxHighlighter::highlightBlock(const QString &text)
{
    //QString pattern = "\\b[A-Za-z]+\\b";
    //QRegExp expression(pattern);

    {
        QTextCharFormat myClassFormat;
        myClassFormat.setFontWeight(QFont::Normal);
        myClassFormat.setForeground(Qt::darkBlue);
        myClassFormat.setFontItalic(false);
        myClassFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);

        QRegExp expression(QString("<*>"), Qt::CaseInsensitive, QRegExp::Wildcard);
        int index = text.indexOf(expression);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, myClassFormat);
            index = text.indexOf(expression, index + length);
        }
    }

    {
        QTextCharFormat myClassFormat;
        myClassFormat.setFontWeight(QFont::Normal);
        myClassFormat.setForeground(Qt::darkGreen);
        myClassFormat.setFontItalic(false);
        myClassFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);

        QRegExp expression(QString("<!--*-->"), Qt::CaseInsensitive, QRegExp::Wildcard);
        int index = text.indexOf(expression);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, myClassFormat);
            index = text.indexOf(expression, index + length);
        }
    }


    {
        QTextCharFormat myClassFormat;
        myClassFormat.setFontWeight(QFont::Bold);
        myClassFormat.setForeground(Qt::darkBlue);
        myClassFormat.setFontItalic(false);
        myClassFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);

        QRegExp expression(QString("\\<[\\/0-9A-Za-z\\>]+"), Qt::CaseInsensitive, QRegExp::RegExp);
        int index = text.indexOf(expression);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, myClassFormat);
            index = text.indexOf(expression, index + length);
        }
    }

    {
        QTextCharFormat myClassFormat;
        myClassFormat.setFontWeight(QFont::Normal);
        myClassFormat.setForeground(Qt::darkBlue);
        myClassFormat.setFontItalic(false);
        myClassFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

        QRegExp expression(QString("[0-9A-Za-z]+://[0-9A-Za-z/\\.\\?\\+\\%\\-\\=\\&\\;]+"), Qt::CaseInsensitive, QRegExp::RegExp);
        int index = text.indexOf(expression);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, myClassFormat);
            index = text.indexOf(expression, index + length);
        }
    }
}


XmlSyntaxHighlighter::XmlSyntaxHighlighter(QObject* parent) : QSyntaxHighlighter(parent)
{

}

XmlSyntaxHighlighter::XmlSyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
{

}

XmlSyntaxHighlighter::XmlSyntaxHighlighter(QTextEdit* parent) : QSyntaxHighlighter(parent)
{

}
