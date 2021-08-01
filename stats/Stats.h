#ifndef STATS_H
#define STATS_H

#include <vector>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <math.h>

class Stats
{
private:

    std::vector<double> convert_to_double(std::vector<std::vector<std::string>> &data, int col_index)
    {
        if (data.size() == 0)
        {
            std::runtime_error("Your data must have at least one element to compute statistics.");
        }
        
        std::vector<std::string> tposed;
		for (int i = 0; i < data.size(); ++i)
		{
			tposed.push_back(data[i][col_index]);
		}

		std::vector<double> data_double(tposed.size());
		try
		{
			std::transform(tposed.begin(), tposed.end(), data_double.begin(), [](const std::string &val) {
				return std::stod(val);
			});
		}
		catch (int e)
		{
			std::runtime_error("Couldn't convert your data to double in transpose_double().");
		}

        return data_double;
    }

public:
    // overload 1 - 2D string vector
    double min(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        int index = std::distance(data_double.begin(), min_element(data_double.begin(), data_double.end()));
        return data_double[index];
    }

    // overload 2 - generic double vector
    double min(std::vector<double> &data)
    {
        int index = std::distance(data.begin(), min_element(data.begin(), data.end()));
        return data[index];
    }





    // overload 1 - 2D string vector
    double max(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        int index = std::distance(data_double.begin(), max_element(data_double.begin(), data_double.end()));
        return data_double[index];
    }

    // overload 2 - generic double vector
    double max(std::vector<double> &data)
    {
        int index = std::distance(data.begin(), max_element(data.begin(), data.end()));
        return data[index];
    }





    // overload 1 - 2D string vector
    double mean(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        return std::accumulate(data_double.begin(), data_double.end(), 0.0) / data_double.size();
    }

    // overload 2 - generic double vector
    double mean(std::vector<double> &data)
    {
        return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    }





    // overload 1 - 2D string vector
    double stdev(std::vector<std::vector<std::string>> &data, int col_index, bool sample = true)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        double _mean = std::accumulate(data_double.begin(), data_double.end(), 0.0) / data_double.size();
        double _sum = 0.0;
        for_each(data_double.begin(), data_double.end(), [&](double x) {
            _sum += (x - _mean) * (x - _mean);
        });
        int sample_size = sample ? data_double.size() - 1 : data_double.size();
        return sqrt(_sum / (double)sample_size);
    }

    // overload 2 - generic double vector
    double stdev(std::vector<double> &data, bool sample = true)
    {
        double _mean = mean(data);
        double _sum = 0.0;
        for_each(data.begin(), data.end(), [&](double x) {
            _sum += (x - _mean) * (x - _mean);
        });
        int sample_size = sample ? data.size() - 1 : data.size();
        return sqrt(_sum / (double)sample_size);
    }





    // overload 1 - 2D string vector
    double median(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        std::sort(data_double.begin(), data_double.end());
        double _median;
        int midpoint;
        if (data_double.size() % 2 == 0)
        {
            midpoint = data_double.size() / 2;
            _median = (data_double[midpoint - 1] + data_double[midpoint]) / 2; 
        }
        else
        {
            midpoint = floor((double)(data_double.size() / 2));
            _median = data_double[midpoint];
        }

        return _median;
    }

    // overload 2 - generic double vector
    double median(std::vector<double> &data)
    {
        std::sort(data.begin(), data.end());
        double _median;
        int midpoint;
        if (data.size() % 2 == 0)
        {
            midpoint = data.size() / 2;
            _median = (data[midpoint - 1] + data[midpoint]) / 2; 
        }
        else
        {
            midpoint = floor((double)(data.size() / 2));
            _median = data[midpoint];
        }

        return _median;
    }





    // overload 1 - 2D string vector
    double sum(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        return std::accumulate(data_double.begin(), data_double.end(), 0.0);
    }

    // overload 2 - generic double vector
    double sum(std::vector<double> &data, int col_index)
    {
        return std::accumulate(data.begin(), data.end(), 0.0);
    }





    // overload 1 - 2D string vectors
    double percentile(std::vector<std::vector<std::string>> &data, int col_index, double percentile)
    {
        if (percentile < 0 || percentile > 1) {
            throw std::invalid_argument("Percentile must be between 0 and 1.");
        }
        std::vector<double> data_double = convert_to_double(data, col_index);
        // yeah, yeah, using == with doubles is generally not good, but we're not expecting extremely small values
        if (percentile == 0.5)
        {
            return median(data, col_index);
        }
        std::sort(data_double.begin(), data_double.end());
        double p_rank, x_int, x_fr; // percentile rank, integer and fraction parts
        // formula for extrapolated percentile taken from https://www.calculatorsoup.com/calculators/statistics/percentile-calculator.php
        p_rank = percentile * (data_double.size() - 1) + 1;
        x_fr = modf(p_rank, &x_int);

        return data_double[x_int - 1] + x_fr*(data_double[x_int] - data_double[x_int - 1]);
    }

    // overload 2 - generic double vector
    double percentile(std::vector<double> &data, double percentile)
    {
        if (percentile < 0 || percentile > 1) {
            throw std::invalid_argument("Percentile must be between 0 and 1.");
        }

        // yeah, yeah, using == with doubles is generally not good, but we're not expecting extremely small values
        if (percentile == 0.5)
        {
            return median(data);
        }
        std::sort(data.begin(), data.end());
        double p_rank, x_int, x_fr; // percentile rank, integer and fraction parts
        // formula for extrapolated percentile taken from https://www.calculatorsoup.com/calculators/statistics/percentile-calculator.php
        p_rank = percentile * (data.size() - 1) + 1;
        x_fr = modf(p_rank, &x_int);

        return data[x_int - 1] + x_fr*(data[x_int] - data[x_int - 1]);
    }


};

#endif