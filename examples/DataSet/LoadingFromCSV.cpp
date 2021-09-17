#include "data/DataSet.hpp"

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