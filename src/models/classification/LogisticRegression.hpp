#ifndef LOGISTICREGRESSION_HPP
#define LOGISTICREGRESSION_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>

#include "../../lib/Classifier.hpp"
#include "../../data/DataSet.hpp"

class LogisticRegression : public Classifier
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
		// categorical cross entropy
		// using 0.01 to "soften" the inside of the logarithm to avoid zeroes
		return -actual_y*log2(predicted_y + 0.01) - (1 - actual_y)*log2(1 - predicted_y + 0.01);
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

	double sigmoid(double x)
	{
		return 1 / (1 + exp(-x));
	}

	// local predict, not to be confused with the public predict()
	double m_predict(std::vector<double> &weights, std::vector<double> input_x_row);

public:
	/*
	* max_iter - maximum iterations to run gradient descent algorithm
	* learning_rate - softening parameter to reduce jumping around the loss function
	* loss_func - optional loss function that can be passed by reference from user if they want to specify their own loss function
	*/
	LogisticRegression(size_t max_iter = 1000, double learning_rate = 0.01, double (*loss_func)(double, double) = nullptr)
	: max_iter{max_iter}, learning_rate{learning_rate} 
	{
		if (loss_func != NULL) { user_loss_func = loss_func; }
	}

	void fit(DataSet<double> &train_x, DataSet<size_t> &train_y) override
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
						weight_adjustments[w] += (loss_deriv(train_y(r, 0), m_predict(weights, train_x.get_row(r))) * learning_rate) / train_x.count_rows();
					}
					else
					{
						weight_adjustments[w] += (loss_deriv(train_y(r, 0), m_predict(weights, train_x.get_row(r))) * learning_rate * train_x(r, w)) / train_x.count_rows();
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

	DataSet<size_t> predict(DataSet<double> &input_x);

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

	std::vector<double> monte_carlo_cv(DataSet<double> xdata, DataSet<size_t> ydata, size_t k = 3, double test_ratio = 0.3);
};
#endif
