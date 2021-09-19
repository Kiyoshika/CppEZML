# Changelog
Take a glimpse at the history of CppEZML and see what features came when.

## Index
* [v0.6.1](#v0.6.1-18-september-2021) - 18 September 2021
* [v0.6.0](#v0.6.0-16-september-2021) - 16 September 2021
* [v0.5.0](#v0.5.0-16-september-2021) - 16 September 2021
* [v0.4.0](#v0.4.0-19-june-2021) - 19 June 2021
* [v0.3.0](#v0.3.0-14-march-2021) - 14 March 2021
* [v0.2.0](#v0.2.0-07-march-2021) - 07 March 2021
* [v0.1.0](#v0.1.0-12-december-2020) - 12 December 2020
* [Pre-C++](#pre-c++-11-april-2020) - 11 April 2020

## v0.6.1 - 18 September 2021
* Bugs when loading 1D vectors into DataSet row-wise; was not using my own API properly due to copy-paste from old API implementation.
* Linear model run times were long due to being 100,000 iterations each. Reduced iterations and upped learning rate.
* k_fold_cv renamed to monte_carlo_cv since the implementation was not actually the K Fold method.
* k_fold_cv (now monte_carlo_cv ) was broken on certain models due to no default constructor. Now passing pointers of objects through function.
<br/><br/>

## v0.6.0 - 16 September 2021
* Overhauled filter(); Removed FilterData.h
* Overhauled DataSet.h (and added drop())
* Converted all headers from .h to .hpp
* Added many more code examples for reference
* Reworked all models to be compatable with DataSet overhaul
* Fixed memory leak in DecisionTree.hpp (and now using smart pointers)
* Monte Carlo Cross Validation
<br/><br/>

## v0.5.0 - 16 September 2021 (Packaged with v0.6.0)
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
<br/><br/>

## v0.4.0 - 19 June 2021
* Slight model optimizations
* Prettified the output of head()
* append(), rename(), describe(), split_data(), to_csv()
* Statistical functions: mean(), max(), min(), etc.
<br/><br/>

## v0.3.0 - 14 March 2021
* Finished implementing DecisionTree
* Created abstract classes for classifiers and regressors to help with organization
* Implemented logistic regression
* Added "on the fly" conversions on DataSet class such as data.cast_data_double(), data.cast_target_int(), etc.
* Added early stopping in linear/logistic regression to improve fit times
* Completely reworked filtering algorithm to support more sophisticated conditions (was reworked again in v0.6.0)
* Added f1 score to classifiers
* Now supporting multi-character delimiters (previously only supported one character)
* Users can now pass custom loss functions to regressors/classifiers
<br/><br/>

## v0.2.0 - 07 March 2021
* LinearRegression implemented (the first model of the library)
* DecisionTree (mostly) implemented
* Basic DataSet filtering (was reworked in v0.3.0)
<br/><br/>

## v0.1.0 - 12 December 2020
* First barebones iteration of DataSet.h
<br/><br/>

## Pre-C++ - 11 April 2020
* Implemented something similar to DataSet.h in Java, but decided to write the library in C++ instead
