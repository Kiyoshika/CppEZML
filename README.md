# CppEZML
A work in progress C++ machine learning library designed to be very easy to use. Everything written from scratch.

# Release Notes
### Latest Release: v0.2.0 -- 07 March 2021
What's new in v0.2.0:
* DecisionTree classifier (still in progress but functional).

# Downloading / Using
This is a pure header library, meaning you can git clone this repo wherever desired and start using.
```c++
// after git clone
#include "CppEZML/data/DataSet.h"
#include <iostream>

int main() {
  DataSet mydata;
  mydata.load("test.csv");
}
```
# Documentation
I'm writing documentation on bit ai here: https://zweaver.bit.ai/docs/view/WFbv5TFLT9NoJ6mp
