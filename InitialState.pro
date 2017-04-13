#-------------------------------------------------
#
# Project created by QtCreator 2016-12-15T15:48:33
#
#-------------------------------------------------

QT       += core gui sql network uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InitialState
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    receiversDialog.cpp \
    hitTargets.cpp \
    utility.cpp \
    battleOrder.cpp \
    commands.cpp \
    positions.cpp \
    server.cpp \
    ownforcesform.cpp \
    addparamform.cpp \
    commandsaddform.cpp \
    basetoolclass.cpp \
    addreciversform.cpp

HEADERS  += mainwindow.h \
    receiversDialog.h \
    hitTargets.h \
    utility.h \
    battleOrder.h \
    commands.h \
    positions.h \
    server.h \
    ownforcesform.h \
    addparamform.h \
    commandsaddform.h \
    basetoolclass.h \
    addreciversform.h

RESOURCES += \
    resources.qrc

FORMS += \
    mainwindow.ui \
    hittargets.ui \
    ownforcesform.ui \
    mainwindow2.ui \
    addparamform.ui \
    commandsaddform.ui \
    addreciversform.ui
