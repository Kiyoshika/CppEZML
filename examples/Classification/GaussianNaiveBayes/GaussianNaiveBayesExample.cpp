#include <iostream>
#include "data/DataSet.hpp"
#include "models/classification/GaussianNaiveBayes.hpp"

int main()
{

    DataSet<double> full_data("datasets/small_classification_test.csv");

    std::vector<std::string> idx = {"target"};
    DataSet<double> xdata = full_data.drop<double>(idx);
    DataSet<size_t> ydata = full_data.select<size_t>(idx);

    GaussianNaiveBayes gnb;
    gnb.fit(xdata, ydata);
    DataSet<size_t> preds = gnb.predict(xdata);
    std::cout << gnb.get_f1_score(ydata, preds) << "\n";
    
    return 0;
}
