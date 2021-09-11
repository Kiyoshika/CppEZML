#ifndef KMEANS_HPP
#define KMEANS_HPP
#include <iostream>
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include "../../lib/Cluster.hpp"
#include "../../data/DataSet.hpp"
class KMeans : public Cluster {
    private:
        size_t k, total_iterations = 0, max_iter;
        bool computed_centroids = false; // false on first iteration, then set to true after computing new centroids via means

        std::unordered_map<size_t, size_t> centroid_map;
        
        // store indices of cluster centroids
        std::vector<size_t> centroid_index_vec;
        // assigning clusters to data points that are the closest to some centroid
        std::vector<size_t> assigned_clusters;
        // used to compare with newly-assigned clusters for consistent_output_check
        std::vector<size_t> previous_clusters;
        
        // used when computing means for new centroids
        // key = cluster, value = data point
        std::unordered_map<size_t, DataSet<double>> cluster_point_map;

        // used to track the indices per cluster for cluster_point_map
        std::unordered_map<size_t, size_t> cluster_point_iter;

        // the new "predicted" centroids per data point after completing the iteration
        // key = cluster, value = data point
        std::unordered_map<size_t, std::vector<double>> new_centroids;

        // used to store distances between data points and centroids
        std::vector<double> distance_vec;
        
        bool initial_clusters_created = false;

        bool has_custom_distance = false;
        double (*custom_distance)(std::vector<double>, std::vector<double>) = NULL;

        // on first iteration of fit(), randomly choose k centroids
        void initial_clusters(DataSet<double> &data)
        {

            srand(time(NULL));

            size_t data_size = data.count_rows();
            size_t centroid_index;

            this->centroid_map.clear();
            this->centroid_index_vec.clear();
            
            // generate k random centroids
            size_t c = 0;
            while (c < this->k)
            {
                centroid_index = rand() % data_size;
                // check if index has NOT been used
                if (this->centroid_map.find(centroid_index) == this->centroid_map.end())
                {
                    this->centroid_index_vec.push_back(centroid_index);
                    c += 1;
                }
            }
        }

        double get_distance(std::vector<double> x1, std::vector<double> x2)
        {
            if (x1.size() != x2.size())
            {
                throw std::invalid_argument("Mismatched sizes when trying to compute distance.");
            }

            if (has_custom_distance)
            {
                return custom_distance(x1, x2);
            }

            // Euclidean distance (default)
            double distance = 0;
            for (int i = 0; i < x1.size(); ++i)
            {
                distance += (x1[i] - x2[i]) * (x1[i] - x2[i]);
            }

            return sqrt(distance);
        }

    public:
        KMeans(
            size_t k, 
            size_t max_iter = 1000,
            double (*custom_distance)(std::vector<double>, std::vector<double>) = NULL) 
            : k{k}, max_iter{max_iter}, custom_distance{custom_distance}
        {
            if (k < 2) { throw std::invalid_argument("k cluster parameter must be at least two."); }
            if (max_iter < 1) { throw std::invalid_argument("Algorithm must have at least one iteration."); }
        }

        size_t argmin_index;
        void fit(DataSet<double> &data) override
        {
            // generate random centroids
            if (!this->initial_clusters_created)
            {
                initial_clusters(data);
                this->initial_clusters_created = true;
            }

            if (assigned_clusters.size() > 0) { previous_clusters = assigned_clusters; }
            assigned_clusters.clear();
            // iterate over data and find closest centroid
            for (size_t i = 0; i < data.count_rows(); ++i)
            {
                // check distance to each centroid
                distance_vec.clear();
                for (size_t c = 0; c < this->k; ++c)
                {
                    if (!computed_centroids) // if first iteration
                    {
                        distance_vec.push_back(get_distance(data.get_row(i), data.get_row(centroid_index_vec[c])));
                    }
                    else // all other iterations (after centroids are computed via means)
                    {
                        distance_vec.push_back(get_distance(data.get_row(i), new_centroids[c]));
                    }
                }
                argmin_index = std::distance(distance_vec.begin(), std::min_element(distance_vec.begin(), distance_vec.end()));
                assigned_clusters.push_back(argmin_index);
            }

            // check for early termination (if output is equal to prior output, then we "converged")
            if (assigned_clusters == previous_clusters)
            { 
                return;
            }

            // partition data on newly-assigned clusters
            size_t get_cluster;
            cluster_point_map.clear();
            // count the data points per cluster to size up cluster_point_map
            std::unordered_map<size_t, size_t> cluster_counts;
            for (size_t cluster : assigned_clusters)
            {
                cluster_counts[cluster]++;
            }

            // initialize cluster iterators to zero
            // size up cluster_point_map
            cluster_point_iter.clear();
            for (size_t i = 0; i < this->k; ++i)
            {
                cluster_point_iter[i] = 0;
                cluster_point_map[i] = DataSet<double>(cluster_counts[i], data.count_columns());
            }


            for (size_t i = 0; i < data.count_rows(); ++i)
            {
                get_cluster = assigned_clusters[i];
                cluster_point_map[get_cluster].set_row(cluster_point_iter[get_cluster], data.get_row(i));
                cluster_point_iter[get_cluster]++;
            }

            // take partitioned data and compute means
            std::vector<double> new_computed_centroid;
            double sum;
            new_centroids.clear();
            for (size_t c = 0; c < this->k; ++c)
            {
                new_computed_centroid.clear();
                sum = 0;
                for (size_t col = 0; col < cluster_point_map[c].count_columns(); ++col)
                {
                    for (size_t row = 0; row < cluster_point_map[c].count_rows(); ++row)
                    {
                        sum += cluster_point_map[c](row, col);
                    }
                    new_computed_centroid.push_back(sum / cluster_point_map[c].count_rows());
                    sum = 0;
                }
                // add new computed centroid
                new_centroids[c] = new_computed_centroid;
            }
            // new centroids have been computed via means
            this->computed_centroids = true;
            this->total_iterations += 1;

            if (this->total_iterations < this->max_iter)
            {
                fit(data);
            }

        }

        DataSet<size_t> predict(DataSet<double> &data)
        {
            if (new_centroids.size() == 0)
            {
                throw std::runtime_error("No centroids found for KMeans. Please use fit() first.");
            }

            std::vector<double> distance;
            std::vector<size_t> predicted_clusters;
            // iterate over data and find closest centroid
            for (size_t i = 0; i < data.count_rows(); ++i)
            {
                // check distance to each centroid
                distance.clear();
                for (size_t c = 0; c < this->k; ++c)
                {
                    distance.push_back(get_distance(data.get_row(i), new_centroids[c]));

                }
                argmin_index = std::distance(distance.begin(), std::min_element(distance.begin(), distance.end()));
                predicted_clusters.push_back(argmin_index);
            }

            DataSet<size_t> clusters_data(predicted_clusters.size(), 1);
            clusters_data.set_column(0, predicted_clusters);
            std::vector<std::string> col_names = {"clusters"};
            clusters_data.set_column_names(col_names);

            return clusters_data;
        }
};
#endif