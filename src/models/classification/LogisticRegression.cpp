#include "LogisticRegression.hpp"

double LogisticRegression::m_predict(std::vector<double> &weights, std::vector<double> input_x_row)
{

    double result = 0;

    for (size_t i = 0; i < weights.size() - 1; ++i)
    {
        result += weights[i] * input_x_row[i];
    }

    // bias term
    result += weights[weights.size() - 1];
    // using sigmoid as link function
    return sigmoid(result);
}

DataSet<size_t> LogisticRegression::predict(DataSet<double> &input_x)
{
    DataSet<size_t> prediction_data;

    if (!is_fitted)
    {
        throw std::logic_error("Please fit() your model before calling predict()!");
    }

    std::vector<size_t> predictions;
    // allocate size
    predictions.resize(input_x.count_rows());

    for (size_t current_row = 0; current_row < input_x.count_rows(); ++current_row)
    {
        // predict() in this case is the private function...no recursion here
        predictions[current_row] = m_predict(weights, input_x.get_row(current_row));
    }

    prediction_data.resize(predictions.size(), 1);
    prediction_data.set_column(0, predictions);
    std::vector<std::string> col_name = {"predicted_y"};
    prediction_data.set_column_names(col_name);

    return prediction_data;
}

std::vector<double> LogisticRegression::monte_carlo_cv(DataSet<double> xdata, DataSet<size_t> ydata, size_t k, double test_ratio)
{
    Classifier& ref = *this;
    return Classifier::monte_carlo_cv(ref, xdata, ydata, k, test_ratio);
}