#ifndef KMEANS_HPP
#define KMEANS_HPP
#include <iostream>
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include "../../lib/Cluster.hpp"
class KMeans : public Cluster {
    private:
        int k, total_iterations = 0, max_iter;
        bool computed_centroids = false; // false on first iteration, then set to true after computing new centroids via means

        std::unordered_map<int, int> centroid_map;
        
        // store indices of cluster centroids
        std::vector<int> centroid_index_vec;
        // assigning clusters to data points that are the closest to some centroid
        std::vector<int> assigned_clusters;
        // used to compare with newly-assigned clusters for consistent_output_check
        std::vector<int> previous_clusters;
        
        // used when computing means for new centroids
        // key = cluster, value = data point
        std::unordered_map<int, std::vector<std::vector<double>>> cluster_point_map;

        // the new "predicted" centroids per data point after completing the iteration
        // key = cluster, value = data point
        std::unordered_map<int, std::vector<double>> new_centroids;

        // used to store distances between data points and centroids
        std::vector<double> distance_vec;
        
        bool initial_clusters_created = false;

        // on first iteration of fit(), randomly choose k centroids
        void initial_clusters(std::vector<std::vector<double>> const& data)
        {
            int data_size = data.size();
            int centroid_index;
            
            // generate k random centroids
            int c = 0;
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

            // Euclidean distance
            double distance = 0;
            for (int i = 0; i < x1.size(); ++i)
            {
                distance += (x1[i] - x2[i]) * (x1[i] - x2[i]);
            }

            return sqrt(distance);
        }

    public:
        KMeans(int k, int max_iter = 1000) : k{k}, max_iter{max_iter}
        {
            if (k < 2) { throw std::invalid_argument("k cluster parameter must be at least two."); }
            if (max_iter < 1) { throw std::invalid_argument("Algorithm must have at least one iteration."); }
        }

        int argmin_index;
        std::vector<int> fit(std::vector<std::vector<double>> const& data) override
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
            for (int i = 0; i < data.size(); ++i)
            {
                // check distance to each centroid
                distance_vec.clear();
                for (int c = 0; c < this->k; ++c)
                {
                    if (!computed_centroids) // if first iteration
                    {
                        distance_vec.push_back(get_distance(data[i], data[centroid_index_vec[c]]));
                    }
                    else // all other iterations (after centroids are computed via means)
                    {
                        distance_vec.push_back(get_distance(data[i], new_centroids[c]));
                    }
                }
                argmin_index = std::distance(distance_vec.begin(), std::min_element(distance_vec.begin(), distance_vec.end()));
                assigned_clusters.push_back(argmin_index);
            }

            // check for early termination (if output is equal to prior output, then we "converged")
            if (assigned_clusters == previous_clusters) { return assigned_clusters; }

            // partition data on newly-assigned clusters
            int get_cluster;
            cluster_point_map.clear();
            for (int i = 0; i < data.size(); ++i)
            {
                get_cluster = assigned_clusters[i];
                cluster_point_map[get_cluster].push_back(data[i]);
            }

            // take partitioned data and compute means
            std::vector<double> new_computed_centroid;
            double sum;
            new_centroids.clear();
            for (int c = 0; c < this->k; ++c)
            {
                new_computed_centroid.clear();
                sum = 0;
                for (int col = 0; col < cluster_point_map[c][0].size(); ++col)
                {
                    for (int row = 0; row < cluster_point_map[c].size(); ++row)
                    {
                        sum += cluster_point_map[c][row][col];
                    }
                    new_computed_centroid.push_back(sum / cluster_point_map[c].size());
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

            return this->assigned_clusters;
        }
};
#endif