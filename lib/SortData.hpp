#ifndef SORTDATA_HPP
#define SORTDATA_HPP

#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>

class SortData
{
private:
    std::vector<std::string> columns;

    int get_column_index(std::string col_name)
    {
        for (int i = 0; i < columns.size(); ++i)
        {
            if (col_name.compare(columns[i]) == 0)
            {
                return i;
            }
        }

        // no column found
        return -1;
    }

    int get_index(std::vector<std::string> column, std::string search_val, std::vector<int> &encountered_indices)
    {
        // at first I tried std::lower_bound but that didn't work. Just going to linearly search for now
        std::vector<int>::iterator idx;
        for (int i = 0; i < column.size(); ++i)
        {
            if (column[i].compare(search_val) == 0)
            {
                // make sure we didn't already record this index
                idx = std::find(encountered_indices.begin(), encountered_indices.end(), i);
                if (idx == encountered_indices.end())
                {
                    encountered_indices.push_back(i);
                    return i;
                }
            }
        }

        return -1;
    }

    // get original indices after sorting a column
    std::vector<int> get_original_indices(std::vector<std::string> original_column, std::vector<std::string> sorted_column)
    {
        std::vector<int> original_indices;
        std::vector<int> encountered_indices;

        int index;
        for (int i = 0; i < sorted_column.size(); ++i)
        {
            std::cout << original_column[i] << " " << sorted_column[i] << "\n";
            index = get_index(original_column, sorted_column[i], encountered_indices);
            original_indices.push_back(index);
        }

        return original_indices;
    }

    std::vector<std::string> transpose(std::vector<std::vector<std::string>> &data, int col_index, bool is_numeric)
    {
        std::vector<std::string> tposed;
        for (int i = 0; i < data.size(); ++i)
        {
            tposed.push_back(data[i][col_index]);
        }

        if (is_numeric)
        {
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

            // convert back to string to match formats
            tposed.clear();
            std::transform(data_double.begin(), data_double.end(), std::back_inserter(tposed), [](const double val) {
                return std::to_string(val);
            });
        }

        return tposed;
    }

    std::vector<std::string> transpose_sort(std::vector<std::vector<std::string>> &data, int col_index, bool is_numeric)
    {
        std::vector<std::string> tposed;
        for (int i = 0; i < data.size(); ++i)
        {
            tposed.push_back(data[i][col_index]);
        }

        if (is_numeric)
        {
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

            // convert back to string after sorting
            std::sort(data_double.begin(), data_double.end());
            tposed.clear();
            std::transform(data_double.begin(), data_double.end(), std::back_inserter(tposed), [](const double val) {
                return std::to_string(val);
            });
        }
        else
        {
            // if not numeric, sort as string instead
            std::sort(tposed.begin(), tposed.end());
        }

        return tposed;
    }

public:
    void set_columns(std::vector<std::string> columns)
    {
        this->columns = columns;
    }

    // sort_conditions :
    // [0] - column name
    // [1] - type (number/text)
    // [2] - direction (asc/desc)
    void sort(std::vector<std::vector<std::string>> &data, std::vector<std::vector<std::string>> &sort_conditions, int iter = 0)
    {
        int index;
        bool is_numeric;
        std::vector<std::string> sorted_column;
        std::vector<std::string> original_column;
        std::vector<int> sorted_indices;
        // Using this to drop columns after they're sorted
        std::vector<std::vector<std::string>> new_data_copy;
        for (int i = 0; i < data.size(); ++i)
        {
            new_data_copy.push_back(std::vector<std::string>());
            for (int j = 0; j < data[i].size(); ++j)
            {
                new_data_copy[i].push_back(data[i][j]);
            }
        }

        if (sort_conditions.size() < 1)
        {
            throw std::invalid_argument("Must sort on at least one column.");
        }
        else if (sort_conditions.size() >= 1)
        {
            index = get_column_index(sort_conditions[iter][0]);
            if (sort_conditions[iter][1].compare("number") == 0)
            {
                is_numeric = true;
            }
            else if (sort_conditions[iter][1].compare("text") == 0)
            {
                is_numeric = false;
            }
            else
            {
                throw std::invalid_argument("Sort type must be either 'number' or 'text'");
            }
            sorted_column = transpose_sort(data, index, is_numeric);
            original_column = transpose(data, index, is_numeric);
            sorted_indices = get_original_indices(original_column, sorted_column);

            std::vector<std::vector<std::string>> sorted_data;
            for (int i = 0; i < data.size(); ++i)
            {
                sorted_data.push_back(std::vector<std::string>());
                sorted_data[i].push_back(data[sorted_indices[i]][index]);
            }

            // drop index from data set copy (creating "residual" columns)
            std::for_each(new_data_copy.begin(), new_data_copy.end(),
                          [&](std::vector<std::string> &row) {
                              row.erase(std::next(row.begin(), index));
                          });

            /* MULTI COLUMN SORTING */
            if (sort_conditions.size() > 1)
            {
                int col_index = get_column_index(sort_conditions[1][0]);
                if (sort_conditions[1][1].compare("number") == 0)
                {
                    is_numeric = true;
                }
                else if (sort_conditions[1][1].compare("text") == 0)
                {
                    is_numeric = false;
                }
                else
                {
                    throw std::invalid_argument("Sort type must be either 'number' or 'text'");
                }
                int primary_col_index = index;
                std::string previous_value;
                std::vector<std::string> temp_vec;
                std::vector<std::string> final_vec;
                original_column.clear();
                for (int i = 0; i < sorted_data.size(); ++i)
                {
                    std::cout << "Previous value: " << previous_value << "\n";
                    if (i == 0)
                    {
                        temp_vec.push_back(data[sorted_indices[i]][col_index]);
                        previous_value = sorted_data[i][0];
                        if (is_numeric)
                        {
                            original_column.push_back(std::to_string(std::stod(data[i][col_index])));
                        }
                        else
                        {
                            original_column.push_back(data[i][col_index]);
                        }
                    }
                    else
                    {
                        if (is_numeric)
                        {
                            original_column.push_back(std::to_string(std::stod(data[i][col_index])));
                        }
                        else
                        {
                            original_column.push_back(data[i][col_index]);
                        }

                        if (previous_value.compare(sorted_data[i][0]) == 0)
                        {
                            temp_vec.push_back(data[sorted_indices[i]][col_index]);
                            previous_value = sorted_data[i][0];
                        }
                        else
                        {
                            if (is_numeric)
                            {
                                std::vector<double> data_double(temp_vec.size());
                                try
                                {
                                    std::transform(temp_vec.begin(), temp_vec.end(), data_double.begin(), [](const std::string &val) {
                                        return std::stod(val);
                                    });
                                }
                                catch (int e)
                                {
                                    std::runtime_error("Couldn't convert your data to double in transpose_double().");
                                }

                                // convert back to string after sorting
                                std::sort(data_double.begin(), data_double.end());
                                temp_vec.clear();
                                std::transform(data_double.begin(), data_double.end(), std::back_inserter(temp_vec), [](const double val) {
                                    return std::to_string(val);
                                });
                            }
                            else
                            {
                                std::sort(temp_vec.begin(), temp_vec.end());
                            }
                            final_vec.insert(final_vec.end(), temp_vec.begin(), temp_vec.end());
                            temp_vec.clear();
                            temp_vec.push_back(data[sorted_indices[i]][col_index]);
                            previous_value = sorted_data[i][0];
                        }
                    }
                }
                // add remainder element to final_vec
                if (is_numeric)
                {
                    // keep display consistent if it's numeric value
                    try
                    {
                        final_vec.push_back(std::to_string(std::stod(temp_vec[0])));
                    }
                    catch (int e)
                    {
                        throw std::runtime_error("Couldn't convert value to numeric when sorting.");
                    }
                }
                else
                {
                    final_vec.push_back(temp_vec[0]);
                }
                temp_vec.clear();

                // add new sorted column to sorted_data
                for (int i = 0; i < sorted_data.size(); ++i)
                {
                    sorted_data[i].push_back(final_vec[i]);
                }
                sorted_indices = get_original_indices(original_column, final_vec);
                final_vec.clear();

                for (int i = 0; i < sorted_indices.size(); ++i)
                {
                    std::cout << sorted_indices[i] << " ";
                }

                // drop index from data set copy (creating "residual" columns)
                std::for_each(new_data_copy.begin(), new_data_copy.end(),
                              [&](std::vector<std::string> &row) {
                                  row.erase(std::next(row.begin(), index));
                              });

                if (!new_data_copy.empty())
                {
                    // reorder residual data set columns
                    std::vector<std::vector<std::string>> temp_reordered;
                    // new_data_copy[sorted_indices[i]][col_index]
                    for (int i = 0; i < new_data_copy.size(); ++i)
                    {
                        temp_reordered.push_back(std::vector<std::string>());
                        for (int j = 0; j < new_data_copy[i].size(); ++j)
                        {
                            temp_reordered[i].push_back(new_data_copy[sorted_indices[i]][j]);
                        }
                    }

                    new_data_copy = temp_reordered;
                }

                // NOTE FOR DEVELOPMENT:
                /*
                    new_data_copy contains the residual colums (unsorted)
                    sorted_data contains the sorted columns

                    TODO:
                    need to collect *which* columns were sorted and rebuild the data set with columns in correct order
                    implement a loop to support sorting more than two columns (current max = 2)
                    return rebuilt sorted data set back to DataSet.h
                */
            }
        }
    }
};

#endif