#include <iostream>
#include "data/DataSet.hpp"
#include "models/regression/LinearRegression.hpp"
#include "models/classification/LogisticRegression.hpp"

int main()
{

    // K fold cross validation on linear regression model
    // Linear models by default use root mean squared error (RMSE)
    // support will be added soon for custom metrics

    /*
    Parameters:
    xdata <double> - independent variables
    ydata <double> - dependent variable
    k (default = 10) - number of folds to split data randomly to fit and evaluate
    test_ratio (default = 0.3) - the percentage to allocate test data when splitting. By default it's 70% train 30% test.
    */
    DataSet<double> full_data_regression("datasets/small_regression_test.csv");
    
    std::vector<std::string> target_col = {"target"};
    DataSet<double> xdata = full_data_regression.drop<double>(target_col);
    DataSet<double> ydata = full_data_regression.select<double>(target_col);

    LinearRegression lr;
    std::vector<double> lr_cv = lr.k_fold_cv(xdata, ydata);

    std::cout << "Mean of 10-Fold CV: " << lr_cv[0] << "\n"; // first index returns the mean of the K folds
    std::cout << "StDev of 10-Fold CV: " << lr_cv[1] << "\n"; // second index returns the sample standard deviation of the K folds

    // k_fold_cv with the other parameters mentioned above
    std::vector<double> lr_cv_2 = lr.k_fold_cv(xdata, ydata, 5, 0.25);
    std::cout << "Mean of 5-Fold CV: " << lr_cv_2[0] << "\n";
    std::cout << "StDev of 5-Fold CV: " << lr_cv_2[1] << "\n";
    



    // K fold cross validation on logistic regression model
    // Classification models by default use f1 score
    // support will be added soon for custom metrics
    DataSet<double> full_data_classification("datasets/small_classification_test.csv");
    
    std::vector<std::string> target_col = {"target"};
    DataSet<double> xdata = full_data_classification.drop<double>(target_col);
    DataSet<size_t> ydata = full_data_classification.select<size_t>(target_col);

    LogisticRegression lr;
    std::vector<double> lr_cv = lr.k_fold_cv(xdata, ydata);

    std::cout << "Mean of 10-Fold CV: " << lr_cv[0] << "\n";
    std::cout << "StDev of 10-Fold CV: " << lr_cv[1] << "\n";
    return 0;
}
