# DataSet.hpp
**Introduced in:** v0.1.0

**Last Updated in:** v0.6.0
## Header
```c++
#include "CppEZML/data/DataSet.hpp"
```
## Short Description
DataSet is a class that mimicks a matrix by using the classic 1D vector trick so that it's still contiguous.

It's a templated class to hold generic types of std::string, double and int (including size_t, uint64_t, etc.)

This class is **statically allocated**, so none of the methods support dynamically adding rows/columns unless desired in the future.

## Constructor Definition
**Overload 1**: \
Default constructor; doesn't serve much purpose, but it's there.
```c++
DataSet() // the default constructor
```
**Overload 2**: \
Constructor to load data from CSV file (equivalent to declaring the object and calling `load()`.)
```c++
DataSet(
  std::string filepath,   // A string indicating the path to the CSV file you want to load.
  std::string sep = ",",  // A string indicating the separator in the CSV file.
  bool has_headers = true // A boolean indicating if the file has headers (column names.)
)
```
**Overload 3**: \
Constructor to resize the data with default values (equivalent to declaring the object and calling `resize()`.) \
The default values for numerical types will be 0 and blank ("null") for std::string.
```c++
DataSet(
  size_t x, // how many rows you desire
  size_t y  // how many columns you desire
)
```
## Index
Note: All examples are written in the scope of main() to shorten the code

1. Loading Data
    1. [load()](#loading-from-csv-file) - load from csv file
    2. [load()](#loading-from-vector) - load from std::vector
2. Data Attributes
    1. [count_rows()](#count-rows) - count total rows
    2. [count_columns()](#count-columns) - count total columns
3. Displaying Data
    1. [head()](#printing-first-n-rows) - print first N rows
4. Accessing/Modifying Data
    1. [resize()](#resizing-data) - resize rows and columns
    2. [(x, y) operator](#accessing-cell) - access one cell from dataset
    3. [set()](#setting-cell) - set value of a cell in dataset
    4. [get_row()](#get-row) - return a certain row as a vector
    5. [get_column()](#get-column) - return a certain column as a vector
    6. [set_row()](#set-row) - overwrite row index with a vector
    7. [set_column()](#set-column) - overwrite column index with a vector
    8. [set_column_names()](#set-column-names) - overwrite column names with std::string vector
    9. [rename()](#rename-columns) - rename a set of columns using a map
6. Operations
    1. [transpose()](#transpose-data) - transpose a dataset
    2. [append()]() - merge two datasets row-wise or column-wise
    3. [select()]() - subset a dataset to certain columns
    4. [drop()]() - inverse of select(); subset a dataset by dropping certain columns
    5. [filter()]() - filter a dataset by a user-defined function
    6. [replace()]() - replace occurences of a value with another
    7. [cast()]() - cast contents of dataset from one datatype to another
    8. [sample()]() - randomly sample from dataset with or without replacement
    9. [split_data()]() - split data into two portions, typically for creating train/test splits
7. Missing Values
    1. [countna()]() - count null columns and print to console
    2. [countna_vector()]() - count null columns and return a vector where each index represents a column
    3. [dropna()]() - drop rows from dataset containing null values
    4. [replacena()]() - replace null values with another value

## Loading From CSV File
#### Definition and Parameters
```c++
void load(
  std::string filepath,   // A string indicating the path to the CSV file you want to load.
  std::string sep = ",",  // A string indicating the separator in the CSV file.
  bool has_headers = true // A boolean indicating if the file has headers (column names.)
)
```
#### Examples
```c++
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
```
## Loading From Vector
#### Definition and Parameters

**Overload 1**: \
This will load a 1D vector row-wise (as you provide a vector of column names.) \
The dimension of the DataSet will be `(1, c)` where c is the size of columns.
```c++
void load(
  std::vector<T> const& data,             // A vector of generic type (std::string, int, double, size_t, etc.)
  std::vector<std::string> const& columns // A vector of column names
)
```
**Overload 2**: \
This will load a 1D vector column-wise (as you provide a single column name.) \
The dimension of the DataSet will be `(r, 1)` where r is the size of data.
```c++
void load(
  std::vector<T> const& data, // A vector of generic type (std::string, int, double, size_t, etc.)
  std::string column_name     // A string providing the column name
)
```
#### Examples
```c++
DataSet<double> mydata;

std::vector<double> vector_data = { 1.0, 2.0, 3.0 };
std::vector<std::string> column_names = { "col1", "col2", "col3" };

// loads data row-wise
mydata.load(vector_data, column_names);
mydata.head();

// loads data column_wise
mydata.load(vector_data, "single_column");
mydata.head();
```
## Count Rows
#### Definition and Parameters
```c++
size_t count_rows()
```
#### Examples
```c++
DataSet<double> sample_data("example_file.csv");
std::cout << sample_data.count_rows() << "\n";
```
## Count Columns
#### Definition and Parameters
```c++
size_t count_columns()
```
#### Examples
```c++
DataSet<double> sample_data("example_file.csv");
std::cout << sample_data.count_columns() << "\n";
```
## Printing First N Rows
#### Definition and Parameters
```c++
void head(
  size_t rows = 10 // the number of rows to print to console
)
```
#### Examples
```c++
DataSet<double> sample_data("example_file.csv");
sample_data.head();
```
## Resizing Data
#### Definition and Parameters
```c++
void resize(
  size_t x, // how many rows you desire
  size_t y  // how many columns you desire
)
```
#### Examples
```c++
DataSet<double> sample_data;

// resize data for 2 rows and 3 columns
// by default, the cells will be 0.0
sample_data.resize(2, 3);
```
## Accessing Cell
#### Definition and Parameters
```c++
T &operator()(
  size_t x, // row index
  size_t y  // column index
)
```
#### Examples
```c++
DataSet<double> mydata(2, 2);
// accesses the cell at 0th row and 1st column
std::cout << mydata(0, 1) << "\n";
```
## Setting Cell
#### Definition and Parameters
```c++
void set(
  size_t x, // row index
  size_t y, // column index
  T value   // value you want to set the cell to
)
```
#### Examples
```c++
DataSet<double> mydata(2, 2);
mydata.set(0, 1, 5.5);
```
## Get Row
#### Definition and Parameters
```c++
std::vector<T> get_row(
  size_t x // row index
)
```
#### Examples
```c++
DataSet<double> mydata(2, 3);
std::vector<double> extract = mydata.get_row(0); // {0.0, 0.0, 0.0}
```
## Get Column
#### Definition and Parameters
```c++
std::vector<T> get_column(
  size_t y // column index
)
```
#### Examples
```c++
DataSet<double> mydata(2, 3);
std::vector<double> extract = mydata.get_column(2); // {0.0, 0.0}
```
## Set Row
#### Definition and Parameters
```c++
void set_row(
  size_t x,                      // row index
  std::vector<T> const& row_data // a vector to overwrite the current row
)
```
#### Examples
```c++
DataSet<double> mydata(2, 3);
std::vector<double> new_data = { 1.2, 4.5, 2.2 };
mydata.set_row(0, new_data);
```
## Set Column
#### Definition and Parameters
```c++
void set_column(
  size_t y,                         // column index
  std::vector<T> const& column_data // a vector to overwrite the current column
)
```
#### Examples
```c++
DataSet<double> mydata(2, 3);
std::vector<double> new_data = { 2.5, 1.8 };
mydata.set_column(2, new_data);
```
## Set Column Names
#### Definition and Parameters
```c++
void set_column_names(
  std::vector<std::string> const& column_names // a vector to overwrite the current column names
)
```
#### Examples
```c++
// when creating dataset through constructor like this,
// columns are not defined. So we can manually set them
DataSet<double> mydata(2, 2);
std::vector<std::string> col_names = { "col1", "col2" };
mydata.set_column_names(col_names);
```
## Rename Columns
#### Definition and Parameters
```c++
void rename(
  // key = original column name, value = new column name
  std::unordered_map<std::string, std::string> new_cols
)
```
#### Examples
```c++
DataSet<double> mydata(2, 2);

std::vector<std::string> col_names = { "col1", "col2" };
mydata.set_column_names(col_names);

std::unordered_map<std::string, std::string> remap_cols = {
    { "col1", "new_col1" } // original name, new name
};
mydata.rename(remap_cols);
```
## Transpose Data
#### Definition and Parameters
NOTE: When transposing data, the column names will be reset to default names \
such as `col0`, `col1`, etc.
```c++
DataSet<T> transpose()
```
#### Examples
```c++
DataSet<double> mydata(2, 3);

std::vector<double> row1 = { 1.0, 2.0, 3.0 };
std::vector<double> row2 = { 4.0, 5.0, 6.0 };

mydata.set_row(0, row1);
mydata.set_row(1, row2);

std::vector<std::string> col_names = { "col1", "col2", "col3" };
mydata.set_column_names(col_names);

mydata.head();

std::cout << "\n";

// tranpose returns an object and does not modify it inplace
mydata.transpose().head();

// if you want to overwrite your dataset with the tranpose, you can
mydata = mydata.transpose();
```
Output:
```
col1            col2            col3
--------------------------------------------  
1.000000        2.000000        3.000000      
4.000000        5.000000        6.000000      

col0            col1
-----------------------------
1.000000        4.000000
2.000000        5.000000
3.000000        6.000000
```
