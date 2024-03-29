# Programming Assignment 2 : Guided Scheduling

## What is this assignment about?
This assignment is to implement a guided scheduling for parallel reduction.
In guided scheduling, chunk size is dynamic while using guided method,
the size of a chunk is proportional to the number of unassigned iterations divided by the number of the threads,
and the size will be decreased to chunk-size (but the last chunk could be smaller than chunk-size)


## Repository structure
- src : source files
- CMakeLists.txt : cmake file
- 3rd-party : 3rd-party libraries
- cmake : cmake file for Google benchmark 
- PA2-report.pdf : report

## Build
To build the executable, please follow the instructions below. The default compiler is g++.
```
mkdir build
cd build
cmake ../
make
cmake ../
make
```
***You need to cmake ../ and make twice to find the installed google benchmark libraries.***

## Run
To run the executable, please follow the instructions below.

```
cd build
./main
```

## Experiment results
The report is available [[here](./PA2-report.pdf)]
