#ifndef LINEARREGRESSION_H
#define LINEARREGRESSION_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>
class LinearRegression {
	private:
		bool is_fitted = false;
		std::vector<double> weights;
		
		double loss(double actual_y, double predicted_y) {
			// mean squared error
			return (actual_y - predicted_y) * (actual_y - predicted_y);
		}
		
		double loss_deriv(double actual_y, double predicted_y) {
			// using limit derivatives from calculus one
			double loss_derivative;
			try {
				loss_derivative = (loss(actual_y, predicted_y + 0.0001) - loss(actual_y, predicted_y)) / 0.0001;
			} catch(int e) {
				loss_derivative = 0;
			}

			return loss_derivative;

		}

		double predict(std::vector<double> weights, std::vector<double> input_x_row) {
			
			double result = 0;

			for (int i = 0; i < weights.size() - 1; ++i) {
				result += weights[i]*input_x_row[i];
			}

			// bias term
			result += weights[weights.size() - 1];
			return result;
		}
	
	public:
		void fit(std::vector<std::vector<double>> train_x, std::vector<double> train_y, int max_iter = 10000, double learning_rate = 0.001) {
			// create initial weights equal to size of input columns (# of indep vars)
			for (int col = 0; col < train_x[0].size(); ++col) {
				weights.push_back(0);
			}	

			weights.push_back(0); // add bias term at the end
			
			int iter = 0;
			double prediction, loss_value, loss_derivative;
			int print_loss_iter = max_iter / 10; // print loss 10 times total during fit
			while (iter < max_iter) {
				loss_value = 0;
				
				for (int currentRow = 0; currentRow < train_x.size(); ++currentRow) {
					prediction = predict(weights, train_x[currentRow]);
					loss_value += loss(train_y[currentRow], prediction);
					loss_derivative = loss_deriv(train_y[currentRow], prediction);
					
					weights[weights.size() - 1] -= learning_rate*loss_derivative; // bias term
					for (int i = 0; i < weights.size() - 1; ++i) {
						weights[i] -= learning_rate*loss_derivative*train_x[currentRow][i];
					}
				}

				if (iter % print_loss_iter == 0) {
					std::cout << "Total loss at iteration #" << iter << ": " << loss_value << std::endl;
				}

				iter += 1;	
			}

			is_fitted = true;	
		}

		std::vector<double> predict(std::vector<std::vector<double>> input_x) {
			if (!is_fitted) {
				throw std::logic_error("Please fit() your model before calling predict()!");
			}

			std::vector<double> predictions;
			// allocate size
			predictions.resize(input_x.size());

			for (int currentRow = 0; currentRow < input_x.size(); ++currentRow) {
				// predict() in this case is the private function...no recursion here
				predictions[currentRow] = predict(weights, input_x[currentRow]);
			}

			return predictions;
		}

		std::vector<double> get_weights() {
			return weights;
		}

		void print_weights(char style = 'h') {
			// h = horizontally
			// v = vertically
			std::cout << "Weights:" << std::endl;
			for (int i = 0; i < weights.size(); ++i) {
				if (style == 'h') {
					std::cout << weights[i] << " ";
				} else if (style == 'v') {
					std::cout << weights[i] << std::endl;
				} else {
					throw std::invalid_argument("'style' argument in print_weights() only accespts 'h' or 'v' as valid inputs.");
				}
			}
			std::cout << std::endl;
		}

		// root mean squared error
		double get_rmse(std::vector<double> actual_y, std::vector<double> predicted_y) {
			double sum = 0;
			for (int i = 0; i < actual_y.size(); ++i) {
				sum += (actual_y[i] - predicted_y[i]) * (actual_y[i] - predicted_y[i]);
			}

			return sqrt(sum / actual_y.size());
		}

		void print_predictions(std::vector<std::vector<double>> input_x, char style = 'h') {
			// h = horizontally
			// v = vertically
			std::vector<double> predictions = predict(input_x);
			std::cout << "Predictions: " << std::endl;
			for (int i = 0; i < predictions.size(); ++i) {
				if (style == 'h') {
					std::cout << predictions[i] << " ";
				}
				else if (style == 'v') {
					std::cout << predictions[i] << std::endl;
				}
				else {
					throw std::invalid_argument("'style' argument in print_predictions() only accepts 'h' or 'v' as valid inputs.");
				}
			}
			std::cout << std::endl;
		}



};
#endif
