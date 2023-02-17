# Programming Assignment 1 : Threadpool

## What is this assignment about?
This assignment is to improve the execution time performance of parallel matrix multiplication (C=A*B)
using threadpool implementations.

## Repository structure
- src : source files
- CMakeLists.txt : cmake file

## Build
To build the executable, please follow the instructions below. The default compiler is g++.
```
mkdir build
cd build
cmake ../
make
```

## Run
To run the executable, please follow the instructions below.

```
cd build
./main M K N T
```
M, K, and N are used to describe the matrices.
Matrix A is of dimension N*K, B is of K*M, and C is of N*M.
T is the number of threads used. 

## Experiment results
