#include <iostream>
#include "data/DataSet.hpp"
#include "stats/density/KernelDensity.hpp"

int main()
{

    DataSet<double> full_data("datasets/small_classification_test.csv");

    // KDE is fit on vectors (which can be extracted with get_column)
    KernelDensity kde;
    kde.fit(full_data.get_column(0));
    
    std::cout << kde.evaluate(3) << "\n";
    std::cout << kde.integrate(1, 3) << "\n";
    std::cout << kde.cdf(2.5) << "\n";
    std::cout << kde.inverse_cdf(0.5) << "\n";

    return 0;
}
