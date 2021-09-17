#ifndef REGRESSOR_HPP
#define REGRESSOR_HPP
#include <vector>
#include <math.h>
#include "../data/DataSet.hpp"
#include "../stats/Stats.hpp"
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
    double get_rmse(DataSet<double> &actual_y, DataSet<double> predicted_y)
    {
        double sum = 0;
        for (int i = 0; i < actual_y.count_rows(); ++i)
        {
            sum += (actual_y(i, 0) - predicted_y(i, 0)) * (actual_y(i, 0) - predicted_y(i, 0));
        }

        return sqrt(sum / actual_y.count_rows());
    }

    template <typename Model>
    std::vector<double> k_fold_cv(DataSet<double> xdata, DataSet<double> ydata, size_t k, double test_ratio = 0.3)
    {
        if (k < 2) { throw std::invalid_argument("k must be at least two for k_fold_cv."); }
        
        DataSet<double> full_data = xdata.append(ydata, 'c');
        DataSet<double> train_data, test_data, train_x, test_x, train_y, test_y;

        std::vector<double> rmse_values(k);
        std::vector<double> return_values(2); // 0 = mean of rmse values, 1 = stdev of rmse values

        for (size_t fold = 0; fold < k; ++fold)
        {
            full_data.split_data(test_ratio, train_data, test_data);

            std::vector<size_t> last_column = {full_data.count_columns() - 1};
            train_x = train_data.drop<double>(last_column);
            train_y = train_data.select<double>(last_column);

            test_x = test_data.drop<double>(last_column);
            test_y = test_data.select<double>(last_column);

            Model model;
            model.fit(train_x, train_y);
            
            rmse_values[fold] = model.get_rmse(test_y, model.predict(test_x));
        }

        Stats stats;
        return_values[0] = stats.mean(rmse_values);
        return_values[1] = stats.stdev(rmse_values);

        return return_values;
    }
};
#endif