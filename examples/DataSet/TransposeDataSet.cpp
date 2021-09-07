#include "data/DataSet.hpp"

int main()
{
    DataSet<std::string> mydata("small_classification_test.csv");

    // original data
    mydata.head();
    
    // transposed data (print)
    mydata.transpose().head();

    // transposed data (assignment)
    DataSet<std::string> transposed_data = mydata.transpose();

    return 0;
}