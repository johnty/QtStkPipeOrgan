#-------------------------------------------------
#
# Project created by QtCreator 2015-11-16T22:22:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtStkTest
TEMPLATE = app

#windows specific defines, including ASIO support
win32:LIBS += odbc32.lib odbccp32.lib winmm.lib Wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib
win32:DEFINES += __WINDOWS_ASIO__ __WINDOWS_MM__
win32:DEFINES -= UNICODE


#NOTE: for the dependencies below to work,
#this project folder should be in the stk_root/projects
INCLUDEPATH += ../../include/ ../../src/include/


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../src/include/asio.cpp \
    ../../src/include/asiodrivers.cpp \
    ../../src/include/asiolist.cpp \
    ../../src/include/iasiothiscallresolver.cpp \
    ../../src/Mutex.cpp \
    ../../src/RtWvOut.cpp \
    ../../src/SineWave.cpp \
    ../../src/Stk.cpp \
    ../../src/RtAudio.cpp \
    stkdac.cpp

HEADERS  += mainwindow.h \
    ../../src/include/asio.h \
    ../../src/include/asiodrivers.h \
    ../../src/include/asiodrvr.h \
    ../../src/include/asiolist.h \
    ../../src/include/asiosys.h \
    ../../src/include/dsound.h \
    ../../src/include/FunctionDiscoveryKeys_devpkey.h \
    ../../src/include/ginclude.h \
    ../../src/include/iasiodrv.h \
    ../../src/include/iasiothiscallresolver.h \
    ../../src/include/soundcard.h \
    ../../include/Mutex.h \
    ../../include/RtMidi.h \
    ../../include/RtWvOut.h \
    ../../include/Stk.h \
    ../../include/SineWave.h \
    stkdac.h

FORMS    += mainwindow.ui
