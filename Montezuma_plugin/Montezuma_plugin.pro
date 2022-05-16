TEMPLATE = lib
TARGET = Montezuma
QT += qml quick
CONFIG += plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = plugins.montezuma

# Input
SOURCES += \
	montezuma_plugin.cpp \
        montezuma.cpp


HEADERS += \
	montezuma_plugin.h \
        montezuma.h

DISTFILES = qmldir

DESTDIR = plugins/montezuma

QMAKE_POST_LINK += $$quote(cp $$PWD/qmldir $$DESTDIR/qmldir; cp $$PWD/config.json config.json)

