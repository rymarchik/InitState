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
    hitTargetsTabForm.cpp

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
    hitTargetsTabForm.h

RESOURCES += \
    resources.qrc

FORMS += \
    hittargets.ui \
    ownforcesform.ui \
    addparamform.ui \
    commandsaddform.ui \
    mainwindow.ui
