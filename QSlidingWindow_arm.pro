#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T16:30:31
#
#-------------------------------------------------

QT       -= gui

TARGET = QSlidingWindow
TEMPLATE = lib

DEFINES += QSLIDINGWINDOW_LIBRARY

SOURCES += qslidingwindow.cpp

HEADERS += qslidingwindow.h\
        qslidingwindow_global.h

INCLUDEPATH += ../QSlidingWindowConsume

#ubuntu
#unix:!macx{
#LIBS += -L /usr/local/linux_lib/lib   -l QSlidingWindowConsume
#}
#arm
unix:!macx{
LIBS += -L /usr/local/arm_lib/   -l QSlidingWindowConsume
}
#mac
unix:macx{
LIBS += -L /usr/local/lib   -l QSlidingWindowConsume
}


#install
#ubuntu
#unix:!macx {
#    target.path = /usr/local/linux_lib/lib
#    INSTALLS += target
#}
#arm
unix:!macx {
    target.path = /usr/local/arm_lib
    INSTALLS += target
}
#mac
unix:macx {
    target.path = /usr/local/lib
    INSTALLS += target
}
