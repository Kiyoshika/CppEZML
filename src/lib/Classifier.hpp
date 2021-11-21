#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP
#include <vector>
#include <iostream>
#include "../data/DataSet.hpp"
#include "../stats/Stats.hpp"

class Classifier
{
public:
    // all classes must implement a predict() and fit() method
    virtual void fit(
        DataSet<double> &,   // data
        DataSet<size_t> &    // target
        ) = 0;

    virtual DataSet<size_t> predict(DataSet<double> &) = 0;

    double get_f1_score(DataSet<size_t> &actual_y, DataSet<size_t> predicted_y);

    std::vector<double> monte_carlo_cv(Classifier &model, DataSet<double> xdata, DataSet<size_t> ydata, size_t k = 3, double test_ratio = 0.3);
};
#endif