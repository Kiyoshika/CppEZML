# CppEZML
A work in progress C++ machine learning library designed to be very easy to use. Everything pretty much written from scratch.

NOTICE: This library is still heavily in development and NOT recommended for production use (or even recreational use at this point).


# Latest Release
## v0.6.3 -- 20 November 2021
#### What's new in v0.6.3
* DecisionTree class was using make_unique instead of make_shared for shared pointers
* Fixed dropna() when inplace = true (was not doing anything)
* get_column_indices() is now a public member instead of private
* Fixed append() when columns were blank
* Added get_rows() to select multiple rows at once (extension of get_row())
* filter() now accepts lambdas that capture local variables
* filter() and select() can now modify DataSet inplace instead of returning only copies

# Latest Update (Dev Work)
## v0.6.4 -- 21 November 2021
* Converting entire project to CMake and changing structure (separating declaration from implementation)
* Going to start using Doxygen for documentation instead of writing it by hand

### What's being worked on now?
* Changing structure of project to separate declaration from implementation
* Converting project to CMake to create static libraries
* Writing Doxygen comments for the better method of documentation

Upcoming:
* Leave one out cross validation (LOOCV), K fold cross validation
* Custom metrics for cross validation

# What's the roadmap?
See "Milestones" under the "Issues" tab for a rough roadmap of the upcoming releases.

# Documentation
Documentation hosted on github pages: https://kiyoshika.github.io/CppEZML/
