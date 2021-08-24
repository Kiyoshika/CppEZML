#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H
#include <time.h>
#include <math.h>
#include <algorithm>
#include <unordered_map>
#include <map>
#include "DecisionTree.h"
#include "../../data/DataSet.h"
class RandomForest : public Classifier {
    private:
        // The maximum amount of columns to randomly sample from the data set
        // Will by default be log(n) where n is the number of columns (after calling fit())
        int max_column_sample;

        // Number of trees to "grow" in the forest
        int forest_size;

        // when calling fit(), a tree is created with the random subsets
        // and stored inside this dictionary.
        // key = tree_n (the n-th tree), value = a fit decision tree
        std::unordered_map<int, DecisionTree*> decision_tree_dictionary;

        // each tree randomly selects columns (features)
        // keep track of which columns were selected for which tree
        std::unordered_map<int, std::vector<int>> selected_columns;

        // get the mode of an integer vec
        int mode(std::vector<int> const& data)
        {
            // key = data value, value = count of occurences
            std::map<int, int> mode_map;
            for (int i = 0; i < data.size(); ++i)
            {
                mode_map[data[i]]++;
            }

            std::map<int,int>::iterator mode_map_counter
                = std::max_element(mode_map.begin(),mode_map.end(),[] (const std::pair<int,int>& a, const std::pair<int,int>& b)->bool{ return a.second < b.second; } );
        std::cout << "MODE: " << mode_map_counter->first << "\n";
        return mode_map_counter->first;
        }

    public:
        RandomForest(int max_column_sample = -1, int forest_size = 100) : max_column_sample{max_column_sample}, forest_size{forest_size} {}
        
        ~RandomForest()
        {
            // empty decision_tree_dictionary which stores pointers to DecisionTree()
            if (decision_tree_dictionary.size() > 0)
            {
                for (int i = 0; i < decision_tree_dictionary.size(); ++i)
                {
                    delete decision_tree_dictionary[i];
                }
            }
        }

        void fit(std::vector<std::vector<double>> const& data, std::vector<int> const& target) override
        {
            // set max_column_sample to log(n) if it was not provided in constructor
            // NOTE: using + 1 incase the log(n) evaluates to zero (you can't sample zero columns...)
            if (max_column_sample == -1) { max_column_sample = (int)log(data[0].size() + 1); }

            // random seed
            srand(time(NULL));

            // the unique indices sampled according to max_column_sample
            std::vector<int> unique_column_indices;

            // iterate over forest_size and grow individual decision trees
            for (int tree_n = 0; tree_n < forest_size; ++tree_n)
            {
                // sample random columns according to max_column_sample
                int random_int;
                while (unique_column_indices.size() < max_column_sample)
                {
                    random_int = rand() % data[0].size();
                    if (std::find(unique_column_indices.begin(), unique_column_indices.end(), random_int) == unique_column_indices.end())
                    {
                        // if index has not been encountered yet
                        unique_column_indices.push_back(random_int);
                    }
                }

                // convert current data to data set in order to select relevant columns

                std::vector<std::vector<double>> subset = DataSet::select_ext(data, unique_column_indices);

                // resample subset with replacement to increase the variation in sample data (bootstrapping)
                subset = DataSet::sample_ext(data);
     
                // insert pointer to DecisionTree and fit on subset
                decision_tree_dictionary.insert(std::make_pair(tree_n, new DecisionTree()));
                decision_tree_dictionary[tree_n]->fit(subset, target);

                // record which columns were selected
                selected_columns.insert(std::make_pair(tree_n, unique_column_indices));
            }
        }

        std::vector<int> predict(std::vector<std::vector<double>> const& data) override
        {
            // iterate over stored trees and get prediction vectors
            // note that each COLUMN refers to a data point. Now we need
            // to transpose this such that each ROW is a data point.
            // That way, we can iterate over each ROW and take the mode
            // for the "ensemble" prediction

            std::vector<std::vector<double>> subset;
            std::vector<std::vector<int>> prediction_vector_matrix;
            std::vector<int> tree_predictions;
            for (int tree_n = 0; tree_n < forest_size; ++tree_n)
            {
                // select the same columns from fit()
                subset = DataSet::select_ext(data, selected_columns[tree_n]);
                std::cout << "Iter #" << tree_n << ": size: " << decision_tree_dictionary.at(tree_n)->predict(subset).size() << "\n";
                //prediction_vector_matrix.push_back(tree_predictions);
            }

            // tranpose prediction matrix so that each ROW is a data point
            //prediction_vector_matrix = DataSet::transpose_ext(prediction_vector_matrix);
            std::cout << prediction_vector_matrix.size() << "\n";

            // iterate over prediction matrix and take the mode of each row which is
            // the "ensemble" prediction for each data point
            std::vector<int> predictions;
            for (int i = 0; i < prediction_vector_matrix.size(); ++i)
            {
                // should probably implement this mode() in the Stats library sometime...
                predictions.push_back(mode(prediction_vector_matrix[i]));
            }

            return predictions;
        }
};
#endif