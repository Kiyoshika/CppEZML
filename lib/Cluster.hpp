#ifndef CLUSTER_HPP
#define CLUSTER_HPP
#include <vector>
class Cluster {
    private:

    public:
        // no labels for clustering algorithms; returns vector with clustered classes
        virtual void fit(
            DataSet<double> & // data
        ) = 0;

        virtual DataSet<size_t> predict(
            DataSet<double> & // data
            ) = 0;
};
#endif