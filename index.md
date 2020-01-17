# Broadmark

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
  - Bullet 2 Algorithms: We include the btAxisSweep3 and btDBVT algorithms. For convenience, these are included in the project and compiled with the remaining files for ease of integration and to reduce dependency troubles.
  - Bullet 3 Algorithms: We include a wrapper to use the b3OpenCL branch algorithms. These include a 3D Grid, a LBVH and a SAP implementation on OpenCL. For convinience, these are included in the project and can be compiled-out by altering the Config.h file. This is useful if you do not have a GPU compatible with OpenCL or does not have the necessary SDKs installed for compilation.
  - CGAL algorithms: We include the ["Intersecting Sequences of dD Iso oriented Boxes"](https://doc.cgal.org/latest/Box_intersection_d/index.html) algorithm. The necessary CGAL and Boost dependencies are not included in this project due to licensing and size. This algorithm can be compiled out by altering the Config.h file. A easy way to have all the dependencies set is to use the [Vcpkg](https://github.com/Microsoft/vcpkg) tool installed and download the cgal:x64-windows package.
  - Tracy's Algorithm: Original implementation of the [Efficient Large-Scale Sweep and Prune Methods with AABB Insertion and Removal](https://dl.acm.org/citation.cfm?id=1549865) paper. This used to be available at the [author's website](http://www.danieljosephtracy.com/) but it is no longer on-line.
  - KD-Tree Algorithm: Original implementation of the [Flexible Use of Temporal and Spatial Reasoning for Fast and Scalable CPU Broad‐Phase Collision Detection Using KD‐Trees](https://onlinelibrary.wiley.com/doi/full/10.1111/cgf.13529) algorithm. This is the exact same implementation as used in the paper.



## Runing the Code

Requirements:
- Python 3 (numpy, pandas, openpyxl, matplotlib, seaborn)
- Unity 2019.2 (optional, other versions might work)
- MVC++ Compiler (e.g., Visual Studio Community)

The system consists of 3 modules: (1) a Unity-based simulation generator tool, used to bake rigid body sims to disk; (2) the broadmark C++ tool, which houses the algorithms and allows them to be benchmarked on a baked scene; and (3) a python 3 module to run tests and parse the results. Out-of-the-box, we provide pre-built binaries and some baked scenes to help getting the system up-and-running for the first time. 

First, download this repository (`git clone https://github.com/ppgia-unifor/Broadmark.git`) and install the lastest Python 3. To get all python dependencies, run `pip install numpy pandas, openpyxl matplotlib seaborn`. At the [releases page](https://github.com/ppgia-unifor/Broadmark/releases), download `SimulationGenerator_Build.exe` file (if you wish to generate the simulations yourself) or download the `FreeFall Cubes N1000 F1000frames.aabbs` pre-generated scene and skip the following paragraph. Then, download the `Broadmark_x64.exe`, which is the pre-built binary of the Broadmark system. In the following, no strict directory structure is needed to run the system, however, it is advisable to put everything on a common folder and to store simulations on a drive with plenty of space. 

To generate scenes yourself, extract the Simulation Generation tool and run it. You should see the following screen, which allows you to customize which scenes you wish to run, for how many objects and frames, and several other options. For now, keep all default values as is, but mark "Record Simulations" and set the "Output Path" to the folder you wish to save scenes. Now, click Generate Simulations and watch/wait until finished. Finally, check the output folder for the `.aabb` files.

Simulation Generator Wizard and its Settings.
![Simulation Generator Wizard](/images/simulation-generator-wizard.png)
Simulation Generator at work producing a Brownian scene.
![Simulation Generator in Action](/images/simulation-generator-brownian.png)
Generated .aabb files.
![Simulation Generator in Action](/images/simulation-generator-files.png)

Having the simulation files ready, we can start running some algorithms. There are two ways of running the Broadmark system, one is the manual way and the other is based on using the Python module to handle the low level work. The Broadmark tool takes as input a .json file which contains the algorithm it should run (and any parameters), the path to the baked scene file and the path where it should put the output json file. The manual execution consists of writing the .json file by hand (or writing a custom tool). Here is a sample .json file to be used with the Broadmark.exe:

```
{
    "m_algorithm": "KD",
    "m_algorithm_prettyName": "KD",
    "m_inputScene": "E:/Simulations/Recordings_AABBs/Brownian Cubes N1000 F1000frames.aabbs",
    "m_outputLog": "D:/Bitbucket/broadmark/Scripts/Tests/Tests 2020-01-17_13-28-56/Results/Brownian Cubes N1000 F1000frames.aabbs_KD.json_results.json",
    "NoAdditionalProperty": ""
}
```

To run the benchmark, replace the path strings to fit your directory structure and run `Broadmark.exe config.json`.

To run multiple algorithms and scenes, a `.json` file per algorithm-simulation pair is required. This ensures that every test is a separate execution context, sandboxing each run from potential memory leaks and/or crashes from other executions. To alleviate the amount of work needed, the Python module comes into play as a middleware to manage the .json generation. To use it, inspect the file `Scripts/run.py`. In it, fill-in the variables `broadmark_bin`, `scenes_folder` and `output_folder` with your directory structure and the `algorithms` variable with a list of algorithms you wish to run. Now, if you run `python run.py`, the batch execution will start, testing all supplied algorithms against all `.aabb` files in the `scenes_folder` path. At the end, some `.csv` files are generated, aggregating the outputs of every test. 

Sample of the Broadmark system performing several benchmark tests.
![Broadmark Execution](/images/broadmark-execution.png)
Outputted log files and .csv summaries.
![Generated Results](/images/broadmark-logs.png)

To generate plots of the obtained results, run `python plot.py`. This script will search for the lastest results (at the "Scripts/Tests/" folder) and present some useful plots of the generated data. To perform custom ploting, you can use the generated .csv files or parse the output .json files yourself, for maximum control. 

At this point, you should be able to generate scenes using the Simulation Generator tool and run benchmarks for the supplied algorithms on the generated scenes. 

From now on, for a more advanced usage, if you wish to develop your own scenes, you need to install the Unity Game Engine and open the Simulation Generator project, which is pretty much unity-esque. With some Unity experience it should not be hard to use, understand and modify. The same goes for the Broadmark tool. The repository contains the necessary Visual Studio project files and they should compile out-of-the-box without any issues. Right now, we only support Windows x64 builds, however, it is fairly SO agnostic and should not be difficult to run on Linux or MAC under Clang or maybe GCC. The threading portion is completely implemented as either STL threads or OpenMP and all dependencies are either bundled or can be compiled to other OSes. For the CGAL algorithm to work, you will have to compile the CGAL library yourself and link to the application. The easiest way to do this is to use the [Vcpkg](https://github.com/Microsoft/vcpkg) tool, which has the CGAL package. For the OpenCL algorithms, you will need to download and install the OpenCL SDK of your graphics card vendor. Currently, we have only tested the application under NVidia Cards using the CUDA Toolkit 8, 9 and 10. We would be glad to hear if the system worked fine on AMD or Intel cards as well. Regarding the python module, its implementation is pretty straight forward and should also be easy to understand and modify. Since the whole system is based on the .json format, it should not be hard to write any custom tools as well. 

Justifying some of the design choices, we opted for a Simulation Generator tool decoupled from the execution to make the system more scalable to massive simulations and to enforce that every test is conducted over the exact same data. Since simulations are baked, they are generated only once, despite the number of tests that will be conducted on them, and they are fixed, so each algorithm will receive the exact same set of AABBs. Regarding the Broadmark system being capable of handling only one pair of Algorithm-Scene per execution, this has the desirable properties of sandboxing each benchmark run and of allowing the test management code to live outside of the error-prone C++ world. While this makes using the system a bit harder, most of the necessary setup is performed only once and should not change over time, such as path strings. For more information, please consider reading our work [Broadmark: A Testing Framework for Broad‐Phase Collision Detection Algorithms](https://onlinelibrary.wiley.com/doi/abs/10.1111/cgf.13884), which throughly elaborates on these and other design choices. 

For any questions, issues and enhancements, please contact us by e-mail or open a github issue or pull request. We will be glad to answer and help in any inquiries. 


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


