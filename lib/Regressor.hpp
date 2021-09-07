#ifndef REGRESSOR_HPP
#define REGRESSOR_HPP
#include <vector>
#include <math.h>
// Base class for all regressors. Contains metrics and such that's common among all models
class Regressor
{
public:
    // all classes must implement a predict() and fit() method
    virtual void fit(
        std::vector<std::vector<double>> const &, // data
        std::vector<double> const &               // target
        ) = 0;

    virtual std::vector<double> predict(std::vector<std::vector<double>> const &) = 0;

    // root mean squared error
    double get_rmse(std::vector<double> const &actual_y, std::vector<double> const &predicted_y)
    {
        double sum = 0;
        for (int i = 0; i < actual_y.size(); ++i)
        {
            sum += (actual_y[i] - predicted_y[i]) * (actual_y[i] - predicted_y[i]);
        }

        return sqrt(sum / actual_y.size());
    }
};
#endif