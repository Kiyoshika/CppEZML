#include <iostream>
#include "data/DataSet.hpp"
#include "models/classification/LogisticRegression.hpp"

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
    DataSet<double> full_data("datasets/small_classification_test.csv");

    std::vector<std::string> idx = {"target"};
    DataSet<double> xdata = full_data.drop<double>(idx);
    DataSet<size_t> ydata = full_data.select<size_t>(idx);

    // default logistic regression (max_iter = 100000, learning_rate = 0.001)
    LogisticRegression lr;
    lr.fit(xdata, ydata);
    
    // F1 Score
    DataSet<size_t> preds = lr.predict(xdata);
    std::cout << lr.get_f1_score(ydata, preds) << "\n";

    // print coefficients
    lr.get_weights().head();

    // print predictions
    preds.head();



    // linear regression with custom loss function (passed as reference)
    // 200,000 iterations with 0.01 learning rate
    LogisticRegression custom_lr(200000, 0.01, &custom_loss);
    custom_lr.fit(xdata, ydata);

    // F1 Score
    DataSet<size_t> preds2 = custom_lr.predict(xdata);
    std::cout << custom_lr.get_f1_score(ydata, preds2) << "\n";

    // print coefficients
    custom_lr.get_weights().head();

    // print predictions
    preds2.head();

    return 0;
}
