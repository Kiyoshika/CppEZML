#ifndef GAUSSIANNAIVEBAYES_HPP
#define GAUSSIANNAIVEBAYES_HPP
#include <unordered_map>
#include <set>
#include <vector>
#include "../../lib/Classifier.hpp"
#include "../../stats/Stats.hpp"
#include "../../data/DataSet.hpp"

class GaussianNaiveBayes : public Classifier
{
    private:
        // a map that partitions the data set based on the class
        // key = class, value = DataSet<> for the class
        std::unordered_map<size_t, DataSet<double>> partitioned_data;

        // a map of density estimators based on the class
        // key = class, value = DataSet<> containing mean/sigma estimates for the normal distribution
        // i.e, each row of data set only has two elements: [0] = mu, [1] = sigma

        // NOTE: each row corresponds to a fit on each column of the training data set
        std::unordered_map<size_t, DataSet<double>> density_estimators;

        std::vector<size_t> unique_classes;

        double gaussian(double x, double mu, double sigma)
        {
            return (1 / (sigma*sqrt(2*3.14159))) * exp( (-(x - mu)*(x - mu)) /  (2*sigma*sigma) );
        }

        std::vector<size_t> get_unique_labels(std::vector<size_t> labels)
        {
            std::vector<size_t> labels_copy = labels;
            std::sort(labels_copy.begin(), labels_copy.end());
            labels_copy.erase(std::unique(labels_copy.begin(), labels_copy.end()), labels_copy.end());

            return labels_copy;
        }


    public:
        void fit(DataSet<double> &data, DataSet<size_t> &target) override
        {
            std::unordered_map<size_t, size_t> class_counts;
            unique_classes = get_unique_labels(target.get_column(0));
            std::unordered_map<size_t, size_t> class_count_iter;

            // count data points per class to resize data sets inside partitioned_data
            for (size_t i = 0; i < target.count_rows(); ++i)
            {
                class_counts[target(i, 0)]++;
            }

            // resize DataSet<> objects in maps
            for (size_t class_label : unique_classes)
            {
                partitioned_data[class_label].resize(class_counts[class_label], data.count_columns());
                density_estimators[class_label].resize(data.count_columns(), 2);
                // keeps track of the current index of the class when partitioning data
                class_count_iter[class_label] = 0;
            }

            // partition the data based on class
            for (size_t row = 0; row < data.count_rows(); ++row)
            {
                partitioned_data[target(row, 0)].set_row(class_count_iter[target(row, 0)], data.get_row(row));
                class_count_iter[target(row, 0)]++;
            }

            // fit density estimators on each class
            std::vector<double> column_data;
            std::vector<double> parameter_estimates;
            double mu, sigma;
            Stats stats;
            for (size_t classes : unique_classes)
            {
                for (int col = 0; col < partitioned_data[classes].count_columns(); ++col)
                {
                    // transpose column data into single vector
                    column_data.clear();
                    for (int row = 0; row < partitioned_data[classes].count_rows(); ++row)
                    {
                        column_data.push_back(partitioned_data[classes](row, col));
                    }
                    parameter_estimates.clear();
                    parameter_estimates.push_back(stats.mean(column_data));
                    parameter_estimates.push_back(stats.stdev(column_data));

                    density_estimators[classes].set_row(col, parameter_estimates);
                }
            }
        }

        DataSet<size_t> predict(DataSet<double> &data) override
        {
            // vector for the likelihoods of each class
            // NOTE: to avoid the problem of incredibly small values when taking products, using log likelihood instead
            // vector syntax: [class][log likelihood of feature i]
            std::vector<double> class_log_likelihood;

            std::vector<size_t> predictions;

            // iterate over data points and compute the log likelihood across all features for all classes to compare
            double log_likelihood;
            int arg_max;
            double mu, sigma;
            for (size_t row = 0; row < data.count_rows(); ++row)
            {
                for (size_t classes : unique_classes)
                {
                    log_likelihood = 0;
                    for (size_t col = 0; col < data.count_columns(); ++col)
                    {
                        mu = density_estimators[classes](col, 0);
                        sigma = density_estimators[classes](col, 1);
                        log_likelihood += std::log(gaussian(data(row, col), mu, sigma));
                    }
                    class_log_likelihood.push_back(log_likelihood);
                }
                arg_max = std::distance(class_log_likelihood.begin(), std::max_element(class_log_likelihood.begin(), class_log_likelihood.end()));
                predictions.push_back(arg_max);
                class_log_likelihood.clear();
            }

            DataSet<size_t> prediction_data(predictions.size(), 1);
            prediction_data.set_column(0, predictions);
            std::vector<std::string> col_name = {"predicted_y"};
            prediction_data.set_column_names(col_name);

            return prediction_data;
        }

        std::vector<double> k_fold_cv(DataSet<double> xdata, DataSet<size_t> ydata, size_t k = 10, double test_ratio = 0.3)
        {
            return Classifier::k_fold_cv<GaussianNaiveBayes>(xdata, ydata, k, test_ratio);
        }
};

#endif