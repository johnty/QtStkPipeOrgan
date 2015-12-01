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
win32:DEFINES += __WINDOWS_ASIO__ __WINDOWS_MM__ __OS_WIN32__
win32:DEFINES -= UNICODE
win32 {
SOURCES += ../../src/include/asio.cpp \
    ../../src/include/asiodrivers.cpp \
    ../../src/include/asiolist.cpp \
    ../../src/include/iasiothiscallresolver.cpp
HEADERS +=     ../../src/include/asio.h \
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
}

#mac/unix specific defines

macx:DEFINES += __MACOSX_CORE__
macx:LIBS += -framework CoreAudio
macx:LIBS += -framework CoreMIDI
macx:LIBS += -framework CoreFoundation

unix:!macx:DEFINES += __OS_LINUX__ __LINUX_ALSA__
unix:!macx:LIBS += -lpthread -lasound

#NOTE: for the dependencies below to work,
#this project folder should be in the stk_root/projects
INCLUDEPATH += ../../include/ ../../src/include/


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../src/ADSR.cpp \
    ../../src/BiQuad.cpp \
    ../../src/BlowBotl.cpp \
    ../../src/DelayL.cpp \
    ../../src/Flute.cpp \
    ../../src/Mutex.cpp \
    ../../src/Noise.cpp \
    ../../src/OnePole.cpp \
    ../../src/PoleZero.cpp \
    ../../src/RtWvOut.cpp \
    ../../src/SineWave.cpp \
    ../../src/Stk.cpp \
    ../../src/RtAudio.cpp \
    ../../src/RtMidi.cpp \
    ../../src/Voicer.cpp \
    OrganSynth.cpp \
    OrganFlue.cpp \
    OrganRank.cpp

HEADERS  += mainwindow.h \
    ../../include/ADSR.h \
    ../../include/BiQuad.h \
    ../../include/BlowBtl.h \
    ../../include/DelayL.h \
    ../../include/Flute.h \
    ../../include/Instrmnt.h \
    ../../include/Mutex.h \
    ../../include/Noise.h \
    ../../include/PoleZero.h \
    ../../include/OnePole.h \
    ../../include/RtMidi.h \
    ../../include/RtWvOut.h \
    ../../include/Stk.h \
    ../../include/SineWave.h \
    ../../include/Voicer.h \    
    OrganSynth.h \
    OrganFlue.h \
    OrganRank.h

FORMS    += mainwindow.ui
