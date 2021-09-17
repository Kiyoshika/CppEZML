# CppEZML
A work in progress C++ machine learning library designed to be very easy to use. Everything pretty much written from scratch.

NOTICE: This library is still heavily in development and NOT recommended for production use (or even recreational use at this point).

This library may become more stable around v0.8.0 or afterwards...

# Latest Release
## v0.4.0 -- 16 September 2021
#### What's new in v0.6.0 (& v0.5.0.1)?
v0.5.0.1
* Implemented KMeans clustering
* Implemented KNN classification
* Added transpose() in DataSet
* Can now load() 1D vectors in DataSet
* 1D vectors now supported in Stats class
* Kernel Density Estimation
* dropna(), replacena(), countna(), replace()
* Fixed bug in select() which dropped the last record
* Added Kernel Naive Bayes classifier
* Added Gaussian Naive Bayes classifier
* Added some static methods in DataSet to use select(), transpose() etc. with double-type 2D vectors
* Random Forest Classification (need to rewrite Decision Tree class better)

v0.6.0
* Overhauled filter(); Removed FilterData.h
* Overhauled DataSet.h (and added drop())
* Converted all headers from .h to .hpp
* Added many more code examples for reference
* Reworked all models to be compatable with DataSet overhaul
* Fixed memory leak in DecisionTree.hpp (and now using smart pointers)
* K-Fold Cross Validation

# Latest Update (Dev Work)
## v0.6.0 -- 16 September 2021
v0.5.0.1 is remaining unreleased due to the plan to overhaul DataSet.h, so it's being merged into v0.6.0 before release.

### What's being worked on in v0.6.0?
Nothing committed since release as of now.

Upcoming:
* Sorting and Joining on DataSet.hpp
* Leave one out cross validation (LOOCV)
* Custom metrics for cross validation

# What's the roadmap?
See "Milestones" under the "Issues" tab for a rough roadmap of the upcoming releases.

# Documentation
I will be reworking the documentation after the release of v0.6.0. Probably going to use GitHub pages.
