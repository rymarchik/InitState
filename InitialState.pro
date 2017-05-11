#-------------------------------------------------
#
# Project created by QtCreator 2016-12-15T15:48:33
#
#-------------------------------------------------

QT       += core gui sql network

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
    addparamform.cpp \
    commandsaddform.cpp \
    basetoolclass.cpp \
    battleOrderChangesBM.cpp \
    battleOrderChangesMBU.cpp \
    battleOrderChangesTZM.cpp \
    hitTargetsTabForm.cpp \
    addreciversform.cpp

HEADERS  += mainwindow.h \
    receiversDialog.h \
    hitTargets.h \
    utility.h \
    battleOrder.h \
    commands.h \
    positions.h \
    server.h \
    addparamform.h \
    commandsaddform.h \
    basetoolclass.h \
    battleOrderChangesBM.h \
    battleOrderChangesMBU.h \
    battleOrderChangesTZM.h \
    hitTargetsTabForm.h \
    addreciversform.h

RESOURCES += \
    resources.qrc

FORMS += \
    hittargets.ui \
    addparamform.ui \
    commandsaddform.ui \
    mainwindow.ui \
    battleOrderChangesBM.ui \
    battleOrderChangesMBU.ui \
    battleOrderChangesTZM.ui \
    addreciversform.ui
