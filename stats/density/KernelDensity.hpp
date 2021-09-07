#ifndef KERNELDENSITY_HPP
#define KERNELDENSITY_HPP

#include <math.h>
#include <vector>
#include "../Stats.hpp"

class KernelDensity {
    private:
        // data used as a reference when computing density estimate. Used in fit() method.
        std::vector<double> fit_data;
        double bandwidth;
        
        // in the future, may generalize this to be different / custom kernels
        double gaussian_kernel(double x)
        {
            return (1 / sqrt(2*3.14159)) * exp( -0.5 * pow(x, 2));
        }

    public:
        void fit(std::vector<double> data)
        {
            // fit data and estimate bandwidth
            this->fit_data = data;

            // bandwidth estimation from "Badwidth selection" section under Wikipedia: https://en.wikipedia.org/wiki/Kernel_density_estimation
            Stats stats;
            double IQR = stats.percentile(this->fit_data, 0.75) - stats.percentile(this->fit_data, 0.25);
            double sd = stats.stdev(this->fit_data);
            double min_statistic = std::min(sd, IQR/1.34);
            this->bandwidth = 0.9 * min_statistic * std::pow(this->fit_data.size(), -0.2);
        }

        // evaluate a data point
        double evaluate(double x)
        {
            double sum = 0.0;
            for (int i = 0; i < this->fit_data.size(); ++i)
            {
                sum += this->gaussian_kernel((x - this->fit_data[i]) / this->bandwidth);
            }

            return (1 / (this->fit_data.size() * this->bandwidth)) * sum;
        }

        // approximate the integral (using Simpson's rule)
        /*
         @param a lower bound of integral
         @param b upper bound of integral
         @param steps number of subdivisions of the integration range. The larger the value, the better the approximation.
        */
        double integrate(double a, double b, int steps = 258)
        {
            if (a > b)
            {
                throw std::invalid_argument("Upper bound of integral must be greater than lower bound.");
            }

            double delta = (b - a) / (double)steps;
            double delta_step = a; // start the interval subdivision at the lower bound
            double sum = 0.0;

            for (int i = 0; i <= steps; ++i)
            {
                // if the index is first or last, multiplied by 1
                if (i == 0 || i == steps)
                {
                    sum += evaluate(delta_step);
                }
                // if index is even, multiply by 2
                else if (i % 2 == 0)
                {
                    sum += 2*evaluate(delta_step);
                }
                // if index is odd, multiply by 4
                else if (i % 2 != 0)
                {
                    sum += 4*evaluate(delta_step);
                }

                // increase step size
                delta_step += delta;
            }

            return (delta/3)*sum;
        }

        double cdf(double x)
        {
            // find lower bound starting point based off mean - 5*sd
            Stats stats;
            double lower_bound = stats.mean(this->fit_data) - 5*stats.stdev(this->fit_data);
            return integrate(lower_bound, x);
        }

        // approximate the value of x which gives probability p
        double inverse_cdf(double p)
        {
            Stats stats;
            double lower_bound = stats.mean(this->fit_data) - 5*stats.stdev(this->fit_data);
            // set initial point to the mean
            double x = stats.mean(this->fit_data);

            // acceptable error tolerance for approximation
            double margin_of_error = 0.0001;
            int max_iter = 1000; // don't want while loop to run forever
            int current_iter = 0;

            double cdf_value = cdf(x);
            while ((abs(cdf_value - p) > margin_of_error) && current_iter < max_iter)
            {
                if (cdf_value > p)
                {
                    x -= 0.1*(cdf_value - p);
                }
                else if (cdf_value < p)
                {
                    x += 0.1*(p - cdf_value);
                }

                cdf_value = cdf(x);
                current_iter += 1;
            }

            if (current_iter == max_iter)
            {
                throw std::runtime_error("Could not find a value of X within margin of error in the maximum number of iterations when computing inverse CDF.");
            }

            return x;
        }
};

#endif