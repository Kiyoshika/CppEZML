#include "data/DataSet.h"


int main()
{
    /*
    This showcases the cast() method to convert an
    entire data set to some data type. Although, this
    should probably rarely be used as there are already
    convenient methods of converting.

    For example, you can directly load the data set as
    a particular data type (see LoadingFromCSV.cpp).

    You can also convert data types while also selecting
    columns/subsetting (see SelectingColumns.cpp).

    Nevertheless, this method was implemented just in case
    there was a special use case.
    */

    // load data as strings
    DataSet<std::string> mydata;
    mydata.load("small_classification_test.csv");

    // convert entire data set to double
    DataSet<double> mydata_double = mydata.cast<double>();
    mydata_double.head();

    return 0;
}