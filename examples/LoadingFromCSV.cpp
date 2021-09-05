#include "data/DataSet.h"

int main()
{
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

    return 0;
}