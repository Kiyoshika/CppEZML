#include "data/DataSet.h"

int main()
{
    /*
    the select() method allows you to select columns from the data set
    based off column indices or column names. It also allows you to
    optionally cast the data type.

    In the example below, we create two instances where we keep one
    as strings and convert the other to double.
    */

    // load data as strings
    DataSet<std::string> mydata;
    mydata.load("example_data.csv");

    // column indices that we want to select
    std::vector<int> idx = {0, 2};
    
    // subset the column of same data type (strings)
    DataSet<std::string> string_subset = mydata.select<std::string>(idx);
    string_subset.head();

    // subset the columns AND convert them to double
    DataSet<double> double_subset = mydata.select<double>(idx);
    double_subset.head();

    // you can also select columns by name
    std::vector<std::string> idx2 = {"col1", "col4"};

    // subset columns by name and convert them to integers
    DataSet<int> int_subset = mydata.select<int>(idx2);
    int_subset.head();

    // of course, you can double subset
    std::vector<std::string> idx3 = {"col4"};
    DataSet<int> int_subset2 = int_subset.select<int>(idx3);
    int_subset2.head();
    
    return 0;
}