#ifndef GAUSSIANNAIVEBAYES_HPP
#define GAUSSIANNAIVEBAYES_HPP
#include <unordered_map>
#include <set>
#include <vector>
#include "../../lib/Classifier.hpp"
#include "../../stats/Stats.hpp"

class GaussianNaiveBayes : public Classifier
{
    private:
        // a map that partitions the data set based on the class
        // key = class, value = 2D vector of data
        std::unordered_map<int, std::vector<std::vector<double>>> partitioned_data;

        // a map of density estimators based on the class
        // key = class, value = vector of vectors containing mean/sigma estimates for the normal distribution
        // i.e, each (inner) vector only has two elements: [0] = mu, [1] = sigma

        // NOTE: each element/index of the (inner) vector corresponds to a fit on each column
        std::unordered_map<int, std::vector<std::vector<double>>> density_estimators;

        double gaussian(double x, double mu, double sigma)
        {
            return (1 / (sigma*sqrt(2*3.14159))) * exp( (-(x - mu)*(x - mu)) /  (2*sigma*sigma) );
        }


    public:
        void fit(std::vector<std::vector<double>> const& data, std::vector<int> const& target) override
        {
            // partition the data based on class
            for (int row = 0; row < data.size(); ++row)
            {
                partitioned_data[target[row]].push_back(data[row]);
            }

            // fit density estimators on each class
            std::vector<double> column_data;
            std::vector<double> parameter_estimates;
            double mu, sigma;
            Stats stats;
            for (int classes = 0; classes < partitioned_data.size(); ++classes)
            {
                for (int col = 0; col < partitioned_data[classes][0].size(); ++col)
                {
                    // transpose column data into single vector
                    column_data.clear();
                    for (int row = 0; row < partitioned_data[classes].size(); ++row)
                    {
                        column_data.push_back(partitioned_data[classes][row][col]);
                    }
                    parameter_estimates.clear();
                    parameter_estimates.push_back(stats.mean(column_data));
                    parameter_estimates.push_back(stats.stdev(column_data));

                    density_estimators[classes].push_back(parameter_estimates);
                }
            }
        }

        std::vector<int> predict(std::vector<std::vector<double>> const& data) override
        {
            // vector for the likelihoods of each class
            // NOTE: to avoid the problem of incredibly small values when taking products, using log likelihood instead
            // vector syntax: [class][log likelihood of feature i]
            std::vector<double> class_log_likelihood;

            std::vector<int> predictions;

            // iterate over data points and compute the log likelihood across all features for all classes to compare
            double log_likelihood;
            int arg_max;
            double mu, sigma;
            for (int row = 0; row < data.size(); ++row)
            {
                for (int classes = 0; classes < density_estimators.size(); ++classes)
                {
                    log_likelihood = 0;
                    for (int col = 0; col < data[0].size(); ++col)
                    {
                        mu = density_estimators[classes][col][0];
                        sigma = density_estimators[classes][col][1];
                        log_likelihood += std::log(gaussian(data[row][col], mu, sigma));
                    }
                    class_log_likelihood.push_back(log_likelihood);
                }
                arg_max = std::distance(class_log_likelihood.begin(), std::max_element(class_log_likelihood.begin(), class_log_likelihood.end()));
                predictions.push_back(arg_max);
                class_log_likelihood.clear();
            }

            return predictions;
        }
};

#endif