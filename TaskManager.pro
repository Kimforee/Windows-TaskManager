QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lPdh
LIBS += -lole32
LIBS += -loleaut32

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    TaskManager_en_IN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator' uiAccess='false'"
RESOURCES += \
    ../build-TaskManager-Desktop_Qt_6_5_1_MinGW_64_bit-Debug/debug/assests.qrc

