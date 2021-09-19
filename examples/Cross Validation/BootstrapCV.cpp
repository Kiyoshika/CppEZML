#include <iostream>
#include "data/DataSet.hpp"
#include "models/regression/LinearRegression.hpp"

int main()
{
    DataSet<double> mydata("datasets/small_regression_test.csv");

    std::vector<std::string> target_col = { "target" };
    DataSet<double> xdata = mydata.drop<double>(target_col);
    DataSet<double> ydata = mydata.select<double>(target_col);

    LinearRegression lr;

    // regression models use RMSE metric for evaluation
    // classification models use F1 metric for evaluation

    // default number of splits is 30 but higher values are recommended for 
    // more consistent statistics. Here I am using 1000 random splits at 70% train 30% test (default)
    std::vector<double> bcv = lr.bootstrap_cv(xdata, ydata, 1000);

    std::cout << "Mean: " << bcv[0] << "\n";
    std::cout << "StDev: " << bcv[1] << "\n";

    return 0;
}