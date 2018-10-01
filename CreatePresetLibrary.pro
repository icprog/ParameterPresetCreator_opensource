#-------------------------------------------------
#
# Project created by QtCreator 2017-12-25T16:04:53
#
#-------------------------------------------------

QT       += core gui    \
            svg         \
            xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PresetCreator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS\
           APP_TARGET=\\\"$$TARGET\\\"
TRANSLATIONS += translate/\"$$TARGET\"_ja.ts \
                translate/\"$$TARGET\"_en.ts

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += src/main.cpp\
        src/ui/dialog/MainWindow.cpp\
        src/common/paramData.cpp\
        src/listView/listedDataCore.cpp\
        src/listView/materialListedData.cpp\
        src/ui/widget/customPushButton.cpp \
        src/ui/widget/customTreeWidget.cpp \
        src/ui/dialog/addDataDialog.cpp


HEADERS  += src/ui/dialog/MainWindow.h\
        src/common/common.h\
        src/common/paramData.h\
        src/listView/listedDataCore.h\
        src/listView/materialListedData.h\
        src/ui/widget/customPushButton.h \
        src/ui/widget/customTreeWidget.h \
        src/ui/dialog/addDataDialog.h

FORMS    += src/ui/dialog/MainWindow.ui \
        src/ui/dialog/addDataDialog.ui

INCLUDEPATH += src \
        src/ui/dialog \
        src/ui/widget

RC_ICONS = app.ico

DISTFILES += fabool_fomat.astylerc

RESOURCES += \
    rc.qrc
