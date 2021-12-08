QT += core gui concurrent network qml quick
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 qml_debug

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    networktools.cpp \
    spotifysongsarray.cpp \
    startpage.cpp

HEADERS += \
    mainwindow.h \
    networktools.h \
    spotifysongsarray.h \
    startpage.h

FORMS += \
    mainwindow.ui \
    startpage.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    NetworkTools.js \
    Resources.qrc \
    dummy.qml \
    qml.qrc
