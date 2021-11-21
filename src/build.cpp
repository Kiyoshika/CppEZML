#include "data/DataSet.hpp"
#include "models/classification/LogisticRegression.hpp"

int main()
{
    DataSet<double> full_data("../datasets/small_classification_test.csv");
    
    std::vector<std::string> target = {"target"};
    DataSet<double> xdata = full_data.drop<double>(target);
    DataSet<size_t> ydata = full_data.select<size_t>(target);

    LogisticRegression lr;
    lr.fit(xdata, ydata);

    std::vector<double> cv_stats = lr.monte_carlo_cv(xdata, ydata);
    std::cout << "Mean: " << cv_stats[0] << "\n";
    std::cout << "StDev: " << cv_stats[1] << "\n";

    return 0;
}