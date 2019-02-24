# -------------------------------------------------
# Project created by QtCreator 2010-01-22T20:32:11
# -------------------------------------------------
TARGET = MuGa
TEMPLATE = app
INCLUDEPATH += /usr/include/qwt
LIBS += -lqwt-qt4
INCLUDEPATH += /usr/include/qwtplot3d
unix:LIBS += -lqwtplot3d-qt5
QT += opengl
SOURCES += main.cpp \
    mainwindow.cpp \
    plot_thread.cpp
HEADERS += mainwindow.h \
    plot_thread.h
FORMS += mainwindow.ui
