#include "data/DataSet.hpp"

int main()
{
    DataSet<double> mydata, train, test;
    mydata.load("example_data.csv");

    // parameters: test_size_ratio, train data, test data
    // here, 30% of the data will be randomly sampled into "test"
    // both data sets are passed by reference
    mydata.split_data(0.3, train, test);

    train.head();
    std::cout << "\n";
    test.head();

    return 0;
}