#include <iostream>
#include "data/DataSet.hpp"

int main()
{

    DataSet<std::string> mydata("datasets/null_values_example.csv");

    // create local variable
    std::string val = "a";

    // we can create a lambda and pass local variables to use in filtering.

    // this can be useful for cases such as computing the mean of a column
    // and filtering for values > or < than the mean

    // IMPORTANT: the type of std::vector MUST match the DataSet<> type
    auto lambda = [val](std::vector<std::string> idx){
        // the index 0 here represents filtering the 0th column
        return idx[0] == val;
    };

    mydata.filter(lambda).head();

    // if desired, you can define the lambda inside the filter method itself
    mydata.filter([](std::vector<std::string> idx){
        return idx[0] == "g";
    }).head();

    // the first approach (creating outer-scope lamda) can be useful
    // if you reuse filters.

    // otherwise, for ad-hoc filters it's cleaner to defined it within the
    // filter() scope like the second approach.

    return 0;
}
