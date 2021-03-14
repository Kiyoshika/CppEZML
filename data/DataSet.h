#ifndef DATASET_H
#define DATASET_H

#include "../lib/FilterData.h"
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <map>

// only supports delimiters of one character length
class DataSet
{
private:
	bool has_headers = true;

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

	void head(int rows = 10)
	{
		// if column vector exists, print columns before data
		if (columns.size() > 0)
		{
			for (int c = 0; c < columns.size(); ++c)
			{
				cout << columns[c] << "\t\t";
			}
		}
		cout << endl;

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
					cout << data[r][c] << "\t\t";
				}
				cout << endl;
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
		map<std::string, int> name_index;
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
};
#endif
