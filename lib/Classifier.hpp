#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP
#include <vector>
#include <iostream>
#include "../data/DataSet.hpp"
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

    double get_f1_score(DataSet<size_t> &actual_y, DataSet<size_t>  &predicted_y)
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
};
#endif