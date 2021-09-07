#include "data/DataSet.hpp"
#include <unordered_map> // technically DataSet.hpp also includes this so it's probably unnecessary
int main()
{
    DataSet<std::string> mydata("small_classification_test.csv");

    // create a map where the key is the old column name and value is new column name
    std::unordered_map<std::string, std::string> col_map = {
        {"col1", "new_col1"},
        {"col2", "new_col2"}
    };

    mydata.rename(col_map);
    mydata.head();

    return 0;
}