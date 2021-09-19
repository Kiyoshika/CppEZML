#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP
#include <vector>
#include <iostream>
#include "../data/DataSet.hpp"
#include "../stats/Stats.hpp"
// Base class for all classifiers. Contains metrics and such that's common among all models
class Classifier
{
public:
    // all classes must implement a predict() and fit() method
    virtual void fit(
        DataSet<double> &,   // data
        DataSet<size_t> &    // target
        ) = 0;

    virtual DataSet<size_t> predict(DataSet<double> &) = 0;

    double get_f1_score(DataSet<size_t> &actual_y, DataSet<size_t> predicted_y)
    {
        if (actual_y.count_rows() != predicted_y.count_rows())
        {
            throw std::invalid_argument("actual_y and predicted_y must be of the same size!");
        }

        size_t true_positives = 0;
        size_t false_positives = 0;
        size_t false_negatives = 0;
        // obtain all necessary values in one loop and calculate at once
        for (size_t i = 0; i < actual_y.count_rows(); ++i)
        {
            if (actual_y(i, 0) == predicted_y(i, 0) && actual_y(i, 0) == 1)
            {
                true_positives += 1;
            }
            else if (actual_y(i, 0) == 0 && predicted_y(i, 0) == 1)
            {
                false_positives += 1;
            }
            else if (actual_y(i, 0) == 1 && predicted_y(i, 0) == 0)
            {
                false_negatives += 1;
            }
        }

        return (double)true_positives / ((double)true_positives + 0.5 * ((double)false_positives + (double)false_negatives));
    }

    template <typename Model>
    std::vector<double> monte_carlo_cv(Model *model_pointer, DataSet<double> xdata, DataSet<size_t> ydata, size_t k, double test_ratio = 0.3)
    {
        if (k < 2) { throw std::invalid_argument("k must be at least two for k_fold_cv."); }
        
        DataSet<double> full_data = xdata.append(ydata.cast<double>(), 'c');
        DataSet<double> train_data, test_data, train_x, test_x;
        DataSet<size_t> train_y, test_y;

        std::vector<double> f1_values(k);
        std::vector<double> return_values(2); // 0 = mean of f1 values, 1 = stdev of f1 values

        std::vector<size_t> last_column;

        for (size_t fold = 0; fold < k; ++fold)
        {
            full_data.split_data(test_ratio, train_data, test_data);

            last_column = {full_data.count_columns() - 1};
            train_x = train_data.drop<double>(last_column);
            train_y = train_data.select<size_t>(last_column);

            test_x = test_data.drop<double>(last_column);
            test_y = test_data.select<size_t>(last_column);

            // passing a pointer to capture the constructor parameters from the model
            Model model = *model_pointer;
            model.fit(train_x, train_y);
            
            f1_values[fold] = model.get_f1_score(test_y, model.predict(test_x));
        }

        Stats stats;
        return_values[0] = stats.mean(f1_values);
        return_values[1] = stats.stdev(f1_values);

        return return_values;
    }
};
#endif