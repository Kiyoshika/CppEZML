#ifndef LINEARREGRESSION_HPP
#define LINEARREGRESSION_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>

#include "../../lib/Regressor.hpp"

class LinearRegression : public Regressor
{
private:
	// hyperparameters
	bool verbose = false;
	int max_iter = 1000;
	double learning_rate = 0.001;
	double (*user_loss_func)(double, double) = nullptr;

	bool is_fitted = false;
	std::vector<double> weights;

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
	double predict(std::vector<double> const &weights, std::vector<double> const &input_x_row)
	{

		double result = 0;

		for (int i = 0; i < weights.size() - 1; ++i)
		{
			result += weights[i] * input_x_row[i];
		}

		// bias term
		result += weights[weights.size() - 1];
		return result;
	}

public:
	/*
	* verbose - prints loss occasionally
	* max_iter - maximum iterations to run gradient descent algorithm
	* learning_rate - softening parameter to reduce jumping around the loss function
	* loss_func - optional loss function that can be passed by reference from user if they want to specify their own loss function
	*/
	LinearRegression(bool verbose = false, int max_iter = 1000, double learning_rate = 0.001, double (*loss_func)(double, double) = nullptr)
	: verbose{verbose}, max_iter{max_iter}, learning_rate{learning_rate} 
	{
		if (loss_func != NULL) { user_loss_func = loss_func; }
	}

	void fit(std::vector<std::vector<double>> const &train_x, std::vector<double> const &train_y) override
	{
		// create initial weights equal to size of input columns (# of indep vars)
		for (int col = 0; col < train_x[0].size(); ++col)
		{
			weights.push_back(0);
		}

		weights.push_back(0); // add bias term at the end

		int iter = 0;
		double prediction, loss_value, loss_derivative;
		int print_loss_iter = max_iter / 10; // print loss 10 times total during fit

		// if loss hasn't (significantly) changed within 10 iterations, stop the algorithm
        double previous_loss;
        int no_change_counter = 0;
		while (iter < max_iter)
		{
			loss_value = 0;

			for (int current_row = 0; current_row < train_x.size(); ++current_row)
			{
				prediction = predict(weights, train_x[current_row]);
				if (user_loss_func != NULL) 
				{
					loss_value += user_loss_func(train_y[current_row], prediction);
				} else 
				{
					loss_value += loss(train_y[current_row], prediction);
				}
				loss_derivative = loss_deriv(train_y[current_row], prediction);

				weights[weights.size() - 1] -= learning_rate * loss_derivative; // bias term
				for (int i = 0; i < weights.size() - 1; ++i)
				{
					weights[i] -= learning_rate * loss_derivative * train_x[current_row][i];
				}
			}

			if (verbose)
			{
				if (iter % print_loss_iter == 0)
				{
					std::cout << "Total loss at iteration #" << iter << ": " << loss_value << std::endl;
				}	
			}

			// early stopping
            if (iter > 0) {

                if (loss_value - previous_loss <= 0.001) {
                    no_change_counter += 1;
                } else {
                    no_change_counter = 0;
                }

                previous_loss = loss_value;
            }

            if (no_change_counter == 100) {
                break;
            }

			iter += 1;
		}

		is_fitted = true;
	}

	std::vector<double> predict(std::vector<std::vector<double>> const &input_x) override
	{
		if (!is_fitted)
		{
			throw std::logic_error("Please fit() your model before calling predict()!");
		}

		std::vector<double> predictions;
		// allocate size
		predictions.resize(input_x.size());

		for (int current_row = 0; current_row < input_x.size(); ++current_row)
		{
			// predict() in this case is the private function...no recursion here
			predictions[current_row] = predict(weights, input_x[current_row]);
		}

		return predictions;
	}

	std::vector<double> get_weights()
	{
		return weights;
	}

	void print_weights(char style = 'h')
	{
		// h = horizontally
		// v = vertically
		std::cout << "Weights:" << std::endl;
		for (int i = 0; i < weights.size(); ++i)
		{
			if (style == 'h')
			{
				std::cout << weights[i] << " ";
			}
			else if (style == 'v')
			{
				std::cout << weights[i] << std::endl;
			}
			else
			{
				throw std::invalid_argument("'style' argument in print_weights() only accespts 'h' or 'v' as valid inputs.");
			}
		}
		std::cout << std::endl;
	}

	void print_predictions(std::vector<std::vector<double>> const &input_x, char style = 'h')
	{
		// h = horizontally
		// v = vertically
		std::vector<double> predictions = predict(input_x);
		std::cout << "Predictions: " << std::endl;
		for (int i = 0; i < predictions.size(); ++i)
		{
			if (style == 'h')
			{
				std::cout << predictions[i] << " ";
			}
			else if (style == 'v')
			{
				std::cout << predictions[i] << std::endl;
			}
			else
			{
				throw std::invalid_argument("'style' argument in print_predictions() only accepts 'h' or 'v' as valid inputs.");
			}
		}
		std::cout << std::endl;
	}
};
#endif
