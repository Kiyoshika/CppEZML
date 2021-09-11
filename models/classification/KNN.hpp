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
    size_t k; // number of neighbors (must be odd)
    std::unordered_map<size_t, DataSet<double>> class_data;
    std::vector<size_t> unique_target; // unique target labels

    // if user passes custom distance function
    double (*custom_distance)(std::vector<double>, std::vector<double>);
    bool has_custom_distance = false;

    // utilities
    std::vector<size_t> get_unique_labels(std::vector<size_t> target)
    {
        std::vector<size_t> labels_copy = target;
        std::sort(labels_copy.begin(), labels_copy.end());
        labels_copy.erase(std::unique(labels_copy.begin(), labels_copy.end()), labels_copy.end());

        return labels_copy;
    }

    double get_distance(
        std::vector<double> x1, 
        std::vector<double> x2, 
        double (*custom_distance)(std::vector<double>, std::vector<double>) = NULL
    )
    {

        if (x1.size() != x2.size())
        {
            throw std::invalid_argument("Mismatched sizes when trying to compute distance.");
        }

        if (has_custom_distance)
        {
            return custom_distance(x1, x2);
        }
        // default distance (Euclidean)
        else
        {
            double distance = 0;

            for (size_t i = 0; i < x1.size(); ++i)
            {
                distance += (x1[i] - x2[i]) * (x1[i] - x2[i]);
            }

            return sqrt(distance);
        }

        // technically unreachable but required
        return 0.0;
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

    size_t get_vector_mode(std::vector<size_t> &vec)
    {

        size_t mode = vec[0];     // set mode to first element of vector
        size_t occurrence = 1;    // its occurred once so far
        size_t maxOccurrence = 1; // and maxOccurrence therefore is also 1

        for (size_t i = 1; i < vec.size(); i++)
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
    KNN(size_t k, double (*custom_distance)(std::vector<double>, std::vector<double>) = NULL) : k{k}
    {
        if (custom_distance != NULL)
        {
            has_custom_distance = true;
            this->custom_distance = custom_distance;
        }

        if (k % 2 == 0 || k <= 0)
        {
            throw std::invalid_argument("k parameter must be an odd integer greater than 0.");
        }
    }

    void fit(DataSet<double> &data, DataSet<size_t> &target) override
    {
        // first, get unique values of target and partition them in class_data map
        this->unique_target = get_unique_labels(target.get_column(0));

        std::unordered_map<size_t, size_t> class_counts;
        std::unordered_map<size_t, size_t> class_count_iter;

        // count data points per class to resize data sets inside class_data
        class_counts.clear();
        for (size_t i = 0; i < target.count_rows(); ++i)
        {
            class_counts[target(i, 0)]++;
        }

        // resize data sets for each class
        for (size_t class_label : unique_target)
        {
            class_data[class_label].resize(class_counts[class_label], data.count_columns());
            // keeps track of the current index of the class when partitioning data
            class_count_iter[class_label] = 0;
        }

        // partition data based on class
        for (size_t i = 0; i < data.count_rows(); ++i)
        {
            class_data[target(i, 0)].set_row(class_count_iter[target(i, 0)], data.get_row(i));
            class_count_iter[target(i, 0)]++;
        }
    }

    DataSet<size_t> predict(DataSet<double> &data) override
    {
        // iterate over unique class partitions and create distance vectors
        std::vector<size_t> class_vector;
        std::vector<double> distance_vector;
        std::vector<double> original_distance_vector;
        std::vector<size_t> prediction_vector;
        std::vector<size_t> neighbors;

        size_t common_class, highest_count;
        size_t current_count = 0;

        for (size_t current_point = 0; current_point < data.count_rows(); ++current_point)
        {
            // reset
            distance_vector.clear();
            original_distance_vector.clear();
            class_vector.clear();
            for (size_t class_label : unique_target)
            {
                for (size_t r = 0; r < class_data[class_label].count_rows(); ++r)
                {
                    // ignore equivalent point(s)
                    // this is a lazy (and bad) way of excluding the current data point
                    if (get_distance(data.get_row(current_point), class_data[class_label].get_row(r), this->custom_distance) == 0) { continue; }
                    else 
                    {
                        distance_vector.push_back(get_distance(data.get_row(current_point), class_data[class_label].get_row(r), this->custom_distance));
                        class_vector.push_back(class_label); 
                    }
                }
            }

            // sort distance vector
            original_distance_vector = distance_vector; // make a copy to preserve
            std::sort(distance_vector.begin(), distance_vector.end());

            // create vector of nearest neighbours up to k (parameter)
            neighbors.clear();
            for (size_t i = 0; i < k; ++i)
            {
                neighbors.push_back(class_vector[get_vector_index(original_distance_vector, distance_vector[i])]);
            }

            // find most common value in neighbors vector
            std::sort(neighbors.begin(), neighbors.end());
            prediction_vector.push_back(get_vector_mode(neighbors));
        }

        DataSet<size_t> prediction_data(prediction_vector.size(), 1);
        prediction_data.set_column(0, prediction_vector);
        std::vector<std::string> col_name = {"predicted_y"};
        prediction_data.set_column_names(col_name);

        return prediction_data;
    }
};
#endif