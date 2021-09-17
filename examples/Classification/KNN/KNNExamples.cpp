#include <iostream>
#include "data/DataSet.hpp"
#include "models/classification/KNN.hpp"

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
    DataSet<size_t> ydata = full_data.select<size_t>(idx);

    // default KNN with k = 3 (default distance is Euclidean)
    KNN knn(3);
    knn.fit(xdata, ydata);
    DataSet<size_t> preds = knn.predict(xdata);
    std::cout << knn.get_f1_score(ydata, preds) << "\n";

    // KNN with our custom Manhattan distance function
    KNN manhattan(3, &custom_distance);
    manhattan.fit(xdata, ydata);
    DataSet<size_t> preds2 = manhattan.predict(xdata);
    std::cout << manhattan.get_f1_score(ydata, preds) << "\n";

    // in this example data set, the F1 score turns out the same

    return 0;
}
