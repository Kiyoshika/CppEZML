#include <iostream>
#include "data/DataSet.hpp"
#include "models/classification/RandomForest.hpp"
#include "models/classification/DecisionTree.hpp"

int main()
{

    DataSet<double> full_data("datasets/small_classification_test.csv");
    
    std::vector<std::string> target_col = {"target"};
    DataSet<double> xdata = full_data.drop<double>(target_col);
    DataSet<size_t> ydata = full_data.select<size_t>(target_col);

    /*
    About the parameters: (will need to allow passing DecisionTree parameters into random forest as well)

    max_column_sample - the number of columns to randomly sample (default = sqrt(columns))

    forest_size - the number of trees to "grow" (default = 100)
    */

    // default RandomForest
    // parameters: max_column_sample = sqrt(f), forest_size = 100
    RandomForest rf;
    rf.fit(xdata, ydata);

    DataSet<size_t> preds = rf.predict(xdata);
    std::cout << rf.get_f1_score(ydata, preds) << "\n";



    // RandomForest using 1000 trees and sampling all columns instead of sqrt(columns)
    RandomForest rf2(xdata.count_columns(), 1000);
    rf2.fit(xdata, ydata);

    DataSet<size_t> preds2 = rf2.predict(xdata);
    std::cout << rf2.get_f1_score(ydata, preds2) << "\n";

    return 0;
}
