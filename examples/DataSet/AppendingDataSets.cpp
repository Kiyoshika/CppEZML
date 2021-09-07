#include "data/DataSet.hpp"

int main()
{
    DataSet<std::string> mydata("small_classification_test.csv");

    // select two arbitrary columns
    std::vector<size_t> idx1 = {0, 1};
    DataSet<std::string> sub1 = mydata.select<std::string>(idx1);

    // select two other arbitrary columns
    std::vector<size_t> idx2 = {2, 3};
    DataSet<std::string> sub2 = mydata.select<std::string>(idx2);

    // append data sets along rows
    // NOTE: the column names will be from the original data set (sub1)
    DataSet<std::string> appended_rows = sub1.append(sub2, 'r');
    appended_rows.head(appended_rows.count_rows());

    // append data sets along columns
    DataSet<std::string> appended_columns = sub1.append(sub2, 'c');
    appended_columns.head();

    return 0;
}