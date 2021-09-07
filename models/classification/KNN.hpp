#ifndef KNN_HPP
#define KNN_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>
#include <unordered_map>
#include <map>
#include <algorithm>

#include "../../lib/Classifier.hpp"
#include "../../data/DataSet.hpp"

class KNN : public Classifier
{
private:
    // hyperparameters
    int k; // number of neighbors (must be odd)
    std::unordered_map<int, std::vector<std::vector<double>>> class_data;
    std::vector<int> unique_target; // unique target labels

    // utilities
    std::vector<int> get_unique_labels(std::vector<int> const &target)
    {
        std::vector<int> labels_copy = target;
        std::sort(labels_copy.begin(), labels_copy.end());
        labels_copy.erase(std::unique(labels_copy.begin(), labels_copy.end()), labels_copy.end());

        return labels_copy;
    }

    double get_distance(std::vector<double> x1, std::vector<double> x2)
    {
        if (x1.size() != x2.size())
        {
            throw std::invalid_argument("Mismatched sizes when trying to compute distance.");
        }

        // Euclidean distance
        double distance = 0;
        for (int i = 0; i < x1.size(); ++i)
        {
            distance += (x1[i] - x2[i]) * (x1[i] - x2[i]);
        }

        return sqrt(distance);
    }

    int get_vector_index(std::vector<double> const &vec, double value)
    {
        auto it = std::find(vec.begin(), vec.end(), value);
        int index;
        if (it != vec.end())
        {
            index = it - vec.begin();
        }
        else
        {
            index = -1;
        }

        return index;
    }

    int get_vector_mode(std::vector<int> const &vec)
    {

        int mode = vec[0];     // set mode to first element of vector
        int occurrence = 1;    // its occurred once so far
        int maxOccurrence = 1; // and maxOccurrence therefore is also 1

        for (int i = 1; i < vec.size(); i++)
        {
            // if we see a new number, set occurence to 1
            if (vec[i] > vec[i - 1])
            {
                occurrence = 1;
            }

            else
                occurrence++; // if same number, simply increment occurrence

            // if occurrence is bigger than or equal to maxOccurrence, we found a new mode
            if (occurrence >= maxOccurrence)
            {
                maxOccurrence = occurrence; // update maxOccurrence
                mode = vec[i];              // update mode
            }
        }

        return mode;
    }

public:
    KNN(int k) : k{k}
    {
        if (k % 2 == 0 || k <= 0)
        {
            throw std::invalid_argument("k parameter must be an odd integer greater than 0.");
        }
    }

    void fit(std::vector<std::vector<double>> const &data, std::vector<int> const &target) override
    {
        // first, get unique values of target and partition them in class_data map
        this->unique_target = get_unique_labels(target);

        // initialize class_data with labels
        for (int i = 0; i < unique_target.size(); ++i)
        {
            class_data[unique_target[i]] = std::vector<std::vector<double>>();
        }

        // partition data based on class
        for (int i = 0; i < data.size(); ++i)
        {
            class_data[target[i]].push_back(data[i]);
        }
    }

    std::vector<int> predict(std::vector<std::vector<double>> const &data) override
    {
        // iterate over unique class partitions and create distance vectors
        std::vector<int> class_vector;
        std::vector<double> distance_vector;
        std::vector<double> original_distance_vector;
        std::vector<int> prediction_vector;
        std::vector<int> neighbors;

        int common_class, highest_count;
        int previous_value = -1;
        int current_count = 0;

        for (int current_point = 0; current_point < data.size(); ++current_point)
        {
            // reset
            distance_vector.clear();
            original_distance_vector.clear();
            class_vector.clear();
            for (int i = 0; i < unique_target.size(); ++i)
            {
                for (int r = 0; r < class_data[i].size(); ++r)
                {
                    // ignore equivalent point(s)
                    // this is a lazy (and bad) way of excluding the current data point
                    if (get_distance(data[current_point], class_data[i][r]) == 0) { continue; }
                    else 
                    {
                        distance_vector.push_back(get_distance(data[current_point], class_data[i][r]));
                        class_vector.push_back(unique_target[i]); 
                    }
                }
            }

            // sort distance vector
            original_distance_vector = distance_vector; // make a copy to preserve
            std::sort(distance_vector.begin(), distance_vector.end());

            // create vector of nearest neighbours up to k (parameter)
            neighbors.clear();
            for (int i = 0; i < k; ++i)
            {
                neighbors.push_back(class_vector[get_vector_index(original_distance_vector, distance_vector[i])]);
            }

            // find most common value in neighbors vector
            std::sort(neighbors.begin(), neighbors.end());
            prediction_vector.push_back(get_vector_mode(neighbors));
        }

        return prediction_vector;
    }
};
#endif