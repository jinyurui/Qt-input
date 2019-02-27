#-------------------------------------------------
#
# Project created by QtCreator 2013-04-04T23:11:38
#
#-------------------------------------------------

QT       += gui-private widgets

CONFIG += plugin

win32:CONFIG(release, debug|release): TARGET = Qt5InputpluginSoftKeyBoard
else:win32:CONFIG(debug, debug|release): TARGET = Qt5InputpluginSoftKeyBoard_d

#TARGET = Qt5InputpluginSoftKeyBoard
#Qt5SoftKeyboard
TEMPLATE = lib

DEFINES += VIRTUALKEYBOARD_LIBRARY

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/pinyin/ -lgooglepinyin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/pinyin/ -lgooglepinyin_d

INCLUDEPATH += $$PWD/pinyin
DEPENDPATH += $$PWD/pinyin

SOURCES += \
    inputcontext.cpp \
    platforminputcontextplugin.cpp \
    googlepinyin.cpp \
    keyboardform.cpp \
    keyeventdispatcher.cpp


HEADERS +=\
    pinyinime.h \
    inputcontext.h \
    platforminputcontextplugin.h \
    virtualkeyboard_global.h \
    googlepinyin.h \
    keyboardform.h \
    keyeventdispatcher.h

RESOURCES += \
    res.qrc

DISTFILES += \
    Qt5Input.json

