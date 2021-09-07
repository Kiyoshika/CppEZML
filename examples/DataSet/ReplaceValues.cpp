#include "data/DataSet.hpp"

int main()
{
    /*
    replace() allows you to replace instances of a value with another.
    Unline replacena() (see HandlingNULLValues.cpp), replace() supports numeric
    data types instead of only std::string.

    You can modify values inplace or return a copy
    */

    // std::string example
    DataSet<std::string> mydata("small_classification_test.csv");
    // modify values inplace by passing "true" to the third parameter (default = false)
    mydata.replace("4.70", "999", true).head();
    // return a copy and not modify "mydata"
    DataSet<std::string> mydata2 = mydata.replace("4.70", "999");

    // double example
    DataSet<double> mydata_double("small_classification_test.csv");
    // modify values inplace by passing "true" to the third parameter (default = false)
    mydata_double.replace(4.70, 999, true).head();
    // return a copy and not modify "mydata"
    DataSet<double> mydata3 = mydata_double.replace(4.70, 999);

    // you can also limit the replacements to the first N occurences.
    // if you pass -1 then it will replace ALL occurences (default behaviour).
    // below will replace the two three occurences of "5.80" with "999"
    // and modify it inplace
    mydata.replace("5.80", "999", false, 2).head();
    
    return 0;
}