#include "data/DataSet.hpp"

int main()
{
    DataSet<std::string> mydata("example_data.csv");
    mydata.to_csv("export_test.csv");

    // you can export data even if the types are numeric
    // (they will be converted to strings)
    DataSet<double> mydata_numeric("example_data.csv");
    mydata_numeric.to_csv("export_test.csv");

    // some additional parameters are specifying a delimiter and if you
    // want to print the headers/column names (default = true).
    // here, we write with delimiter "|" and don't print the column headers
    mydata.to_csv("export_test.csv", "|", false);

    return 0;
}