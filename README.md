# Broadmark

A framework for the development, testing and evaluation of broad phase collision detection algorithms. Our framework consists of a Unity-based scenario generation tool and a C++ environment to execute algorithms upon generated scenes.

Out-of-the-box, we have currently implemented the following:
- Scenario Generation Engines:
  - PhysX: Built-in Unity physics engine
  - Bullet: Through the Bullet Unity project
  - Viewer: Used to view previously generated scenes. Useful for movie recordings
  - Splitter: Views n-objects simulation and outputs a 8n-objects simulation by splitting objects in 8 sub-objects

- Built-in Scenes:
  - Brownian: A dynamic and chaotic scene with randomly moving boxes. Each object is assigned with a new random speed every now and then. Useful as baseline benchmark and analysis of algorithms under a uniform object distribution.
  - Free Fall: A coherent simulation of free falling boxes. Useful for testing temporal-optimized algorithms.
  - Gravity: A coherent simulation of free falling boxes under a strong and moving gravity. This is a stress test scenario which generates many collisions and forces all objects to move significantly within the world. 

- Implemented Algorithms:
  - Original Implementations: Bruteforce, Sweep-and-Prune and Grid. Several variants of these algorithms are implemented, such as SIMD optimized, Open-MP parallel and SIMD + OpenMP. For the Grid algorithm, we present it using a Grid + BF and Grid + SAP settings as well as using a 2D or 3D grid. Best variants are BF SIMD Parallel, SAP SIMD Parallel and Grid 3D SAP Parallel.
  - Bullet 2 Algorithms: We include the btAxisSweep3 and btDBVT algorithms. For convenience, they are incorported into the project and compiled with the remaining files for ease of integration and to reduce dependency troubles.
  - Bullet 3 Algorithms: We include a wrapper to use the b3OpenCL branch algorithms. These contain also a 3D Grid, a LBVH and a SAP implementation on OpenCL. For convenience, they are incorporated into the project and can be compiled-out by altering the Config.h file. This is useful if you do not have a GPU compatible with OpenCL or the necessary SDKs installed for compilation.
  - CGAL algorithms: We include the ["Intersecting Sequences of dD Iso oriented Boxes"](https://doc.cgal.org/latest/Box_intersection_d/index.html) algorithm. The necessary CGAL and Boost dependencies are not incorporated into this project due to licensing and size. This algorithm can be compiled out by altering the Config.h file. An easy way to set up all the dependencies is to use the [Vcpkg](https://github.com/Microsoft/vcpkg) tool installed and download the cgal:x64-windows package.
  - Tracy's Algorithm: Original implementation of the [Efficient Large-Scale Sweep and Prune Methods with AABB Insertion and Removal](https://dl.acm.org/citation.cfm?id=1549865) paper. This used to be available at the [author's website](http://www.danieljosephtracy.com/) but it is no longer on-line.
  - KD-Tree Algorithm: Original implementation of the [Flexible Use of Temporal and Spatial Reasoning for Fast and Scalable CPU Broad‐Phase Collision Detection Using KD‐Trees](https://onlinelibrary.wiley.com/doi/full/10.1111/cgf.13529) algorithm. This is exactly the same implementation as used in the paper.



## Runing the Code

The Simulation Generator tool has been implemented using the Unity 2019.2 edition, but should probably work on any 2017+ version. For non-unity users, a pre-built version for Windows x64 is available at the 'releases' section of the GitHub repository.

The Broadmark tool comes with Visual Studio 2017 solution files but can be run from VS Code without much trouble. The code was originally written and tested only on Windows machines. However, it is fairly SO agnostic and should not be difficult to run on Linux or MAC under Clang or maybe GCC. The threading portion is completely implemented as either STL threads or OpenMP and all dependencies are either bundled or can be compiled to other OSes.

Most of the system will work out-of-the-box, except for the CGAL and OpenCL algorithms. For the CGAL algorithm to work, you will have to compile the CGAL library yourself and link to the application. The easiest way to do this is to use the [Vcpkg](https://github.com/Microsoft/vcpkg) tool. For the OpenCL algorithms, you will need to download and install the OpenCL SDK of your graphics card vendor. Currently, we have only tested the application under Nvidia Cards using the CUDA Toolkit 8 and 9. I would be glad to hear if the system worked fine on AMD or Intel cards as well.


## Citing our Work

A complete description of the Broadmark system can be found on the article [Broadmark: A Testing Framework for Broad‐Phase Collision Detection Algorithms](https://onlinelibrary.wiley.com/doi/abs/10.1111/cgf.13884)

To cite this work, please use the following bibtex entry:

```
@inproceedings{serpa2019broadmark,
  title={Broadmark: A Testing Framework for Broad-Phase Collision Detection Algorithms},
  author={Serpa, Ygor Rebou{\c{c}}as and Rodrigues, Maria Andr{\'e}ia Formico},
  booktitle={Computer Graphics Forum},
  year={2019},
  organization={Wiley Online Library}
}
```


