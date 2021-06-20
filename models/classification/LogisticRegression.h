#ifndef LOGISTICREGRESSION_H
#define LOGISTICREGRESSION_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>

#include "../../lib/Classifier.h"

class LogisticRegression : public Classifier
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
		// cross entropy
		return -actual_y*log2(predicted_y) - (1 - actual_y)*log2(1 - predicted_y);
	}

	double loss_deriv(double actual_y, std::vector<double> const &train_x_row, int train_y_row)
	{
		// using limit derivatives from calculus one
		double loss_derivative;
		try
		{
			if (user_loss_func != NULL)
			{
				loss_derivative = loss_derivative = (user_loss_func(train_y_row, evaluate(this->weights, train_x_row, 0.0001)) - user_loss_func(train_y_row, evaluate(this->weights, train_x_row))) / 0.0001;
			} else 
			{
				loss_derivative = loss_derivative = (loss(train_y_row, evaluate(this->weights, train_x_row, 0.0001)) - loss(train_y_row, evaluate(this->weights, train_x_row))) / 0.0001;
			}
		}
		catch (int e)
		{
			loss_derivative = 0;
		}

		return loss_derivative;
	}

	// evaluate models output
	double evaluate(std::vector<double> const &weights, std::vector<double> const &input_x_row, double adjust = 0.0)
	{

		double result = 0;

		for (int i = 0; i < weights.size() - 1; ++i)
		{
			result += weights[i] * input_x_row[i];
		}

		// bias term
		result += weights[weights.size() - 1];
		return sigmoid(result + adjust);
	}

    double sigmoid(double x) {
        double e = 2.71828;
        return 1 / (1 + pow(e, -x));
    }

public:
	/*
	* verbose - prints loss occasionally
	* max_iter - maximum iterations to run gradient descent algorithm
	* learning_rate - softening parameter to reduce jumping around the loss function
	* loss_func - optional loss function that can be passed by reference from user if they want to specify their own loss function
	*/
	LogisticRegression(bool verbose = false, int max_iter = 1000, double learning_rate = 0.001, double (*loss_func)(double, double) = nullptr)
	: verbose{verbose}, max_iter{max_iter}, learning_rate{learning_rate} 
	{
		if (loss_func != NULL) { user_loss_func = loss_func; }
	}

	void fit(std::vector<std::vector<double>> const &train_x, std::vector<int> const &train_y) override
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
				prediction = evaluate(weights, train_x[current_row]);
				if (user_loss_func != NULL) 
				{
					loss_value += user_loss_func(train_y[current_row], prediction);
				} else 
				{
					loss_value += loss(train_y[current_row], prediction);
				}

                previous_loss = loss_value;
				loss_derivative = loss_deriv(train_y[current_row], train_x[current_row], train_y[current_row]);

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

	std::vector<int> predict(std::vector<std::vector<double>> const &input_x) override
	{
		if (!is_fitted)
		{
			throw std::logic_error("Please fit() your model before calling predict()!");
		}

		std::vector<int> predictions;
		// allocate size
		predictions.resize(input_x.size());
        double pred;
		for (int current_row = 0; current_row < input_x.size(); ++current_row)
		{
            pred = evaluate(weights, input_x[current_row]);
            if (pred > 0.5) { predictions[current_row] = 1; }
            else { predictions[current_row] = 0; }
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
		std::vector<int> predictions = predict(input_x);
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
