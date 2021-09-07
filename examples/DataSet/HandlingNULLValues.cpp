#include "data/DataSet.hpp"

int main()
{
    /*
    This shows some functions for checking/replacing null values.

    One caveat to this, if your data includes null (blank) values, it
    MUST be loaded as std::string. Trying to load null values with
    double/integer types will lead to errors as it won't be able to
    convert a blank value to a double/int.

    After removing/replacing null values, you can then cast the data set
    to a double/int type.
    */

    // if your data contains null values, it MUST be loaded as std::string !!
    DataSet<std::string> mydata("small_classification_test.csv");

    // count null columns and print to console
    mydata.countna();

    // count null columns and return it as a vector
    // (each index corresponds to the column index)
    std::vector<size_t> null_column_counts = mydata.countna_vector();

    // drop rows that contain a null value
    // you can drop the values inplace or return a copy with the values dropped
    // (default = false)
    // here, we drop the values inplace directly modifying "mydata"
    mydata.dropna(true).head();

    // return a copy of dropped values without modifying "mydata"
    // note that this is for display only since in the previous line we already
    // dropped the values inplace
    DataSet<std::string> mydata_dropped = mydata.dropna();

    // replace all null values with a passed value (std::string)
    // can optionally modify data set inplace or return a copy
    // (default = false)
    // here, we modify the values inplace
    mydata.replacena("not null anymore", true).head();

    // return a copy of replaced values without modifying "mydata"
    DataSet<std::string> mydata_replaced = mydata.replacena("not null anymore");
    mydata_replaced.head();

    return 0;
}