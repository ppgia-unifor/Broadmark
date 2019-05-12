![Broadmark](/images/main-logo.png)

Our framework consists of a Unity-based scenario generation tool and a C++ environment to execute algorithms upon generated scenes.

Out-of-the-box, we currently implement the following:
- Scenario Generation Engines:
  - PhysX: built-in Unity physics engine
  - Bullet: through the Bullet Unity project
  - Viewer: used to view previously generated scenes. Useful for movie recordings
  - Splitter: views a n-objects simulation and outputs a 8n-objects simulation by splitting objects in 8 sub-objects

- Built-in Scenes:
  - Brownian: a dynamic and chaotic scene of randomly moving boxes. Each object is assigned a new random speed every now and then. Useful as baseline benchmark and analysis of algorithms under a uniform object distribution.
  - Free Fall: a coherent simulation of free falling boxes. Useful for testing temporal-optimized algorithms.
  - Gravity: a coherent simulation of free falling boxes under a strong and moving gravity. This is a stress test scenario which generates many collisions and forces all objects to move significantly within the world. 

- Implemented Algorithms:
  - Original Implementations: Bruteforce, Sweep-and-Prune and Grid. Several variants of these algorithms are implemented, such as SIMD optimized, Open-MP parallel and SIMD + OpenMP. For the Grid algorithm, we present it using a Grid + BF and Grid + SAP settings as well as using a 2D or 3D grid. Best variants are BF SIMD Parallel, SAP SIMD Parallel and Grid 3D SAP Parallel.
  - Bullet 2 Algorithms: We include the btAxisSweep3 and btDBVT algorithms. For convinience, these are included in the project and compiled with the remaining files for ease of integration and to reduce dependency troubles.
  - Bullet 3 Algorithms: We include a wrarper to use the b3OpenCL branch algorithms. These include a 3D Grid, a LBVH and a SAP implementation on OpenCL. For convinience, these are included in the project and can be compiled-out by altering the Config.h file. This is useful if you do not have a GPU compatible with OpenCL or does not have the necessary SDKs installed for compilation.
  - CGAL algorithms: We include the ["Intersecting Sequences of dD Iso oriented Boxes"](https://doc.cgal.org/latest/Box_intersection_d/index.html) algorithm. The necessary CGAL and Boost dependencies are not included in this project due to licensing and size. This algorithm can be compiled out by altering the Config.h file. A easy way to have all the dependencies set is to use the [Vcpkg](https://github.com/Microsoft/vcpkg) tool installed and download the cgal:x64-windows package.
  - Tracy's Algorithm: Original implementation of the [Efficient Large-Scale Sweep and Prune Methods with AABB Insertion and Removal](https://dl.acm.org/citation.cfm?id=1549865) paper. This used to be available at the [author's website](http://www.danieljosephtracy.com/) but it is no longer on-line.
  - KD-Tree Algorithm: Original implementation of the [Flexible Use of Temporal and Spatial Reasoning for Fast and Scalable CPU Broad‐Phase Collision Detection Using KD‐Trees](https://onlinelibrary.wiley.com/doi/full/10.1111/cgf.13529) algorithm. This is the exact same implementation as used in the paper.



## Runing the Code

The Simulation Generator tool was made using the Unity 2018.2 edition, but should probably work on any 2017+ version.

The Broadmark tool comes with Visual Studio 2017 solution files but can be run from VS Code without much trouble. The code was made and tested only on Windows machines, however, it is fairly SO agnostic and should not be difficult to run on Linux or MAC under Clang or maybe GCC. The threading portion is completely implemented as either STL threads or OpenMP and all dependencies are either bundled or can be compiled to other OSes.

Most of the system will work out-of-the-box, except for the CGAL and OpenCL algorithms. For the CGAL algorithm to work, you will have to compile the CGAL library yourself and link to the application. The easiest way to do this is to use the [Vcpkg](https://github.com/Microsoft/vcpkg) tool. For the OpenCL algorithms, you will need to download and install the OpenCL SDK of your graphics card vendor. Currently, we have only tested the application under NVidia Cards using the CUDA Toolkit 8 and 9. I would be glad to hear if the system worked fine on AMD or Intel cards as well.



