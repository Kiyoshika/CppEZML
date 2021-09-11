#ifndef DECISIONTREE_HPP
#define DECISIONTREE_HPP
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdexcept>
#include <memory>

#include "../../lib/Classifier.hpp"

class Node
{
public:
	std::shared_ptr<Node> left;
	std::shared_ptr<Node> right;

	DataSet<double> data;
	std::vector<size_t> labels;
	int feature_index;
	double split_point;

	// majority vote prediction
	int predict_label()
	{
		std::vector<size_t>::iterator result;
		result = std::max_element(labels.begin(), labels.end());
		int result_index = std::distance(labels.begin(), result);

		return labels[result_index];
	}

	std::vector<size_t> get_unique_labels(std::vector<size_t> &labels_vec)
	{
		std::vector<size_t> labels_copy = labels_vec;
		std::sort(labels_copy.begin(), labels_copy.end());
        labels_copy.erase(std::unique(labels_copy.begin(), labels_copy.end()), labels_copy.end());

		return labels_copy;
	}
};


class DecisionTree : public Classifier
{
private:
	// hyperparameters
	size_t max_depth = 1000;
	size_t min_samples_split = 2;
	std::vector<size_t> *categorical_columns = nullptr;

	Node root_node; // a copy of the root node so we can deallocate the pointer later on

	// utility functions
	std::vector<size_t> get_unique_labels(std::vector<size_t> labels)
	{
		std::vector<size_t>::iterator ip;
		ip = std::unique(labels.begin(), labels.end());
		labels.resize(std::distance(labels.begin(), ip));

		return labels;
	}

	double get_class_proportion(size_t target_class, std::vector<size_t> const &class_labels)
	{
		double sum = 0;
		for (int i = 0; i < class_labels.size(); ++i)
		{
			if (class_labels[i] == target_class)
			{
				sum += 1;
			}
		}

		return sum / class_labels.size();
	}

	double entropy(std::vector<size_t> class_labels)
	{
		// keep original class labels
		std::vector<size_t> original_class_labels = class_labels;

		// get unique class labels
		std::vector<size_t>::iterator ip;
		std::sort(class_labels.begin(), class_labels.end());
		ip = std::unique(class_labels.begin(), class_labels.end());
		class_labels.resize(std::distance(class_labels.begin(), ip));

		// iterate over unique classes, calculate proportion and finally entropy
		double entropy_sum = 0;
		double proportion;
		for (size_t i = 0; i < class_labels.size(); ++i)
		{
			proportion = get_class_proportion(class_labels[i], original_class_labels);
			entropy_sum -= proportion * log2(proportion);
		}

		return entropy_sum;
	}

	double column_median(size_t column_index, DataSet<double> &data)
	{
		// tranpose column into row
		std::vector<double> row_data = data.get_column(column_index);

		// sort and compute median
		double median;
		double mid_point = floor(row_data.size() / 2);
		std::sort(row_data.begin(), row_data.end());
		if (row_data.size() % 2 == 0)
		{
			median = 0.5 * (row_data[mid_point] + row_data[mid_point - 1]);
		}
		else
		{
			median = row_data[mid_point];
		}

		return median;
	}

	std::vector<double> split_feature(
		size_t feature_index, 
		DataSet<double> &data, 
		std::vector<size_t> labels, 
		char feature_type
	)
	{
		std::vector<size_t> left_labels;
		std::vector<size_t> right_labels;
		double split_point;
		double split_entropy;
		// feature_type = n is numeric
		// feature type = c is categorical
		if (feature_type == 'n')
		{
			// numeric features will split on median
			split_point = column_median(feature_index, data);
			for (size_t i = 0; i < data.count_rows(); ++i)
			{
				if (data(i, feature_index) <= split_point)
				{
					left_labels.push_back(labels[i]);
				}
				else
				{
					right_labels.push_back(labels[i]);
				}
			}

			split_entropy = ((double)left_labels.size() / (double)data.count_rows()) * entropy(left_labels) + ((double)right_labels.size() / (double)data.count_rows()) * entropy(right_labels);
		}
		else if (feature_type == 'c')
		{
			std::vector<size_t> temp_left_labels; // going to be resuing these until we find best category label to split on
			std::vector<size_t> temp_right_labels;

			// iterate over every unique label and select one that returns minimum entropy (to maximize information gain)
			// first, transpose column vector into integer row vector and call get_unique_labels
			std::vector<size_t> unique_category_labels;
			for (size_t i = 0; i < data.count_rows(); ++i)
			{
				unique_category_labels.push_back((size_t)data(i, feature_index));
			}

			unique_category_labels = get_unique_labels(unique_category_labels);
			std::vector<double> category_entropy_list;
			double temp_entropy;

			for (size_t i = 0; i < unique_category_labels.size(); ++i)
			{
				for (size_t r = 0; r < data.count_rows(); ++r)
				{
					if ((size_t)data(r, feature_index) == unique_category_labels[i])
					{
						temp_left_labels.push_back(labels[r]);
					}
					else
					{
						temp_right_labels.push_back(labels[r]);
					}
				}

				temp_entropy = ((double)left_labels.size() / (double)data.count_rows()) * entropy(left_labels) + ((double)right_labels.size() / (double)data.count_rows()) * entropy(right_labels);
				category_entropy_list.push_back(temp_entropy);
				// empty vectors to reuse on next iteration
				temp_left_labels.clear();
				temp_right_labels.clear();
			}

			// find which category contained best (minimum) entropy
			int best_index = -1;
			double current_entropy = -1;
			for (size_t i = 0; i < category_entropy_list.size(); ++i)
			{
				if (category_entropy_list[i] > current_entropy)
				{
					best_index = i;
					current_entropy = category_entropy_list[i];
				}
			}

			// finally, set the split point and entropy
			split_entropy = current_entropy;
			split_point = unique_category_labels[best_index];
		}

		std::vector<double> split_feature_data;
		split_feature_data.push_back(split_entropy);
		split_feature_data.push_back(split_point);

		// return data about the feature split such as the entropy and split point
		return split_feature_data;
	}

	void grow_tree(
		std::shared_ptr<Node> tree_node,
		DataSet<double> &data,
		std::vector<size_t> labels,
		size_t max_depth,
		size_t current_depth,
		size_t min_samples_split,
		std::vector<size_t> *categorical_columns)
	{

		if (current_depth > max_depth)
		{
			throw std::runtime_error("Current tree depth is greater than maximum allowed depth!");
		}

		double root_entropy = entropy(labels);

		size_t feature_to_split = 0;
		double max_info_gain = -1.0;
		// iterate over every feature and grow the tree
		double info_gain;
		double split_point;
		std::vector<double> feature_split;

		for (size_t i = 0; i < data.count_columns(); ++i)
		{

			if (categorical_columns == NULL || categorical_columns != NULL && std::find(categorical_columns->begin(), categorical_columns->end(), i) == categorical_columns->end())
			{
				feature_split = split_feature(i, data, labels, 'n');
			}
			else if (
				categorical_columns != NULL && std::find(categorical_columns->begin(), categorical_columns->end(), i) != categorical_columns->end())
			{
				feature_split = split_feature(i, data, labels, 'c');
			}
			else
			{
				throw std::invalid_argument("There was a problem trying to determine if your data was categorical or not. Please check your categorical_columns parameter.");
			}
			info_gain = root_entropy - feature_split[0]; // zeroth index contains the entropy from the split
			if (info_gain > max_info_gain)
			{
				max_info_gain = info_gain;
				feature_to_split = i;
				split_point = feature_split[1]; // first index contains the split point of the feature
			}
		}

		// if we gain no further information, break out of recursion by setting current depth greater than max_depth
		if (max_info_gain == 0)
		{
			current_depth = max_depth + 1;
		}

		DataSet<double> left_data;
		std::vector<size_t> left_labels;

		DataSet<double> right_data;
		std::vector<size_t> right_labels;

		size_t left_data_size = 0, right_data_size = 0;

		// pass through to resize data sets appropriately
		for (size_t i = 0; i < data.count_rows(); ++i)
		{
			if (data(i, feature_to_split) <= split_point)
			{
				left_data_size++;
			}
			else
			{
				right_data_size++;
			}
		}

		left_data.resize(left_data_size, data.count_columns());
		right_data.resize(right_data_size, data.count_columns());

		std::vector<size_t> labels_vec = labels;

		// a map used to track the current indices when setting data
		// for left/right nodes

		// key = type (left, right), value = current index
		std::unordered_map<size_t, size_t> left_right_indices = {
			{0, 0}, // left = 0 by default
			{1, 0} // right = 0 by default
		};

		// add data on second pass through
		for (size_t i = 0; i < data.count_rows(); ++i)
		{
			if (data(i, feature_to_split) <= split_point)
			{
				left_data.set_row(left_right_indices[0], data.get_row(i));
				left_labels.push_back(labels_vec[i]);
				left_right_indices[0]++;
			}
			else
			{
				right_data.set_row(left_right_indices[1], data.get_row(i));
				right_labels.push_back(labels_vec[i]);
				left_right_indices[1]++;
			}
		}

		tree_node->split_point = split_point;
		tree_node->feature_index = feature_to_split;

		tree_node->left = std::make_unique<Node>();
		tree_node->left->data = left_data;
		tree_node->left->labels = left_labels;

		tree_node->right = std::make_unique<Node>();
		tree_node->right->data = right_data;
		tree_node->right->labels = right_labels;

		std::vector<size_t> left_unique = get_unique_labels(left_labels);
		std::vector<size_t> right_unique = get_unique_labels(right_labels);

		if (current_depth < max_depth)
		{
			// grow tree in both directions
			// must have at least two samples to split (could be controlled by parameter)
			// must have more than one unique label (otherwise it's a perfect leaf node)
			if (tree_node->left->data.count_rows() > (min_samples_split - 1) && left_unique.size() > 1)
			{
				grow_tree(tree_node->left, left_data, left_labels, max_depth, current_depth + 1, min_samples_split, categorical_columns);
			}

			if (tree_node->right->data.count_rows() > (min_samples_split - 1) && right_unique.size() > 1)
			{
				grow_tree(tree_node->right, right_data, right_labels, max_depth, current_depth + 1, min_samples_split, categorical_columns);
			}
		}
	}

	// traversing tree when calling predict()
	size_t predict_down(std::shared_ptr<Node> tree_node, std::vector<double> data)
	{
		if (tree_node->left == NULL && tree_node->right == NULL)
		{
			return tree_node->predict_label();
		}
		else if (data[tree_node->feature_index] <= tree_node->split_point)
		{
			return predict_down(tree_node->left, data);
		}
		else if (data[tree_node->feature_index] > tree_node->split_point)
		{
			return predict_down(tree_node->right, data);
		}

		// technically unreachable but required
		return 0;
	}

	/*void dealloc_tree(Node *current_node)
	{
		if (current_node != NULL)
		{
			dealloc_tree(current_node->left);
			dealloc_tree(current_node->right);
			delete current_node;
		}
	}*/

public:
	DecisionTree(size_t max_depth = 1000, size_t min_samples_split = 2, std::vector<size_t> *categorical_columns = nullptr) 
	: max_depth{max_depth}, min_samples_split{min_samples_split}, categorical_columns{categorical_columns} {}

	std::shared_ptr<Node> root = std::make_unique<Node>();

	// free the memory after it's used to make predictions
	~DecisionTree()
	{
		//dealloc_tree(root);
	}

	/*int get_tree_depth(Node *root)
	{
		if (root == NULL)
		{
			return 0;
		}
		else
		{
			int left_height = get_tree_depth(root->left);
			int right_height = get_tree_depth(root->right);
			if (left_height >= right_height)
			{
				return left_height + 1;
			}
			else
			{
				return right_height + 1;
			}
		}
	}*/

	void fit(
		DataSet<double> &data, 
		DataSet<size_t> &labels
		) override
	{
		if (min_samples_split < 2)
		{
			throw std::invalid_argument("Please set min_samples_split parameter to at least 2.");
		}

		grow_tree(root, data, labels.get_column(0), max_depth, 1, min_samples_split, categorical_columns);
	}

	DataSet<size_t> predict(DataSet<double> &data) override
	{

		std::vector<size_t> predictions;

		// start at root node and traverse
		for (size_t i = 0; i < data.count_rows(); ++i)
		{
			predictions.push_back(predict_down(root, data.get_row(i)));
		}

		DataSet<size_t> predictions_data(predictions.size(), 1);
		predictions_data.set_column(0, predictions);
		std::vector<std::string> col_name = {"predicted_y"};
		predictions_data.set_column_names(col_name);

		return predictions_data;
	}
};

#endif
