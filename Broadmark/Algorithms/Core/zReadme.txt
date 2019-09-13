


The "Core" folder has all auxiliary/utils classes, namely:
	Mathematical Objects (Math folder)
	Data Structures (Structures folder)

	Assetions, doctest glue code to use assetions outside of test cases
	Clock, a basic windows-only high-resolution timer
	Params, a storage object to send config data to algorithms
	Results, a storage object to receive collected data from algorithms


In general, all classes within the "Core" folder are not limited to broad phase collision detection or
to benchmarking in general. 
Here, we do not aim at providing a complete set of math objects, data structures or other related utilities, 
this project is not intended to be a full BLAS library or Container library. This also means that we did not
seek to extensively optimize any of these classes or that they are meant for general use.
On the other hand, this also does not mean they were badly-written or intentionally crippled.
As guidance, consider that all classes written within this folder and subfolders were written as needed and
possess only the relevant functionality for the project.

As a side note, we sought to provide only basic interfaces for each class, so that they can be changed later
if needed. The most important example is the Vec3 class. We did not implement calls such as the dot product
or cross product because, in this project, Vec3 objects are mainly treated as points, not vectors.