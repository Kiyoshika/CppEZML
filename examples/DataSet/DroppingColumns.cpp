#include "data/DataSet.hpp"


int main()
{
    // load data as strings
    DataSet<double> mydata("small_classification_test.csv");

    // assume data has columns:
    // col1, col2, col3, target
    std::vector<std::string> idx = {"target"};
    DataSet<double> subset = mydata.drop<double>(idx);
    subset.head();

    // you can also drop by index
    std::vector<size_t> idx2 = {1,3};
    DataSet<double> subset2 = mydata.drop<double>(idx2);
    subset2.head();

    return 0;
}