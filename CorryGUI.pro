QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    helper/corryconfigparser.cpp \
    src/geometrybuilder.cpp \
    helper/moduleconfiguration.cpp \
    helper/corrymodulemodel.cpp \
    src/moduleconfigurator.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/runmanager.cpp

HEADERS += \
    helper/corryconfigparser.h \
    src/geometrybuilder.h \
    helper/moduleconfiguration.h \
    helper/corrymodulemodel.h \
    src/moduleconfigurator.h \
    src/mainwindow.h \
    src/runmanager.h

FORMS += \
    ui/runmanager.ui \
    ui/geometrybuilder.ui \
    ui/moduleconfigurator.ui \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
