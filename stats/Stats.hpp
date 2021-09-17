#ifndef STATS_H
#define STATS_H

#include <vector>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <math.h>

class Stats
{

public:

    double min(std::vector<double> data)
    {
        int index = std::distance(data.begin(), min_element(data.begin(), data.end()));
        return data[index];
    }

    double max(std::vector<double> data)
    {
        int index = std::distance(data.begin(), max_element(data.begin(), data.end()));
        return data[index];
    }

    double mean(std::vector<double> data)
    {
        return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    }

    double stdev(std::vector<double> data, bool sample = true)
    {
        double _mean = mean(data);
        double _sum = 0.0;
        for_each(data.begin(), data.end(), [&](double x) {
            _sum += (x - _mean) * (x - _mean);
        });
        int sample_size = sample ? data.size() - 1 : data.size();
        return sqrt(_sum / (double)sample_size);
    }

    double median(std::vector<double> data)
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

    double sum(std::vector<double> data)
    {
        return std::accumulate(data.begin(), data.end(), 0.0);
    }

    double percentile(std::vector<double> data, double percentile)
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