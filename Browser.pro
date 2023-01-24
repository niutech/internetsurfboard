#-------------------------------------------------
#
# Project created by QtCreator 2010-10-26T14:42:04
#
#-------------------------------------------------

QT       += core gui webkit network phonon multimedia svg opengl

TARGET = Browser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    webview.cpp \
    webpage.cpp \
    lineedit.cpp \
    tabbar.cpp \
    bookmarks.cpp \
    networkaccessmanager.cpp \
    styleloader.cpp \
    menu.cpp \
    trafficlog.cpp \
    javascriptconsole.cpp \
    sourceview.cpp \
    browserapplication.cpp \
    desktopwindowmanager.cpp \
    downloadmanager.cpp \
    settings.cpp \
    settingsdialog.cpp \
    gripwidget.cpp \
    pushbutton.cpp \
    urlshortcutresolver.cpp

HEADERS  += mainwindow.h \
    webview.h \
    webpage.h \
    lineedit.h \
    tabbar.h \
    bookmarks.h \
    networkaccessmanager.h \
    styleloader.h \
    menu.h \
    trafficlog.h \
    javascriptconsole.h \
    sourceview.h \
    browserapplication.h \
    desktopwindowmanager.h \
    downloadmanager.h \
    settings.h \
    settingsdialog.h \
    gripwidget.h \
    pushbutton.h \
    urlshortcutresolver.h

RESOURCES += \
    rs.qrc

FORMS += \
    trafficlog.ui \
    javascriptconsole.ui \
    sourceview.ui \
    downloadmanager.ui \
    settingsdialog.ui

win32:RC_FILE = browser.rc

CONFIG += static
