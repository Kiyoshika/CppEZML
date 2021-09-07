#include "data/DataSet.hpp"

// this is a user-defined filter
// MUST be bool and MUST be same data type as data set
// this will filter records where column 0 > 3.0 and column 4 > 5.0
bool my_filter(std::vector<double> indices)
{
    return (indices[0] > 3.0) && (indices[4] > 5.0);
}

int main()
{
    DataSet<double> mydata("example_data.csv");

    DataSet<double> filtered_data = mydata.filter(&my_filter);
    filtered_data.head();

    return 0;
}