#ifndef DATASET_H
#define DATASET_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <type_traits>

#include "../lib/FilterData.h"
#include "../lib/SortData.h"
#include "../stats/Stats.h"

class DataSet
{
private:
	bool has_headers = true;

	std::vector<std::string> get_unique_columns(std::vector<std::string> const &columns)
	{
		std::vector<std::string> columns_copy = columns;
		std::sort(columns_copy.begin(), columns_copy.end());
		columns_copy.erase(std::unique(columns_copy.begin(), columns_copy.end()), columns_copy.end());

		return columns_copy;
	}

	// load rows into data matrix
	void split(std::string text, int current_row, std::string sep = ",")
	{
		bool inside_string = false;
		int start_index = 0;

		for (int i = 0; i < text.length(); i++)
		{
			if ((text.substr(i, sep.length()).compare(sep)) == 0 && !inside_string)
			{
				data[current_row].push_back(text.substr(start_index, i - start_index));
				start_index = i + sep.length();
			}

			if (i == text.length() - 1)
			{
				data[current_row].push_back(text.substr(start_index, text.length()));
			}

			// check if we're inside quotes
			if (text[i] == '\"' && !inside_string)
			{
				inside_string = true;
			}
			else if (text[i] == '\"' && inside_string)
			{
				inside_string = false;
			}
		}
	}

	// load headers (if exists) into columns vector
	void split(std::string text, std::string sep = ",")
	{
		bool inside_string = false;
		int start_index = 0;

		for (int i = 0; i < text.length(); i++)
		{
			if ((text.substr(i, sep.length()).compare(sep)) == 0 && !inside_string)
			{
				columns.push_back(text.substr(start_index, i - start_index));
				start_index = i + sep.length();
			}

			if (i == text.length() - 1)
			{
				columns.push_back(text.substr(start_index, text.length()));
			}

			// check if we're inside quotes
			if (text[i] == '\"' && !inside_string)
			{
				inside_string = true;
			}
			else if (text[i] == '\"' && inside_string)
			{
				inside_string = false;
			}
		}

		if (get_unique_columns(columns).size() != columns.size())
		{
			throw std::runtime_error("Columns must be uniquely named when loading");
		}
	}

	void print_describe_line(double value)
	{
		std::string num_string, cutoff_str;

		num_string = std::to_string(value);
		if (num_string.length() < 15)
		{
			std::cout << num_string << std::setfill(' ') << std::setw(15 - num_string.length());
		}
		else if (num_string.length() == 15)
		{
			std::cout << num_string;
		}
		else
		{
			cutoff_str = num_string;
			cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
			std::cout << cutoff_str;
		}
		std::cout << "\t";
	}

public:
	std::vector<std::vector<std::string>> data;
	std::vector<std::string> columns;

	// load from file
	void load(std::string filepath, std::string sep = ",", bool has_headers = true)
	{
		this->has_headers = has_headers;

		std::string current_line;
		int current_row = 0;

		try
		{
			ifstream datafile(filepath);

			// check if file exists
			if (datafile.fail())
			{
				throw std::runtime_error("There was a problem loading your data!\nCheck your directory/filename.");
			}

			// read data
			if (datafile.is_open())
			{
				while (getline(datafile, current_line))
				{
					if (has_headers)
					{
						// populate columns vector (if headers exist)
						split(current_line, sep);
						has_headers = false;
					}
					else
					{
						// populate data matrix
						data.push_back(std::vector<std::string>()); // create rows
						split(current_line, current_row, sep);
						current_row += 1;
					}
				}
			}
			datafile.close();
		}
		catch (int e)
		{
		}
	}

	// load from another data set (with columns)
	void load(std::vector<std::vector<std::string>> data, std::vector<std::string> columns)
	{
		this->data = data;
		this->columns = columns;
	}

	// load from another 1D data set with columns (will be translated into row vector)
	void load(std::vector<int> data, std::vector<std::string> columns) // overload 1 (int)
	{
		std::vector<std::vector<std::string>> new_data;
		new_data.push_back(std::vector<std::string>());

		for (int i = 0; i < data.size(); ++i)
		{
			new_data[0].push_back(std::to_string(data[i]));
		}


		this->data = new_data;
		this->columns = columns;
	}

	void load(std::vector<double> data, std::vector<std::string> columns) // overload 2 (double)
	{
		std::vector<std::vector<std::string>> new_data;
		new_data.push_back(std::vector<std::string>());

		for (int i = 0; i < data.size(); ++i)
		{
			new_data[0].push_back(std::to_string(data[i]));
		}


		this->data = new_data;
		this->columns = columns;
	}

	void load(std::vector<std::string> data, std::vector<std::string> columns) // overload 3 (string)
	{
		std::vector<std::vector<std::string>> new_data;
		new_data.push_back(std::vector<std::string>());

		for (int i = 0; i < data.size(); ++i)
		{
			new_data[0].push_back(data[i]);
		}


		this->data = new_data;
		this->columns = columns;
	}

	// load from another 1D data set with columns (will be translated into column vector)
	void load(std::vector<int> data, std::string column_name) // overload 1 (int)
	{
		std::vector<std::vector<std::string>> new_data;
		for (int i = 0; i < data.size(); ++i)
		{
			new_data.push_back(std::vector<std::string>());
			new_data[i].push_back(std::to_string(data[i]));
		}

		// create default column
		std::vector<std::string> new_column = { column_name };
		this->data = new_data;
		this->columns = new_column;
	}

	void load(std::vector<double> data, std::string column_name) // overload 2 (double)
	{
		std::vector<std::vector<std::string>> new_data;
		for (int i = 0; i < data.size(); ++i)
		{
			new_data.push_back(std::vector<std::string>());
			new_data[i].push_back(std::to_string(data[i]));
		}

		// create default column
		std::vector<std::string> new_column = { column_name };
		this->data = new_data;
		this->columns = new_column;
	}

	void load(std::vector<std::string> data, std::string column_name) // overload 3 (string)
	{
		std::vector<std::vector<std::string>> new_data;
		for (int i = 0; i < data.size(); ++i)
		{
			new_data.push_back(std::vector<std::string>());
			new_data[i].push_back(data[i]);
		}

		// create default column
		std::vector<std::string> new_column = { column_name };
		this->data = new_data;
		this->columns = new_column;
	}

	void head(int rows = 10)
	{
		std::string cutoff_str;
		// if column vector exists, print columns before data
		if (columns.size() > 0)
		{
			for (int c = 0; c < columns.size(); ++c)
			{

				//std::cout << columns[c] << "\t\t";
				if (columns[c].length() < 15)
				{
					std::cout << columns[c] << std::setfill(' ') << std::setw(15 - columns[c].length());
				}
				else if (columns[c].length() == 15)
				{
					std::cout << columns[c];
				}
				else
				{
					cutoff_str = columns[c];
					cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
					std::cout << cutoff_str;
				}

				std::cout << "\t";
			}
			std::cout << "\n";
			std::cout << std::setfill('-') << std::setw(15 * columns.size());
		}
		std::cout << "\n";

		// print #[rows] of data
		for (int r = 0; r < rows; ++r)
		{
			if (r >= data.size())
			{
				break;
			} // check if rows is larger than our data set
			else
			{
				for (int c = 0; c < data[0].size(); ++c)
				{
					//std::cout << data[r][c] << "\t\t";
					//std::cout << columns[c] << "\t\t";
					if (data[r][c].length() < 15)
					{
						std::cout << data[r][c] << std::setfill(' ') << std::setw(15 - data[r][c].length());
					}
					else if (data[r][c].length() == 15)
					{
						std::cout << data[r][c];
					}
					else
					{
						cutoff_str = data[r][c];
						cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
						std::cout << cutoff_str;
					}

					std::cout << "\t";
				}
				std::cout << endl;
			}
		}
	}

	// select subset of original data set by index (creates new data set)
	DataSet select(std::vector<int> indices)
	{
		int new_size = indices.size();

		std::vector<std::string> new_columns;

		if (has_headers)
		{
			for (int i = 0; i < new_size; ++i)
			{
				new_columns.push_back(columns[indices[i]]);
			}
		}

		std::vector<std::vector<std::string>> new_data;

		for (int r = 0; r < data.size() - 1; ++r)
		{
			new_data.push_back(std::vector<std::string>());
			for (int c = 0; c < new_size; ++c)
			{
				new_data[r].push_back(data[r][indices[c]]);
			}
		}

		DataSet subset;
		subset.load(new_data, new_columns);

		return subset;
	}

	// select subset of original data set by name (creates new data set)
	DataSet select(std::vector<std::string> names)
	{
		int new_size = names.size();

		// create map of indices from original column names to positions
		unordered_map<std::string, int> name_index;
		for (int i = 0; i < columns.size(); ++i)
		{
			name_index.insert(pair<std::string, int>(columns[i], i));
		}

		std::vector<std::string> new_columns;
		for (int i = 0; i < new_size; ++i)
		{
			new_columns.push_back(names[i]);
		}

		std::vector<std::vector<std::string>> new_data;

		for (int r = 0; r < data.size() - 1; ++r)
		{
			new_data.push_back(std::vector<std::string>());
			for (int c = 0; c < new_size; ++c)
			{
				new_data[r].push_back(data[r][name_index[new_columns[c]]]);
			}
		}

		DataSet subset;
		subset.load(new_data, new_columns);

		return subset;
	}

	// return subset of original data filtered by conditions
	DataSet filter(std::vector<std::vector<std::string>> conditions)
	{

		FilterData fd;
		fd.set_columns(this->columns);
		std::vector<std::vector<std::string>> filtered_data = fd.filter(data, conditions);

		DataSet subset;
		subset.load(filtered_data, columns);

		return subset;
	}

	std::vector<int> get_column_indices(std::vector<std::string> column_names)
	{
		// iterate over provided vector of column names and return the indices
		std::vector<int> column_indices;
		for (int i = 0; i < column_names.size(); ++i)
		{
			for (int j = 0; j < columns.size(); ++j)
			{
				if (columns[j].compare(column_names[i]) == 0)
				{
					column_indices.push_back(j);
					break;
				}
			}
		}

		// validate that column indices matches original input
		if (column_indices.size() != column_names.size())
		{
			throw std::invalid_argument("column_names parameter could not find all columns in data set. Please double check your column names.");
		}

		return column_indices;
	}

	// used for "on the fly" conversions e.g when using fit() methods on models and such
	// designed for independent variables
	std::vector<std::vector<double>> cast_data_double()
	{
		std::vector<std::vector<double>> new_data;
		std::vector<double> new_row;

		for (int i = 0; i < this->data.size(); ++i)
		{
			for (int j = 0; j < this->data[i].size(); ++j)
			{
				try
				{
					new_row.push_back(stod(this->data[i][j]));
				}
				catch (int e)
				{
					throw std::runtime_error("Could not convert some of your data to a double!");
				}
			}
			new_data.push_back(new_row);
			new_row.clear();
		}

		return new_data;
	}

	// used for "on the fly" conversions e.g when using fit() methods on models and such
	// designed for dependent variables (e.g classes must be integers, but regression could be doubles)
	std::vector<int> cast_target_int()
	{
		std::vector<int> new_data;

		for (int i = 0; i < this->data.size(); ++i)
		{
			try
			{
				// we're assuming that target is a one-column data set
				new_data.push_back(stoi(this->data[i][0]));
			}
			catch (int e)
			{
				throw std::runtime_error("Could not convert some of your data to an int!");
			}
		}

		return new_data;
	}

	std::vector<double> cast_target_double()
	{
		std::vector<double> new_data;

		for (int i = 0; i < this->data.size(); ++i)
		{
			try
			{
				// we're assuming that target is a one-column data set
				new_data.push_back(stod(this->data[i][0]));
			}
			catch (int e)
			{
				throw std::runtime_error("Could not convert some of your data to a double!");
			}
		}

		return new_data;
	}

	/*
	@param n Number of samples to take (default = 1)
	@param replace Whether to replace a sample after it was drawn (default = false)
	@return A new instance of DataSet representing the sampled data.
	*/
	DataSet sample(int n = 1, bool replace = false)
	{
		if (n < 1)
		{
			throw std::invalid_argument("You must sample at least one element.");
		}
		else if (n > this->data.size() && !replace)
		{
			throw std::invalid_argument("You can't uniquely sample elements larger than your data set.");
		}

		unordered_map<int, int> unique_indices;
		std::vector<int> random_indices;
		int iter = 0;
		int random_index;

		while (iter < n)
		{
			random_index = rand() % (this->data.size() + 1);
			if (!replace && unique_indices.count(random_index) == 0)
			{
				unique_indices[random_index] = 1;
				random_indices.push_back(random_index);
				iter += 1;
			}
			else
			{
				random_indices.push_back(random_index);
				iter += 1;
			}
		}

		// extract rows that were sampled
		std::vector<std::vector<std::string>> sampled_data;
		for (int i = 0; i < n; ++i)
		{
			sampled_data.push_back(this->data[random_indices[i]]);
		}

		DataSet sampled;
		sampled.load(sampled_data, this->columns);

		return sampled;
	}

	/*
	@param other_data DataSet to append
	@param type Append on rows ('r') or columns ('r')
	*/
	DataSet append(DataSet other_data, char type = 'r')
	{
		DataSet appended;

		// making a copy of current data set in order to not mutate it
		std::vector<std::string> columns_copy = this->columns;
		std::vector<std::vector<std::string>> data_copy = this->data;

		if (type == 'r')
		{
			// check if columns match
			if (this->data[0].size() != other_data.data[0].size())
			{
				throw std::runtime_error("Dimensions when appending data sets don't match.");
			}
			for (int i = 0; i < other_data.data.size(); ++i)
			{
				data_copy.push_back(other_data.data[i]);
			}

			appended.load(data_copy, columns_copy);
		}
		else if (type == 'c')
		{
			// check if rows match
			if (this->data.size() != other_data.data.size())
			{
				throw std::runtime_error("Dimensions when appending data sets don't match.");
			}

			// check column collision (all columns need to be uniquely named)
			std::vector<std::string> col_intersection;
			std::sort(columns_copy.begin(), columns_copy.end());
			std::sort(other_data.columns.begin(), other_data.columns.end());
			std::set_intersection(columns_copy.begin(), columns_copy.end(),
								  other_data.columns.begin(), other_data.columns.end(),
								  back_inserter(col_intersection));

			if (col_intersection.size() > 0)
			{
				throw std::runtime_error("Columns must be uniquely named between both data sets when appending.");
			}

			for (int i = 0; i < other_data.columns.size(); ++i)
			{
				columns_copy.push_back(other_data.columns[i]);
			}

			for (int i = 0; i < this->data.size(); ++i)
			{
				for (int j = 0; j < other_data.columns.size(); ++j)
				{
					data_copy[i].push_back(other_data.columns[j]);
				}
			}

			appended.load(data_copy, columns_copy);
		}
		else
		{
			throw std::invalid_argument("Only 'r' (rows) and 'c' (columns) are allowed when appending data.");
		}

		return appended;
	}

	void rename(std::unordered_map<std::string, std::string> new_cols)
	{
		std::unordered_map<string, string>::iterator map_it;
		std::vector<std::string>::iterator vec_it;

		for (map_it = new_cols.begin(); map_it != new_cols.end(); ++map_it)
		{
			vec_it = std::find(this->columns.begin(), this->columns.end(), map_it->first);
			if (vec_it != this->columns.end())
			{
				this->columns[vec_it - this->columns.begin()] = map_it->second;
			}

			// if column in map is not found, ignore it
		}
	}

	std::vector<std::string> transpose(int col_index)
	{
		std::vector<std::string> tposed;
		for (int i = 0; i < this->data.size(); ++i)
		{
			tposed.push_back(this->data[i][col_index]);
		}

		return tposed;
	}

	std::vector<double> transpose_double(int col_index)
	{
		std::vector<std::string> data_tposed = transpose(col_index);
		std::vector<double> data_double(data_tposed.size());
		try
		{
			std::transform(data_tposed.begin(), data_tposed.end(), data_double.begin(), [](const std::string &val)
						   { return std::stod(val); });
		}
		catch (int e)
		{
			std::runtime_error("Couldn't convert your data to double in transpose_double().");
		}

		return data_double;
	}

	void describe()
	{
		double _sum, _mean, _stdev, _min, _10p, _25p, _median, _75p, _90p, _max;
		std::string cutoff_str;
		std::cout << "             |  ";
		for (int c = 0; c < columns.size(); ++c)
		{

			if (columns[c].length() < 15)
			{
				std::cout << columns[c] << std::setfill(' ') << std::setw(15 - columns[c].length());
			}
			else if (columns[c].length() == 15)
			{
				std::cout << columns[c];
			}
			else
			{
				cutoff_str = columns[c];
				cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
				std::cout << cutoff_str;
			}

			std::cout << "\t";
		}
		std::cout << "\n";
		std::cout << std::setfill('-') << std::setw(15 * columns.size() + 15);

		Stats stats;
		std::cout << "\nSum:" << std::setfill(' ') << std::setw(10) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_sum = stats.sum(this->data, c);
			print_describe_line(_sum);
		}

		std::cout << "\nMin:" << std::setfill(' ') << std::setw(10) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_min = stats.min(this->data, c);
			print_describe_line(_min);
		}

		std::cout << "\nMax:" << std::setfill(' ') << std::setw(10) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_max = stats.max(this->data, c);
			print_describe_line(_max);
		}

		std::cout << "\nMean:" << std::setfill(' ') << std::setw(9) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_mean = stats.mean(this->data, c);
			print_describe_line(_mean);
		}

		std::cout << "\nStDev:" << std::setfill(' ') << std::setw(8) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_stdev = stats.stdev(this->data, c);
			print_describe_line(_stdev);
		}

		std::cout << "\n10th %:" << std::setfill(' ') << std::setw(7) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_10p = stats.percentile(this->data, c, 0.1);
			print_describe_line(_10p);
		}

		std::cout << "\n25th %:" << std::setfill(' ') << std::setw(7) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_25p = stats.percentile(this->data, c, 0.25);
			print_describe_line(_25p);
		}

		std::cout << "\nMedian:" << std::setfill(' ') << std::setw(7) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_median = stats.median(this->data, c);
			print_describe_line(_median);
		}

		std::cout << "\n75th %:" << std::setfill(' ') << std::setw(7) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_75p = stats.percentile(this->data, c, 0.75);
			print_describe_line(_75p);
		}

		std::cout << "\n90th %:" << std::setfill(' ') << std::setw(7) << "|"
				  << "\t";
		for (int c = 0; c < columns.size(); ++c)
		{
			_90p = stats.percentile(this->data, c, 0.90);
			print_describe_line(_90p);
		}

		std::cout << "\n";
	}

	void sort(std::vector<std::vector<std::string>> sort_conditions)
	{
		SortData sd;
		sd.set_columns(this->columns);
		sd.sort(this->data, sort_conditions);
	}

	// @param test_ratio The ratio of the test data. Must be in interval (0, 1).
	// @param train The training data set passed by reference.
	// @param test The testing data set passed by reference.
	void split_data(double test_ratio, DataSet *train, DataSet *test)
	{

		int data_size = this->data.size();
		int sample_iter = 0;
		std::unordered_map<int, int> index_map;
		int test_size;
		int rand_index;

		DataSet train_temp, test_temp;

		if (test_ratio > 0 && test_ratio < 1)
		{
			test_size = (int)(test_ratio * data_size);
			// populate test data set
			while (sample_iter < test_size)
			{
				rand_index = rand() % data_size;
				if (index_map.find(rand_index) == index_map.end())
				{
					test_temp.data.push_back(this->data[rand_index]);
					index_map.insert({{rand_index, 1}});
					sample_iter += 1;
				}
			}

			// populate train data set by skipping the indices added from test data
			// probably a better way to go about this, but this is fine for now
			// could create a vector of test indices and remove it from the vector of 0, ..., data_size to get train indices
			for (int i = 0; i < data_size; ++i)
			{
				if (index_map.find(i) == index_map.end())
				{
					train_temp.data.push_back(this->data[i]);
				}
			}

			// override data references with temporary data
			// this is to allow support for calling split_data on the same object over and over
			train->data = train_temp.data;
			test->data = test_temp.data;
		}
		else
		{
			throw std::invalid_argument("test_ratio parameter must be in interval (0, 1).");
		}
	}

	// @param file_name The name of the CSV file (without the .csv extension).
	// @param sep The separator/delimiter of the CSV file.
	// @param print_header Whether or not to include the column names in the CSV file.
	void to_csv(std::string file_name, std::string sep = ",", bool print_header = true)
	{
		std::string full_file_name = file_name + ".csv";
		std::ofstream ofile;
		ofile.open(full_file_name);
		std::string write_string = "";

		// write column names to CSV file (optional)
		if (print_header)
		{
			for (int h = 0; h < this->columns.size(); ++h)
			{
				// don't add separator for last column
				if (h == this->columns.size() - 1)
				{
					ofile << this->columns[h];
				}
				else
				{
					ofile << this->columns[h] << sep;
				}
			}

			ofile << "\n";
		}


		for (int row = 0; row < this->data.size(); ++row)
		{
			for (int col = 0; col < this->data[0].size(); ++col)
			{
				if (col == this->data[0].size() - 1)
				{
					// don't add separator for last column
					write_string += this->data[row][col];
				}
				else
				{
					write_string += this->data[row][col] + sep;
				}
			}
			
			ofile << write_string << "\n";
			write_string = "";
		}
		ofile.close();
	}


	// @param modify_inline if true, will modify the DataSet object in place and not return anything. Otherwise, will return a new DataSet object with data transposed.
	// Note that column names will be reset after transposing.
	DataSet transpose(bool modify_inplace = false)
	{
		std::vector<std::vector<std::string>> transposed_data;
		// size the transposed_data to the opposite dimensions of the original data
		// R x C will become C x R size
		
		for (int col = 0; col < this->data[0].size(); ++col)
		{
			transposed_data.push_back(std::vector<std::string>());
			for (int row = 0; row < this->data.size(); ++row)
			{
				transposed_data[col].push_back("");
			}
		}

		// iterate over original data and swap indices
		for (int row = 0; row < this->data.size(); ++row)
		{
			for (int col = 0; col < this->data[0].size(); ++col)
			{
				transposed_data[col][row] = this->data[row][col];
			}
		}

		// reset columns after transposing
		std:vector<std::string> new_columns;
		for (int row = 0; row < this->data.size(); ++row)
		{
			new_columns.push_back("col" + std::to_string(row));
		}

		DataSet TransposedData;
		TransposedData.load(transposed_data, new_columns);

		// if modifying inplace, replace current reference for data/columns.
		if (modify_inplace)
		{
			this->data = transposed_data;
			this->columns = new_columns;
		}

		return TransposedData;
	}

	void countna()
	{
		int counter;
		std::cout << "column name : null count\n--------------------------\n";

		for (int col = 0; col < this->columns.size(); ++col)
		{
			counter = 0;
			for (int row = 0; row < this->data.size(); ++row)
			{
				if (this->data[row][col] == "") { counter += 1; }
			}
			std::cout << this->columns[col] << " : " << counter << "\n";
		}
	}

	// returns a vector of null counts (index corresponds to column index)
	std::vector<int> countna_vector()
	{
		int counter;
		std::vector<int> na_count;

		for (int col = 0; col < this->columns.size(); ++col)
		{
			counter = 0;
			for (int row = 0; row < this->data.size(); ++row)
			{
				if (this->data[row][col] == "") { counter += 1; }
			}
			na_count.push_back(counter);
		}

		return na_count;
	}

	DataSet dropna(bool inplace = false)
	{
		DataSet subset;
		std::vector<std::vector<std::string>> new_data;

		bool contains_na = false;
		
		for (int row = 0; row < this->data.size(); ++row)
		{
			for (int col = 0; col < this->columns.size(); ++col)
			{
				if (this->data[row][col]  == "") { contains_na = true; }
			}
			// if no null values present, copy row into new data set
			if (!contains_na) {
				new_data.push_back(this->data[row]);
			}

			// reset flag
			contains_na = false;
		}

		// modify inplace (optional)
		if (inplace)
		{
			this->data = new_data;
		}

		// construct new subset and return
		subset.load(new_data, this->columns);
		return subset;

	}

	void replacena(const std::string replace_text)
	{
		for (int rows = 0; rows < this->data.size(); ++rows)
		{
			for (int cols = 0; cols < this->data[0].size(); ++cols)
			{
				if (this->data[rows][cols] == "")
				{
					this->data[rows][cols] = replace_text;
				}
			}
		}
	}

	void replace(const std::string prior_text, const std::string replace_text)
	{
		for (int rows = 0; rows < this->data.size(); ++rows)
		{
			for (int cols = 0; cols < this->data[0].size(); ++cols)
			{
				if (this->data[rows][cols] == prior_text)
				{
					this->data[rows][cols] = replace_text;
				}
			}
		}
	}
};
#endif