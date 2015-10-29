# OpenniSampleTaker

Allows to take sample from a 3D sensor and display. It can also make the
average of the samples and display it.

## Dependencies

* CMake 2.8
* Openni 2
* SDL 1.2

## Build

This software was developed and tested only on Linux (or GNU/Linux...). The
following instructions are for Linux.

No brainer instructions:

    git clone https://github.com/Alex-Canales/OpenniSampleTaker.git
    cd OpenniSampleTaker
    mkdir build
    cd build
    cmake ..
    make
    ./sampleTaker

Now you have the executable ``sampleTaker`` in in the directory ``build``.

The software was tested with the Structure Sensor.

## Note

If you came here by accident: good luck to understand why this program would be
useful (or even to understand what the program does).
