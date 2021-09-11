#include <iostream>
#include "data/DataSet.hpp"
#include "models/classification/DecisionTree.hpp"

int main()
{

    DataSet<double> full_data("datasets/small_classification_test.csv");
    
    std::vector<std::string> target_col = {"target"};
    DataSet<double> xdata = full_data.drop<double>(target_col);
    DataSet<size_t> ydata = full_data.select<size_t>(target_col);

    // default decision tree
    // parameters: max_depth = 1000, min_samples_split = 2, categorical_columns = NULL
    DecisionTree dt;
    dt.fit(xdata, ydata);

    DataSet<size_t> preds = dt.predict(xdata);
    std::cout << dt.get_f1_score(ydata, preds) << "\n";



    // decision tree with modified parameters and one categorical column
    // max_depth = 10, min_samples_split = 5, categorical_columns = {0}
    std::vector<size_t> categorical_columns = {0};
    DecisionTree dt2(10, 5, &categorical_columns);

    dt2.fit(xdata, ydata);
    DataSet<size_t> preds2 = dt2.predict(xdata);
    std::cout << dt2.get_f1_score(ydata, preds2) << "\n";

    /*
    About the parameters:
    
    max_depth - this is the maximum depth for the tree to grow. Larger sizes can potentially lead to overfitting.
    
    min_samples_split - this is the minimum number of data points inside a leaf in order to 
    allow it to split into two different nodes.
    
    categorical_columns - this specifies which column(s) are categorical instead of numeric. 
    Instead of splitting on the median (like numeric columns), the data is partitioned on each discrete value of the
    category when evaluating information gain.
    */

    return 0;
}
