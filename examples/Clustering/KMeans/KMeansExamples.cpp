#include <iostream>
#include "data/DataSet.hpp"
#include "models/clustering/KMeans.hpp"

// user-defined distance function for KNN
// function MUST contain two vectors of doubles

// the size of the vectors is checked on the backend, so no need to manually check
// if the sizes of x1 and x2 match

// this is an example of Manhattan distance
double custom_distance(std::vector<double> x1, std::vector<double> x2)
{
    double distance = 0;
    for (size_t i = 0; i < x1.size(); ++i)
    {
        distance += abs(x1[i] - x2[i]);
    }

    return distance;
}

int main()
{

    DataSet<double> full_data("datasets/small_classification_test.csv");

    std::vector<std::string> idx = {"target"};
    DataSet<double> xdata = full_data.drop<double>(idx);

    // NOTE: A random seed is used to select initial clusters, so running the algorithm
    // multiple times can lead to different results.

    // KMeans with two clusters (default 1000 max iterations)
    KMeans km(2);
    km.fit(xdata);
    km.predict(xdata).head();

    // KMeans with three clusters, 10,000 max iterations
    // and a user-defined distance function
    KMeans km_custom(3, 10000, &custom_distance);
    km_custom.fit(xdata);
    km_custom.predict(xdata).head();

    return 0;
}
