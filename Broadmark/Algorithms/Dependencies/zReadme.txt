

This folder contains all dependencies that are small enough to be inserted directly into the project
All dependencies are Apache/MIT licensed and carry their license/header within

The "Bullet 2" library is a general purpose physics and collision detection library. We bundle a subset
of it containing the necessary files for the 'btAxisSweep3' and 'btDBVT' broad phase algorithms.
The "Bullet 3" is also included for the b3OpenCL algorithms. The source files have been slightly altered
to include the conditional compilation '#if USE_GPU'
https://github.com/bulletphysics/bullet3

The "Tracy" library has the source code of Tracy et al. algorithm. The source code was available at the
following link, however, it seems that the page is no longer being hosted :c
https://www.sweepandprune.com

The "ctpl.h" library is a header-only thread pool library and it is used for the Tracy Parallel algorithm
https://github.com/vit-vit/CTPL

The "doctest.h" library is a header-only unit test library and it is used throughout the project for unit
testing and for assertion macros with automatic expansions.
https://github.com/onqtam/doctest/

The "json.hpp" is a header-only json library for C++ that attempts to portray json objects as first-class
objects for the C++ language. It makes heavy use of C++ 11/14/17 features. It is used in this project to
serialize and desserialize 'settings.h' and 'results.h'
https://github.com/nlohmann/json