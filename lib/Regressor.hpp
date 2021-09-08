#ifndef REGRESSOR_HPP
#define REGRESSOR_HPP
#include <vector>
#include <math.h>
#include "../data/DataSet.hpp"
// Base class for all regressors. Contains metrics and such that's common among all models
class Regressor
{
public:
    // all classes must implement a predict() and fit() method
    virtual void fit(
        DataSet<double> &, // data
        DataSet<double> &  // target
        ) = 0;

    virtual DataSet<double> predict(DataSet<double> &) = 0;

    // root mean squared error
    double get_rmse(DataSet<double> &actual_y, DataSet<double> &predicted_y)
    {
        double sum = 0;
        for (int i = 0; i < actual_y.count_rows(); ++i)
        {
            sum += (actual_y(i, 0) - predicted_y(i, 0)) * (actual_y(i, 0) - predicted_y(i, 0));
        }

        return sqrt(sum / actual_y.count_rows());
    }
};
#endif