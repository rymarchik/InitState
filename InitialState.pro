#-------------------------------------------------
#
# Project created by QtCreator 2016-12-15T15:48:33
#
#-------------------------------------------------

QT       += core gui sql network serialport

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
    addparamform.cpp \
    commandsaddform.cpp \
    basetoolclass.cpp \
    battleOrderChangesBM.cpp \
    battleOrderChangesMBU.cpp \
    battleOrderChangesTZM.cpp \
    hitTargetsTabForm.cpp \
    addreciversform.cpp \
    battleOrderDialogBM.cpp \
    battleOrderDialog.cpp \
    commandsmessagebox.cpp \
    datatransmissionmodule.cpp \
    converter.cpp \
    docmessagebox.cpp \
    mapsrc/networkmodule.cpp \
    mapsrc/NetworkObject.cpp \
    mapsrc/NetworkObjectManager.cpp \
    gpsModule.cpp \
    northSearchUnit.cpp \
    mapCommands.cpp

HEADERS  += mainwindow.h \
    receiversDialog.h \
    hitTargets.h \
    utility.h \
    battleOrder.h \
    commands.h \
    positions.h \
    addparamform.h \
    commandsaddform.h \
    basetoolclass.h \
    battleOrderChangesBM.h \
    battleOrderChangesMBU.h \
    battleOrderChangesTZM.h \
    hitTargetsTabForm.h \
    addreciversform.h \
    battleOrderDialogBM.h \
    battleOrderDialog.h \
    commandsmessagebox.h \
    datatransmissionmodule.h \
    converter.h \
    docmessagebox.h \
    mapsrc/networkmodule.h \
    mapsrc/NetworkObject.h \
    mapsrc/NetworkObjectManager.h \
    mapsrc/PropertyObj.h \
    gpsModule.h \
    northSearchUnit.h \
    mapCommands.h

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
    addreciversform.ui \
    battleOrderDialogBM.ui \
    battleOrderDialog.ui
