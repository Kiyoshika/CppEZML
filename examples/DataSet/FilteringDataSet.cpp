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
    DataSet<double> mydata("datasets/small_regression_test.csv");

    // filtering using external user-defined function (useful if you use the filter multiple times)
    DataSet<double> filtered_data = mydata.filter(&my_filter);
    filtered_data.head();

    // filtering using lambdas (useful if you only intend to use this filter once)
    // in this case, I'm just replicating the same filter as the external function
    
    // note that the std::vector argument must be the same data type as the data set
    // (in this case, double)
    DataSet<double> lambda_filter = mydata.filter([](std::vector<double> indices){
        return (indices[0] > 3.0) && (indices[4] > 5.0);
    });
    lambda_filter.head();

    return 0;
}
