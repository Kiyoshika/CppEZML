#ifndef LINEARREGRESSION_HPP
#define LINEARREGRESSION_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>

#include "../../lib/Regressor.hpp"
#include "../../data/DataSet.hpp"

class LinearRegression : public Regressor
{
private:
	// hyperparameters
	size_t max_iter;
	double learning_rate;
	double (*user_loss_func)(double, double) = nullptr;

	bool is_fitted = false;
	std::vector<double> weights;

	std::vector<std::string> independent_variable_names;

	// default loss
	double loss(double actual_y, double predicted_y)
	{
		// mean squared error
		return (actual_y - predicted_y) * (actual_y - predicted_y);
	}

	double loss_deriv(double actual_y, double predicted_y)
	{
		// using limit derivatives from calculus one
		double loss_derivative;
		try
		{
			if (user_loss_func != NULL)
			{
				loss_derivative = (user_loss_func(actual_y, predicted_y + 0.0001) - user_loss_func(actual_y, predicted_y)) / 0.0001;
			} else 
			{
				loss_derivative = (loss(actual_y, predicted_y + 0.0001) - loss(actual_y, predicted_y)) / 0.0001;
			}
		}
		catch (int e)
		{
			loss_derivative = 0;
		}

		return loss_derivative;
	}

	// local predict, not to be confused with the public predict()
	double predict(std::vector<double> &weights, std::vector<double> input_x_row)
	{

		double result = 0;

		for (size_t i = 0; i < weights.size() - 1; ++i)
		{
			result += weights[i] * input_x_row[i];
		}

		// bias term
		result += weights[weights.size() - 1];
		return result;
	}

public:
	/*
	* max_iter - maximum iterations to run gradient descent algorithm
	* learning_rate - softening parameter to reduce jumping around the loss function
	* loss_func - optional loss function that can be passed by reference from user if they want to specify their own loss function
	*/
	LinearRegression(size_t max_iter = 100000, double learning_rate = 0.001, double (*loss_func)(double, double) = nullptr)
	: max_iter{max_iter}, learning_rate{learning_rate} 
	{
		if (loss_func != NULL) { user_loss_func = loss_func; }
	}

	void fit(DataSet<double> &train_x, DataSet<double> &train_y) override
	{
		this->independent_variable_names = train_x.column_names;

		// create initial weights equal to size of input columns (# of indep vars)
		for (size_t col = 0; col < train_x.count_columns(); ++col)
		{
			weights.push_back(0);
		}

		weights.push_back(0); // add bias term

		// vector to adjust the weights from loss derivative
		std::vector<double> weight_adjustments(weights.size());

		size_t iter = 0;
		double prediction, loss_derivative;

		while (iter < max_iter)
		{
			loss_derivative = 0;

			// reset weight_adjustments to zero
			std::fill(weight_adjustments.begin(), weight_adjustments.end(), 0.0);

			for (size_t w = 0; w < weight_adjustments.size(); ++w)
			{
				for (size_t r = 0; r < train_x.count_rows(); ++r)
				{
					// bias / intercept
					if (w == weight_adjustments.size() - 1)
					{
						weight_adjustments[w] += (loss_deriv(train_y(r, 0), predict(weights, train_x.get_row(r))) * learning_rate) / train_x.count_rows();
					}
					else
					{
						weight_adjustments[w] += (loss_deriv(train_y(r, 0), predict(weights, train_x.get_row(r))) * learning_rate * train_x(r, w)) / train_x.count_rows();
					}
				}
			}

			// adjust weights
			for (size_t w = 0; w < weights.size(); ++w)
			{
				weights[w] -= weight_adjustments[w];
			}

			iter += 1;
		}

		is_fitted = true;
	}

	DataSet<double> predict(DataSet<double> &input_x) override
	{
		DataSet<double> prediction_data;

		if (!is_fitted)
		{
			throw std::logic_error("Please fit() your model before calling predict()!");
		}

		std::vector<double> predictions;
		// allocate size
		predictions.resize(input_x.count_rows());

		for (size_t current_row = 0; current_row < input_x.count_rows(); ++current_row)
		{
			// predict() in this case is the private function...no recursion here
			predictions[current_row] = predict(weights, input_x.get_row(current_row));
		}

		prediction_data.resize(predictions.size(), 1);
		prediction_data.set_column(0, predictions);
		std::vector<std::string> col_name = {"predicted_y"};
		prediction_data.set_column_names(col_name);

		return prediction_data;
	}

	DataSet<double> get_weights()
	{
		DataSet<double> weights_data;
		weights_data.resize(1, weights.size());
		weights_data.set_row(0, weights);
		std::vector<std::string> col_names;
		for (size_t i = 0; i < weights.size(); ++i)
		{
			if (i == weights.size() - 1) { col_names.push_back("Intercept"); }
			else
			{
				col_names.push_back(this->independent_variable_names[i]);
			}
		}
		weights_data.set_column_names(col_names);
		return weights_data;
	}

	std::vector<double> k_fold_cv(DataSet<double> xdata, DataSet<double> ydata, size_t k = 10, double test_ratio = 0.3)
	{
		return Regressor::k_fold_cv<LinearRegression>(xdata, ydata, k, test_ratio);
	}
};
#endif
