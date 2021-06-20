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
    double min(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        int index = std::distance(data_double.begin(), min_element(data_double.begin(), data_double.end()));
        return data_double[index];
    }

    double max(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        int index = std::distance(data_double.begin(), max_element(data_double.begin(), data_double.end()));
        return data_double[index];
    }

    double mean(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        return std::accumulate(data_double.begin(), data_double.end(), 0.0) / data_double.size();
    }

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

    double sum(std::vector<std::vector<std::string>> &data, int col_index)
    {
        std::vector<double> data_double = convert_to_double(data, col_index);
        return std::accumulate(data_double.begin(), data_double.end(), 0.0);
    }

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


};

#endif