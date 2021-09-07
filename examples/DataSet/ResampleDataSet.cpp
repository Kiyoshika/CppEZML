#include "data/DataSet.hpp"

int main()
{
    DataSet<std::string> mydata("example_data.csv");

    // sample without replacement
    // (will throw error if sampled size is larger than actual data set)
    DataSet<std::string> resampled_without_replacement = mydata.sample(100);
    resampled_without_replacement.head(100);
    
    // sample with replacement
    DataSet<std::string> resampled_with_replacement = mydata.sample(100, true);
    resampled_with_replacement.head(100);
    
    return 0;
}