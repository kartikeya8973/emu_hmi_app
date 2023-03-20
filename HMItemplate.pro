QT       += core gui multimedia multimediawidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS +=-lvlc -lgstreamer-1.0

INCLUDEPATH += /usr/include/gstreamer-1.0/ /usr/include/glib-2.0/ /usr/lib/x86_64-linux-gnu/glib-2.0/include/


SOURCES += \
    audioplayer.cpp \
    customle.cpp \
    devicewindow.cpp \
    etbarchivewindow.cpp \
    httpdownloader.cpp \
    infowindow.cpp \
    keyboarddialog.cpp \
    logindialog.cpp \
    logswindow.cpp \
    main.cpp \
    mainwindow.cpp \
    nvrwindow.cpp \
    pingthread.cpp \
    qjsontablemodel.cpp \
    renamewindow.cpp \
    screenshot.cpp \
    settingswindow.cpp \
    videoarchivewindow.cpp \
    videoplayer.cpp

HEADERS += \
    audioplayer.h \
    customle.h \
    devicewindow.h \
    etbarchivewindow.h \
    httpdownloader.h \
    infowindow.h \
    keyboarddialog.h \
    logindialog.h \
    logswindow.h \
    mainwindow.h \
    nvrwindow.h \
    pingthread.h \
    qjsontablemodel.h \
    renamewindow.h \
    screenshot.h \
    settingswindow.h \
    videoarchivewindow.h \
    videoplayer.h

FORMS += \
    audioplayer.ui \
    devicewindow.ui \
    etbarchivewindow.ui \
    infowindow.ui \
    keyboarddialog.ui \
    logindialog.ui \
    logswindow.ui \
    mainwindow.ui \
    nvrwindow.ui \
    renamewindow.ui \
    screenshot.ui \
    settingswindow.ui \
    videoarchivewindow.ui \
    videoplayer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

DISTFILES += \
    4-black-squares.png
