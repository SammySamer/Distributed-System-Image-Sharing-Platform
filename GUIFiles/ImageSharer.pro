#-------------------------------------------------
#
# Project created by QtCreator 2019-11-19T12:46:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageSharer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG += console
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    login.cpp \
    signup.cpp \
    profile.cpp \
    allusers.cpp \
    notifications.cpp \
    myimages.cpp \
    editmyimages.cpp \
    allusers_images.cpp \
    sharedimages.cpp \
    viewimage.cpp

HEADERS += \
        mainwindow.h \
    login.h \
    signup.h \
    profile.h \
    allusers.h \
    peer.h \
    message.h \
    udpsocketclient.h \
    notifications.h \
    myimages.h \
    editmyimages.h \
    allusers_images.h \
    sharedimages.h \
    viewimage.h

FORMS += \
        mainwindow.ui \
    login.ui \
    signup.ui \
    profile.ui \
    allusers.ui \
    notifications.ui \
    myimages.ui \
    editmyimages.ui \
    allusers_images.ui \
    sharedimages.ui \
    viewimage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
