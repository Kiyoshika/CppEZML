#include <iostream>
#include "data/DataSet.hpp"
#include "models/regression/LinearRegression.hpp"

// absolute loss
// note that the order of the parameters DO matter (actual MUST be first)
double custom_loss(double actual_y, double predicted_y)
{
    return abs(actual_y - predicted_y);
}

int main()
{
    /*
    This example shows two examples of a default linear regression and a regression model
    which takes a user-defined loss function
    */
    DataSet<double> full_data("datasets/small_regression_test.csv");

    std::vector<std::string> idx = {"target"};
    DataSet<double> xdata = full_data.drop<double>(idx);
    DataSet<double> ydata = full_data.select<double>(idx);

    // default linear regression (max_iter = 100000, learning_rate = 0.001)
    LinearRegression lr;
    lr.fit(xdata, ydata);
    
    // root mean squared error
    DataSet<double> preds = lr.predict(xdata);
    std::cout << lr.get_rmse(ydata, preds) << "\n";

    // print coefficients
    lr.get_weights().head();

    // print predictions
    preds.head();



    // linear regression with custom loss function (passed as reference)
    // 200,000 iterations with 0.01 learning rate
    LinearRegression custom_lr(200000, 0.01, &custom_loss);
    custom_lr.fit(xdata, ydata);

    // root mean squared error
    DataSet<double> preds2 = custom_lr.predict(xdata);
    std::cout << custom_lr.get_rmse(ydata, preds2) << "\n";

    // print coefficients
    custom_lr.get_weights().head();

    // print predictions
    preds2.head();

    return 0;
}
