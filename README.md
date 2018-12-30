## tychoBCG3D

Welcome to tychoBCG3D (TYCHO Boundary Conditions Generator for 3D STL- and point data (e.g. 3D scans), which you can download here. 
It is written in C++ with the Qt framework and the libQGLViewer library

The present version V0.1 is the first release.

Aim of this application is the generation of 3D boundary conditions for TYCHO from STL- and point data (e.g. 3D scans). You are welcome to download it and do whatever you want with it. Keep in mind that this code does not come with any guarantee.

To compile the sources you need a C++ compiler (e.g. http://gcc.gnu.org/), the QT library including the qt-devel packages and the libQGLViewer library. Note that QT is available under the open source LGPL version 2.1 license, from the release of Qt 4.5 onward (read more).

![tycho_BCG3D](https://github.com/kapferer/tychoBCG3D/blob/master/appscreen.jpg "tycho_BCGG3D")

To generate tychoBCG3D you need to execute the following steps:

qmake-qt4
make

 # A typical workflow could be

1.) open STL data or point data (plain text file with x,y,z and normal vector x,y,z in one datarow)

2.) set the relative size of the computational domain (the physical diameters are then set within a TYCHO simulation); note that tychoBCG3D scales all models to 1

3.) set the resolution

4.) save the boundary for TYCHO

4.a) you can check the sampled data of the boundaries by choosing "sampled data" in the dropdown box in the right part of the MainApp

5.) start tychoGUI and use this boundary in your TYCHO 3D-simulation

