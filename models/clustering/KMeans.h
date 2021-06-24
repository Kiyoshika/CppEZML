#ifndef KMEANS_H
#define KMEANS_H
#include <iostream>
#include <vector>
#include "lib/Cluster.h"
class KMeans : public Cluster {
    private:
        int k;

    public:
        KMeans(int k) : k{k}
        {
            if (k < 2) { throw std::invalid_argument("k cluster parameter must be at least two."); }
        }

        std::vector<int> fit(std::vector<std::vector<double>> const& data) override
        {
            // unfinished
        }
};
#endif