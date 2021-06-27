#ifndef KMEANS_H
#define KMEANS_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include "../../lib/Cluster.h"
class KMeans : public Cluster {
    private:
        int k, total_iterations = 0, max_iter;

        std::unordered_map<int, int> centroid_map;
        std::unordered_map<int, std::vector<std::vector<double>>> cluster_map;
        
        std::vector<int> centroid_index_vec;
        std::vector<int> cluster_vec;
        
        // used when computing means for new centroids
        std::vector<std::vector<double>> cluster_points;
        
        bool initial_clusters_created = false;

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

        std::vector<int> fit(std::vector<std::vector<double>> const& data) override
        {
            // generate random centroids
            if (!this->initial_clusters_created)
            {
                initial_clusters(data);
                this->initial_clusters_created = true;
            }

            // iterate over data and assign clusters
            std::vector<int> cluster_vec_copy = this->cluster_vec; // take current snapshot of the clusters
            std::vector<double> cluster_point;
            std::vector<double> distance_vec;
            int cluster_argmin;
            for (int i = 0; i < data.size(); ++i)
            {
                distance_vec.clear();

                for (int c = 0; c < k; ++c)
                {
                    // cluster_points will be populated when re-adjusting centroids
                    // this first condition is the special case for the first iteration of the algorithm (random centroids)
                    if (this->cluster_points.size() == 0)
                    {
                        cluster_point = data[this->centroid_index_vec[c]];
                    }
                    else
                    {
                        cluster_point = this->cluster_points[c];
                    }
                    distance_vec.push_back(get_distance(data[i], cluster_point));
                    // after distances are obtained, clear the cluster points
                    this->cluster_points.clear();
                }

                // argmin for distance vector
                cluster_argmin = std::distance(distance_vec.begin(), std::min_element(distance_vec.begin(), distance_vec.end()));
                this->cluster_vec.push_back(cluster_argmin);
                this->cluster_map[cluster_argmin].push_back(data[i]);
            }

            bool matching_clusters = false;
            if (cluster_vec_copy.size() > 0)
            {
                // compare clusters and check any changes
                for (int i = 0; i < cluster_vec_copy.size(); ++i)
                {
                    if (cluster_vec_copy[i] != this->cluster_vec[i])
                    {
                        matching_clusters = false;
                        break;
                    }
                }
            }

            if (!matching_clusters)
            {
                // if cluster assignments have changed, re compute centroids by taking means and refit
                double mean;
                for (int c = 0; c < k; ++c)
                {
                    this->cluster_points.push_back(std::vector<double>());
                    for (int col = 0; col < this->cluster_map[c][0].size(); ++col)
                    {
                        mean = 0;
                        for (int row = 0; row < this->cluster_map[c].size(); ++row)
                        {
                            mean += this->cluster_map[c][row][col];
                        }
                        this->cluster_points[c].push_back(mean / this->cluster_map[c].size());
                    }
                }
                total_iterations += 1;
            }

            if (total_iterations < max_iter)
            {
                // empty current "predictions" when re-iterating algorithm
                this->cluster_vec.clear();
                fit(data); 
            }

            return this->cluster_vec;
        }
};
#endif