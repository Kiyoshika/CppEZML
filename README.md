# CppEZML
A work in progress C++ machine learning library designed to be very easy to use. Everything pretty much written from scratch.

NOTICE: This library is still heavily in development and NOT recommended for production use (or even recreational use at this point).

This library may become more stable around v0.8.0 or afterwards...

# Latest Release
## v0.6.0 -- 16 September 2021
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
* Monte Carlo cross validation

# Latest Update (Dev Work)
## v0.6.3 -- 20 November 2021
v0.6.3 Progress:
* DecisionTree class was using make_unique instead of make_shared for shared pointers
* dropna() was not working correctly if inplace = true
* get_column_indices() is now a public member instead of private

### What's being worked on now?
* Currently writing documentation following v0.6.0 release.
* Performing hotfixes and general bug fixes

Upcoming:
* Allowing local variables in filter()
* Leave one out cross validation (LOOCV), K fold cross validation
* Custom metrics for cross validation

# What's the roadmap?
See "Milestones" under the "Issues" tab for a rough roadmap of the upcoming releases.

# Documentation
Documentation hosted on github pages: https://kiyoshika.github.io/CppEZML/
