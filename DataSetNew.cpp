#include "data/DataSetNew.h"


int main()
{
    // load data as strings
    DataSet<std::string> mydata;
    mydata.load("small_classification_test.csv");

    DataSet<double> mydata_double = mydata.cast<double>();
    mydata_double.head();

    return 0;
}