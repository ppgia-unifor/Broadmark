

#pragma once

// Fixed seed for RNGs
#define RANDOM_SEED 10

// Conditional Compilation for CGAL and GPU, as these are harder to setup
// Setting this to 'false' is handy if you have no intention on using these
// two solutions or if you are having a hard time compiling them
#define USE_CGAL true
#define USE_GPU true