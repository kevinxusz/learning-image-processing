#-------------------------------------------------
#
# Project created by QtCreator 2012-03-10T00:20:53
#
#-------------------------------------------------

QT       += core gui

TARGET = LEARNING_IMAGE_PROCESSING
TEMPLATE = app


SOURCES += main.cpp\
    GUI/mainwindow.cpp \
    LOGIC/histogram.cpp \
    LOGIC/imagenpgm.cpp \
    LOGIC/imagenppm.cpp \
    CONTROLLER/maincontroller.cpp \
    DATA_ACCESS/imagefile.cpp \
    LOGIC/image.cpp \
    GUI/colordepthqwidget.cpp \
    GUI/thresholdqwidget.cpp \
    LOGIC/imagendcm.cpp \
    GUI/windowlevelqwidget.cpp \
    GUI/convolutionqwidget.cpp \
    GUI/customqlabel.cpp \
    GUI/sobelqwidget.cpp \
    LOGIC/filter.cpp \
    LOGIC/edgedetector.cpp \
    LOGIC/geometricoperation.cpp \
    LOGIC/globaltransformation.cpp \
    LOGIC/basic.cpp \
    LOGIC/segmentation.cpp \
    GUI/structureelementqwidget.cpp\
    GUI/cannywidget.cpp \
    GUI/changeframewidget.cpp


HEADERS  += \
    GUI/mainwindow.h \
    LOGIC/histogram.h \
    LOGIC/imagenpgm.h \
    LOGIC/imagenppm.h \
    CONTROLLER/maincontroller.h \
    DATA_ACCESS/imagefile.h \
    LOGIC/image.h \
    GUI/colordepthqwidget.h \
    GUI/thresholdqwidget.h \
    LOGIC/imagendcm.h \
    GUI/windowlevelqwidget.h \
    GUI/convolutionqwidget.h \
    GUI/customqlabel.h \
    GUI/sobelqwidget.h \
    LOGIC/filter.h \
    LOGIC/edgedetector.h \
    LOGIC/geometricoperation.h \
    LOGIC/globaltransformation.h \
    LOGIC/basic.h \
    LOGIC/segmentation.h \
    GUI/structureelementqwidget.h \
    GUI/cannywidget.h \
    GUI/changeframewidget.h

FORMS    += \
    GUI/mainwindow.ui \
    GUI/colordepthqwidget.ui \
    GUI/thresholdqwidget.ui \
    GUI/windowlevelqwidget.ui \
    GUI/convolutionqwidget.ui \
    GUI/sobelqwidget.ui \
    GUI/structuringElementQwidget.ui \
    GUI/cannywidget.ui \
    GUI/changeframewidget.ui
    

QMAKE_CXXFLAGS += -DHAVE_CONFIG_H

unix:!macx:!symbian: LIBS += -ldcmimgle
unix:!macx:!symbian: LIBS += -ldcmdata
unix:!macx:!symbian: LIBS += -loflog
unix:!macx:!symbian: LIBS += -lofstd

OTHER_FILES += \
    GUI/UNIVALLE.ppm

RESOURCES += \
    GUI/icons.qrc \
    files.qrc
