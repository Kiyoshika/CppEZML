#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
class Cluster {
    private:

    public:
        // no labels for clustering algorithms; returns vector with clustered classes
        virtual std::vector<int> fit(
            std::vector<std::vector<double>> const& // data
        ) = 0;
};
#endif