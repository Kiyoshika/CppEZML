# CppEZML
A work in progress C++ machine learning library designed to be very easy to use. Everything pretty much written from scratch.

# Latest Release
## v0.4.0 -- 19 June 2021
#### What's new in v0.4.0?
* Slight model optimizations
* Prettified the output of head()
* append(), rename(), describe(), split_data(), to_csv(), sample()
* Statistical functions: mean(), max(), min(), etc.

# Latest Update (Dev Work)
## v0.5.0.1 -- 29 August 2021
#### What's being worked on in v0.5.0.1?
Finished:
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

In Progress:
* Overhauling DataSet.h; moving away from vector<vector<...>>

Upcoming:
* Kernel regression
* K-Fold Cross Validation
* drop() method in DataSet
* And more...

# What's the roadmap?
See "Milestones" under the "Issues" tab for a rough roadmap of the upcoming releases.

# Documentation
I'm hosting my documentation here: https://zweaver.bit.ai/docs/view/WFbv5TFLT9NoJ6mp
