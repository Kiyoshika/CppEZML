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
    2. [describe()](#describe-data) - print summary statistics for numerical data
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
    2. [append()](#append-data) - merge two datasets row-wise or column-wise
    3. [select()](#select-columns) - subset a dataset to certain columns
    4. [drop()](#drop-columns) - inverse of select(); subset a dataset by dropping certain columns
    5. [filter()](#filter-data) - filter a dataset by a user-defined function
    6. [replace()](#replace-data) - replace occurences of a value with another
    7. [cast()](#cast-data) - cast contents of dataset from one datatype to another
    8. [sample()](#sample-data) - randomly sample from dataset with or without replacement
    9. [split_data()](#split-data) - split data into two portions, typically for creating train/test splits
7. Missing Values
    1. [countna()](#count-null-values) - count null columns and print to console
    2. [countna_vector()](#vector-of-null-counts) - count null columns and return a vector where each index represents a column
    3. [dropna()](#drop-null-values) - drop rows from dataset containing null values
    4. [replacena()](#replace-null-values) - replace null values with another value
8. Exporting Data
    1. [to_csv()](#writing-to-csv) - write dataset to csv file

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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
## Describe Data
#### Definition and Parameters
```c++
void describe() // NOTE: only works with numerical data
```
#### Examples
```c++
DataSet<double> mydata("datasets/small_regression_test.csv");
mydata.describe();
```
Output:
```
             |  col1            col2            col3            col4            col5            target        
----------------------------------------------------------------------------------------------------
Sum:         |  399.500000      376.000000      199.800000      477.650000      315.850000      542.850000    
Min:         |  1.600000        2.500000        0.010000        0.530000        -1.700000       1.100000      
Max:         |  6.500000        5.200000        4.200000        9.200000        7.600000        10.400000     
Mean:        |  3.995000        3.760000        1.998000        4.776500        3.158500        5.428500      
StDev:       |  1.662563        0.819214        1.365597        2.125624        2.787686        2.276435      
10th %:      |  1.700000        2.780000        0.371000        2.450000        -0.322000       2.470000      
25th %:      |  2.500000        3.200000        0.915000        3.250000        0.935000        4.275000      
Median:      |  3.800000        3.700000        1.750000        4.900000        3.450000        5.635000      
75th %:      |  5.475000        4.125000        3.050000        6.100000        5.625000        6.725000      
90th %:      |  5.920000        5.110000        4.100000        6.970000        6.480000        8.400000 
```
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
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
<br/><br/>
## Append Data
#### Definitions and Parameters
```c++
DataSet<T> append(
  DataSet<T> other_data, // the data set you want to append
  char type = 'r'        // the axis to append on. 'r' = rows, 'c' = columns
)
```
There is currently a bug where if you append on columns without having any column names, \
nothing will happen.
#### Examples
```c++
DataSet<double> dataset_one(2, 2);
DataSet<double> dataset_two(2, 2);

std::vector<std::string> col1 = { "col1", "col2" };
std::vector<std::string> col2 = { "col3", "col4" };

dataset_one.set_column_names(col1);
dataset_two.set_column_names(col2);

// append row-wise
dataset_one.append(dataset_two).head(); // new size: (4, 2)

std::cout << "\n";

// append column-wise
// NOTE: this method will NOT work if your column names are empty
dataset_one.append(dataset_two, 'c').head(); // new size: (2, 4)
```
Output:
```
col1            col2
----------------------------- 
0.000000        0.000000
0.000000        0.000000
0.000000        0.000000
0.000000        0.000000

col1            col2            col3            col4
-----------------------------------------------------------
0.000000        0.000000        0.000000        0.000000
0.000000        0.000000        0.000000        0.000000  
```
<br/><br/>
## Select Columns
#### Definition and Parameters
**Override 1**:
You can select columns by index.
```c++
DataSet<X> select(
  std::vector<size_t> const& indices // indices of columns to select
)
```
**Override 2**:
You can also select columns by name.
```c++
DataSet<X> select(
  std::vector<std::string> const& indices // names of columns to select
)
```
You can also cast data while simultaneously selecting columns (see example 2.)
#### Examples
Example 1: Selecting columns by index and name.
```c++
DataSet<double> mydata(2, 2);

std::vector<std::string> names = { "col1", "col2" };
mydata.set_column_names(names);

// select by index
std::vector<size_t> byidx = { 0 };
mydata.select<double>(byidx).head();

std::cout << "\n";

// select by name
std::vector<std::string> byname = { "col2" };
mydata.select<double>(byname).head();
```
Output:
```
col1
--------------
0.000000
0.000000

col2
--------------
0.000000
0.000000
```
Example 2: Selecting column and casting to std::string into another dataset.
```c++
DataSet<double> mydata(2, 2);

std::vector<std::string> names = { "col1", "col2" };
mydata.set_column_names(names);

// select by index
std::vector<size_t> byidx = { 0 };
DataSet<std::string> string_data1 = mydata.select<std::string>(byidx);

// select by name
std::vector<std::string> byname = { "col2" };
DataSet<std::string> string_data2 = mydata.select<std::string>(byname);
```
<br/><br/>
## Drop Columns
#### Definition and Parameters
**Override 1**:
```c++
DataSet<X> drop(                     // <X> is the new data type to cast to (if new = old, no conversion is made)
  std::vector<size_t> const& indices // indices of columns to drop
)
```
**Override 2**:
```c++
DataSet<X> drop(                          // <X> is the new data type to cast to (if new = old, no conversion is made)
  std::vector<std::string> const& indices // names of columns to drop
)
```
#### Examples
Example 1: Dropping columns by index and name.
```c++
DataSet<double> mydata(2, 2);

std::vector<std::string> names = { "col1", "col2" };
mydata.set_column_names(names);

// drop by index
std::vector<size_t> byidx = { 0 };
mydata.drop<double>(byidx).head();

std::cout << "\n";

// drop by name
std::vector<std::string> byname = { "col2" };
mydata.drop<double>(byname).head();
```
Output:
```
col2
--------------
0.000000
0.000000

col1
--------------
0.000000
0.000000
```
Example 2: Just like select(), you can cast the data after dropping columns.
```c++
DataSet<double> mydata(2, 2);

std::vector<std::string> names = { "col1", "col2" };
mydata.set_column_names(names);

// select by index
std::vector<size_t> byidx = { 0 };
DataSet<std::string> string_data1 = mydata.drop<std::string>(byidx);

// select by name
std::vector<std::string> byname = { "col2" };
DataSet<std::string> string_data2 = mydata.drop<std::string>(byname);
```
<br/><br/>
## Filter Data
#### Definition and Parameters
```c++
DataSet<T> filter(
  bool (*filter_conditions)(std::vector<T>) // function pointer or lambda
)
```
#### Examples
Example 1: Using function pointers. This is useful if you use the filter multiple times.
```c++
// NOTE: vector type must be same as DataSet<> type
// in this case, double
bool my_filter(std::vector<double> indices)
{
    // the indices refer to the column index
    return (indices[0] < indices[2]) || (indices[1] > 4);
}

// somewhere in main()...
DataSet<double> mydata("datasets/small_regression_test.csv");
mydata.filter(&my_filter).head();
```
Output:
```
col1            col2            col3            col4            col5            target        
-----------------------------------------------------------------------------------------
2.400000        5.200000        1.000000        4.800000        0.740000        6.700000
2.500000        5.100000        0.580000        4.900000        5.600000        2.500000      
2.500000        4.100000        1.800000        3.100000        5.700000        5.800000
5.700000        4.200000        1.700000        6.800000        0.610000        2.900000
3.000000        4.100000        2.700000        3.300000        -0.180000       4.900000
5.200000        5.200000        2.900000        3.600000        -1.700000       6.800000
6.100000        4.800000        4.200000        2.600000        5.800000        10.400000     
2.400000        5.200000        1.000000        4.800000        0.740000        6.700000
2.500000        5.100000        0.580000        4.900000        5.600000        2.500000
2.500000        4.100000        1.800000        3.100000        5.700000        5.800000
```
Example 2: Using lambdas. This is useful for adhoc filters that you need once.
```c++
DataSet<double> mydata("datasets/small_regression_test.csv");

mydata.filter([](std::vector<double> indices){
    return (indices[0] < indices[2]) || (indices[1] > 4);
}).head();
```
Output:
```
col1            col2            col3            col4            col5            target        
-----------------------------------------------------------------------------------------
2.400000        5.200000        1.000000        4.800000        0.740000        6.700000
2.500000        5.100000        0.580000        4.900000        5.600000        2.500000      
2.500000        4.100000        1.800000        3.100000        5.700000        5.800000
5.700000        4.200000        1.700000        6.800000        0.610000        2.900000
3.000000        4.100000        2.700000        3.300000        -0.180000       4.900000
5.200000        5.200000        2.900000        3.600000        -1.700000       6.800000
6.100000        4.800000        4.200000        2.600000        5.800000        10.400000     
2.400000        5.200000        1.000000        4.800000        0.740000        6.700000
2.500000        5.100000        0.580000        4.900000        5.600000        2.500000
2.500000        4.100000        1.800000        3.100000        5.700000        5.800000
```
Note that as of now, you can NOT capture any variables in the lambda as I'm using \
a function pointer. However, this is planned to change so you can filter against \
local variables, aggregations, etc.
<br/><br/>
## Replace Data
#### Definition and Parameters
```c++
DataSet<T> replace(
  T original_value,     // original value in dataset to replace
  T replace_value,      // value to replace with
  bool inplace = false, // whether or not to modify the dataset inplace (overwrite)
  size_t occurences = 0 // replace up to first N occurences. If 0, will replace ALL occurences
)
```
#### Examples
```c++
DataSet<double> mydata(2, 2);
mydata.set(0, 1, 5.5);
mydata.set(1, 0, 5.5);
mydata.head();

std::cout << "\n";

// replace only first occurence and modify data inplace
mydata.replace(5.5, 10.5, true, 1);
mydata.head();
```
Output:
```
0.000000        5.500000      
5.500000        0.000000


0.000000        10.500000
5.500000        0.000000
```
<br/><br/>
## Cast Data
#### Definition and Parameters
```c++
DataSet<X> cast() // <X> is the new type to cast to
```
#### Examples
```c++
DataSet<double> mydata(2, 2);
mydata.set(0, 1, 5.5);
mydata.set(1, 0, 5.5);
mydata.head();

std::cout << "\n";

// cast doubles to size_t
DataSet<size_t> size_t_data = mydata.cast<size_t>();
size_t_data.head();

std::cout << "\n";

// cast size_t to std::string
DataSet<std::string> string_data = size_t_data.cast<std::string>();
string_data.head();
```
Output:
```
0.000000        5.500000
5.500000        0.000000


0               5
5               0


0               5
5               0
```
<br/><br/>
## Sample Data
#### Definition and Parameters
```c++
DataSet<T> sample(
  size_t n = 1,        // number of records to sample
  bool replace = false // whether or not to sample with replacement
)
```
#### Examples
```c++
DataSet<double> mydata("datasets/small_regression_test.csv");
DataSet<double> without_replacement = mydata.sample(5);
DataSet<double> with_replacement = mydata.sample(100, true);
```
<br/><br/>
## Split Data
#### Definition and Parameters
```c++
void split_data(
  double test_ratio, // percentage to allocate to test data; value in interval (0, 1)
  DataSet<T> &train, // training data passed by reference
  DataSet<T> &test   // test data passed by reference
)
```
#### Examples
```c++
DataSet<double> mydata("datasets/small_regression_test.csv");
DataSet<double> train_data, test_data;
mydata.split_data(0.3, train_data, test_data);

std::cout << "Full Data Rows: " << mydata.count_rows() << "\n";
std::cout << "Train Data Rows: " << train_data.count_rows() << "\n";
std::cout << "Test Data Rows: " << test_data.count_rows() << "\n";
```
Output:
```
Full Data Rows: 100
Train Data Rows: 71
Test Data Rows: 29
```
Seems there's some rounding error, but nevertheless, test size is ~30%.
<br/><br/>
## Count Null Values
#### Definition and Parameters
```c++
void countna() // NOTE: only works with std::string data
```
#### Examples
```c++
DataSet<std::string> mydata(2, 2);

std::vector<std::string> cols = { "col1", "col2" };
mydata.set_column_names(cols);

mydata.set(0, 1, "X");
mydata.set(1, 0, "X");

mydata.head();

std::cout << "\n";

// only works if you have column names defined
mydata.countna();
```
Output:
```
col1            col2
-----------------------------
                X
X

column name : null count
--------------------------
col1 : 1
col2 : 1
```
<br/><br/>
## Vector of Null Counts
#### Definition and Parameters
```c++
std::vector<size_t> countna_vector() // NOTE: only works with std::string data
```
#### Examples
```c++
DataSet<std::string> mydata(2, 2);

std::vector<std::string> cols = { "col1", "col2" };
mydata.set_column_names(cols);

mydata.set(0, 1, "X");
mydata.set(1, 0, "X");

std::vector<size_t> null_column_counts = mydata.countna_vector(); // { 1, 1 }
```
<br/><br/>
## Drop Null Values
#### Definition and Parameters
Will drop any rows that contain a null (empty) value. \
Known Issues:
* inplace = true currently doesn't function as expected

```c++
DataSet<T> dropna(     // NOTE: only works on std::string data
  bool inplace = false // whether or not to modify the dataset inplace
)
```
#### Examples
```c++
DataSet<std::string> mydata(2, 2);

std::vector<std::string> cols = { "col1", "col2" };
mydata.set_column_names(cols);

mydata.set(0, 1, "X");
mydata.set(1, 0, "X");
mydata.set(1, 1, "X");

mydata.head();

// mydata.dropna(true) to modify dataset inplace
mydata = mydata.dropna();

std::cout << "\n";

mydata.head();
```
Output:
```
col1            col2
----------------------------- 
                X
X               X

col1            col2
-----------------------------
X               X
```
<br/><br/>
## Replace Null Values
#### Definitions and Parameters
```c++
DataSet<T> replacena(             // NOTE: only works on std::string data
  const std::string replace_text, // the text you want to replace the null values with
  bool inplace = false            // whether or not you want to modify the dataset inplace
)
```
#### Examples
```c++
DataSet<std::string> mydata(2, 2);

std::vector<std::string> cols = { "col1", "col2" };
mydata.set_column_names(cols);

mydata.set(0, 1, "X");
mydata.set(1, 0, "X");
mydata.set(1, 1, "X");

mydata.head();

mydata.replacena("Y", true);

std::cout << "\n";

mydata.head();
```
Output:
```
col1            col2
-----------------------------
                X
X               X

col1            col2
-----------------------------
Y               X
X               X
```
<br/><br/>
## Writing to CSV
#### Definition and Parameters
```c++
void to_csv(
  std::string file_name,   // output filename / filepath
  std::string sep = ",",   //separator to write csv file with
  bool print_header = true // whether or not to print the column headers to the output file
)
```
#### Examples
```c++
DataSet<double> mydata("datasets/small_regression_test.csv");
mydata.to_csv("output.csv"); // writes to local directory
```
