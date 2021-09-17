#ifndef RANDOMFOREST_HPP
#define RANDOMFOREST_HPP
#include <time.h>
#include <math.h>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <memory>
#include "DecisionTree.hpp"
#include "../../data/DataSet.hpp"
class RandomForest : public Classifier {
    private:
        // The maximum amount of columns to randomly sample from the data set
        // Will by default be sqrt(n) where n is the number of columns (after calling fit())
        size_t max_column_sample;

        // Number of trees to "grow" in the forest
        size_t forest_size;

        // when calling fit(), a tree is created with the random subsets
        // and stored inside this dictionary.
        // key = tree_n (the n-th tree), value = a fit decision tree
        std::vector<std::shared_ptr<DecisionTree>> decision_tree_vector;

        // each tree randomly selects columns (features)
        // keep track of which columns were selected for which tree
        DataSet<size_t> selected_columns;

        // get the mode of an integer vec
        int mode(std::vector<size_t> data)
        {
            // key = data value, value = count of occurences
            std::map<size_t, size_t> mode_map;
            for (size_t i = 0; i < data.size(); ++i)
            {
                mode_map[data[i]]++;
            }

            std::map<size_t, size_t>::iterator mode_map_counter
                = std::max_element(mode_map.begin(),mode_map.end(),[] (const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b)->bool{ return a.second < b.second; } );
        return mode_map_counter->first;
        }

    public:
        RandomForest(size_t max_column_sample = 0, size_t forest_size = 100) : max_column_sample{max_column_sample}, forest_size{forest_size} {}

        void fit(DataSet<double> &data, DataSet<size_t> &target) override
        {
            decision_tree_vector.resize(forest_size);

            // set max_column_sample to sqrt(n) if it was not provided in constructor
            if (max_column_sample == 0) { max_column_sample = (size_t)sqrt(data.count_columns()); }
            if (max_column_sample > data.count_columns()) { throw std::invalid_argument("max_column_sample cannot be larger than the column size."); }

            // the unique indices sampled according to max_column_sample
            std::vector<size_t> unique_column_indices;

            // size up selected_columns to record which column indices were selected
            selected_columns.resize(forest_size, max_column_sample);

            // iterate over forest_size and grow individual decision trees
            for (size_t tree_n = 0; tree_n < forest_size; ++tree_n)
            {
                // random seed
                srand(time(NULL));

                // sample random columns according to max_column_sample
                size_t random_int;
                unique_column_indices.clear();
                while (unique_column_indices.size() < max_column_sample)
                {
                    random_int = rand() % data.count_columns();
                    if (std::find(unique_column_indices.begin(), unique_column_indices.end(), random_int) == unique_column_indices.end())
                    {
                        // if index has not been encountered yet
                        unique_column_indices.push_back(random_int);
                    }
                }

                // convert current data to data set in order to select relevant columns
                DataSet<double> subset = data.select<double>(unique_column_indices);

                // resample subset with replacement to increase the variation in sample data (bootstrapping)
                DataSet<double> resampled_subset(subset.count_rows(), subset.count_columns());
                std::vector<size_t> resampled_target_vec;
                size_t rand_index;
                for (size_t i = 0; i < subset.count_rows(); ++i)
                {
                    rand_index = rand() % subset.count_rows();
                    resampled_subset.set_row(i, subset.get_row(rand_index));
                    resampled_target_vec.push_back(target(rand_index, 0));
                }

                DataSet<size_t> resampled_target(resampled_target_vec.size(), 1);
                resampled_target.set_column(0, resampled_target_vec);

                decision_tree_vector[tree_n] = std::make_shared<DecisionTree>();
                decision_tree_vector[tree_n]->fit(resampled_subset, resampled_target);

                // record which columns were selected
                selected_columns.set_row(tree_n, unique_column_indices);
            }
        }

        DataSet<size_t> predict(DataSet<double> &data) override
        {
            // iterate over stored trees and get prediction vectors
            // note that each COLUMN refers to a data point. Now we need
            // to transpose this such that each ROW is a data point.
            // That way, we can iterate over each ROW and take the mode
            // for the "ensemble" prediction

            DataSet<double> subset;
            DataSet<size_t> prediction_vector_matrix(forest_size, data.count_rows());
            DataSet<size_t> tree_predictions;
            for (size_t tree_n = 0; tree_n < forest_size; ++tree_n)
            {
                // select the same columns from fit()
                subset = data.select<double>(selected_columns.get_row(tree_n));
                tree_predictions = decision_tree_vector[tree_n]->predict(subset);
                prediction_vector_matrix.set_row(tree_n, tree_predictions.get_column(0));
            }

            // tranpose prediction matrix so that each ROW is a data point
            prediction_vector_matrix = prediction_vector_matrix.transpose();

            // iterate over prediction matrix and take the mode of each row which is
            // the "ensemble" prediction for each data point
            std::vector<size_t> predictions;
            for (size_t i = 0; i < prediction_vector_matrix.count_rows(); ++i)
            {
                // should probably implement this mode() in the Stats library sometime...
                predictions.push_back(mode(prediction_vector_matrix.get_row(i)));
            }

            DataSet<size_t> predictions_data(predictions.size(), 1);
            predictions_data.set_column(0, predictions);
            std::vector<std::string> col_name = {"predicted_y"};
            predictions_data.set_column_names(col_name);

            return predictions_data;
        }

        std::vector<double> k_fold_cv(DataSet<double> xdata, DataSet<size_t> ydata, size_t k = 10, double test_ratio = 0.3)
        {
            return Classifier::k_fold_cv<RandomForest>(xdata, ydata, k, test_ratio);
        }
};
#endif