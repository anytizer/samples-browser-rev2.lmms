# This CMake file is for compiling this application as a Plugin for LMMS.
# The only difference is to exclude parent's main.cpp.

INCLUDE(BuildPlugin)

BUILD_PLUGIN(samples2
    Samples2.h
    Samples2View.h
    Samples2.cpp
    Samples2View.cpp

    CustomTableWidget.h
    WaveformData.h
    WaveformWidget.cpp
    WaveformWidget.h
MOCFILES
    Samples2View.h
    Samples2.h
    #CustomTableWidget.h
    WaveformWidget.h
EMBEDDED_RESOURCES
    *.png
)