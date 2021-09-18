# DataSet.hpp
**Introduced in:** v0.1.0

**Last Updated in:** v0.6.0
## Short Description
DataSet is a class that mimicks a matrix by using the classic 1D vector trick so that it's still contiguous.

It's a templated class to hold generic types of std::string, double and int (including size_t, uint64_t, etc.)

This class is **statically allocated**, so none of the methods support dynamically adding rows/columns unless desired in the future.

A sample constructor call is:

```c++
#include "CppEZML/data/DataSet.hpp"
#include <iostream>

int main()
{
  // create empty dataset with 2 rows and 3 columns
  // note that when creating datasets via constructor like this, columns are not defined
  // (this default behavior may change in a future version)
  DataSet<double> mydata(2, 3);
  
  // for numerical types, default values will be zero
  // for std::string, default values will be empty string (i.e "null" value)
  std::cout << mydata(0, 0) << "\n";
  return 0;
}
```
## Constructor Definition
This is a snippet from v0.6.0 of DataSet.hpp
```c++
DataSet() {}

// load data set when passing filename
DataSet(std::string filepath, std::string sep = ",", bool has_headers = true)
{
    this->load(filepath, sep, has_headers);
}

DataSet(size_t x, size_t y) {
    data.resize(x*y);
    rows = x;
    columns = y;
}
```
## Index
1. Loading Data
    1. [load()](loading-from-csv-file) - load from csv file
    2. [load()]() - load from std::vector
2. Data Attributes
    1. [count_rows()]() - count total rows
    2. [count_columns()]() - count total columns
3. Displaying Data
    1. [head()]() - print first N rows
4. Accessing/Modifying Data
    1. [resize()]() - resize rows and columns
    1. [(x, y)]() operator - access one cell from data set
    2. [get_row()]() - return a certain row as a vector
    3. [get_column()]() - return a certain column as a vector
    4. [set_row()]() - overwrite row index with a vector
    5. [set_column()]() - overwrite column index with a vector
    6. [set_column_names()]() - overwrite column names with std::string vector
6. Operations
    1. [transpose()]() - transpose a dataset
    2. [append()]() - merge two datasets row-wise or column-wise
    3. [select()]() - subset a dataset to certain columns
    4. [drop()]() - inverse of select(); subset a dataset by dropping certain columns
    5. [filter()]() - filter a dataset by a user-defined function
    6. [replace()]() - replace occurences of a value with another
    7. [cast()]() - cast contents of dataset from one datatype to another
    8. [rename()]() - rename columns via map
    9. [sample()]() - randomly sample from dataset with or without replacement
    10. [split_data()]() - split data into two portions, typically for creating train/test splits
7. Missing Values
    1. [countna()]() - count null columns and print to console
    2. [countna_vector()]() - count null columns and return a vector where each index represents a column
    3. [dropna()]() - drop rows from dataset containing null values
    4. [replacena()]() - replace null values with another value

## Loading From CSV File
#### Definition and Parameters
```c++
void load(std::string filepath, std::string sep = ",", bool has_headers = true)
```
* `std::string filepath`{:.c++} - A string indicating the path to the CSV file you want to load.
* `std::string sep = ","`{:.c++} - A string indicating the separator in the CSV file. (Default is ",")
* `bool has_headers = true`{:.c++} - A boolean indicating if the file has headers (column names). (Default is true)
#### Examples
```c++
#include "CppEZML/data/DataSet.hpp"

int main()
{
    // you can load data from constructor
    DataSet<std::string> mydata_constr("example_data.csv");
    // you can also pass custom delimiter and whether or not to read first row as headers (default = true)
    DataSet<std::string> mydata_constr2("example_data.csv", "|", false);

    // you can also explicitly load data using load() method (with same arguments as above)

    // load data as string
    DataSet<std::string> mydata_string;
    mydata_string.load("example_data.csv");
    mydata_string.head();

    // load data as doubles
    DataSet<double> mydata_double;
    mydata_double.load("example_data.csv");
    mydata_double.head();

    // load data as integers
    DataSet<int> mydata_int;
    mydata_int.load("example_data.csv");
    mydata_int.head();

    // load data with custom separator (|) and without headers
    DataSet<std::string> mydata_other;
    // parameters: filename, sep = ",", headers = true
    mydata_other.load("example_data.csv", "|", false);

    return 0;
}
```
