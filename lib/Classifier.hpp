#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP
#include <vector>
#include <iostream>
// Base class for all classifiers. Contains metrics and such that's common among all models
class Classifier
{
public:
    // all classes must implement a predict() and fit() method
    virtual void fit(
        std::vector<std::vector<double>> const&,   // data
        std::vector<int> const&                    // target
        ) = 0;

    virtual std::vector<int> predict(std::vector<std::vector<double>> const&) = 0;

    double get_f1_score(std::vector<int> const &actual_y, std::vector<int> const &predicted_y)
    {
        if (actual_y.size() != predicted_y.size())
        {
            throw std::invalid_argument("actual_y and predicted_y must be of the same size!");
        }

        int true_positives = 0;
        int false_positives = 0;
        int false_negatives = 0;
        // obtain all necessary values in one loop and calculate at once
        for (int i = 0; i < actual_y.size(); ++i)
        {
            if (actual_y[i] == predicted_y[i] && actual_y[i] == 1)
            {
                true_positives += 1;
            }
            else if (actual_y[i] == 0 && predicted_y[i] == 1)
            {
                false_positives += 1;
            }
            else if (actual_y[i] == 1 && predicted_y[i] == 0)
            {
                false_negatives += 1;
            }
        }

        return (double)true_positives / ((double)true_positives + 0.5 * ((double)false_positives + (double)false_negatives));
    }
};
#endif