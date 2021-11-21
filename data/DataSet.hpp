#ifndef DATASET_HPP
#define DATASET_HPP

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <type_traits>

#include "../stats/Stats.hpp"

template <class T>
class DataSet { 
    private:
        bool has_headers = true;

        std::vector<T> data;
        size_t columns, rows;

        size_t get_x, get_y;

        std::vector<std::string> get_unique_columns()
        {
            std::vector<std::string> columns_copy = column_names;
            std::sort(columns_copy.begin(), columns_copy.end());
            columns_copy.erase(std::unique(columns_copy.begin(), columns_copy.end()), columns_copy.end());

            return columns_copy;
        }

        // takes text parsed by split() and converts it to appropriate data type
        T check_text_type(std::string input_text)
        {
            T return_value;
            
            if constexpr (std::is_same_v<T, std::string>)
            {
                return_value = input_text;
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                return_value = std::stod(input_text);
            }
            else if constexpr (std::is_integral_v<T>)
            {
                return_value = std::stoi(input_text);
            }

            return return_value;
        }

        // load rows into data matrix
        void split(std::string text, size_t current_row, std::string sep = ",")
        {
            bool inside_string = false;
            size_t start_index = 0;
            size_t column_counter = 0;
            T converted_text;

            for (size_t i = 0; i < text.length(); i++)
            {
                if ((text.substr(i, sep.length()).compare(sep)) == 0 && !inside_string)
                {
                    converted_text = check_text_type(text.substr(start_index, i - start_index));
                    this->set(current_row, column_counter, converted_text);
                    start_index = i + sep.length();
                    column_counter += 1;
                }

                if (i == text.length() - 1)
                {
                    converted_text = check_text_type(text.substr(start_index, text.length()));
                    this->set(current_row, column_counter, converted_text);
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
            size_t start_index = 0;

            for (size_t i = 0; i < text.length(); i++)
            {
                if ((text.substr(i, sep.length()).compare(sep)) == 0 && !inside_string)
                {
                    column_names.push_back(text.substr(start_index, i - start_index));
                    start_index = i + sep.length();
                }

                if (i == text.length() - 1)
                {
                    column_names.push_back(text.substr(start_index, text.length()));
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

            if (get_unique_columns().size() != column_names.size())
            {
                throw std::runtime_error("Columns must be uniquely named when loading");
            }
        }

        // a replica of split() for headers but used for
        // counting columns
        size_t count_columns_from_file(std::string text, std::string sep = ",")
        {
            bool inside_string = false;
            size_t start_index = 0;
            size_t column_counter = 0;

            for (size_t i = 0; i < text.length(); i++)
            {
                if ((text.substr(i, sep.length()).compare(sep)) == 0 && !inside_string)
                {
                    column_counter += 1;
                    start_index = i + sep.length();
                }

                if (i == text.length() - 1)
                {
                    column_counter += 1;
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

            if (get_unique_columns().size() != column_names.size())
            {
                throw std::runtime_error("Columns must be uniquely named when loading");
            }

            return column_counter;
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

        // get dimensions of file to resize data set
        void get_dims_from_file(std::string filepath, std::string sep = ",", bool has_headers = true)
        {
            size_t row_count = 0, column_count = 0;

            this->has_headers = has_headers;

            std::string current_line;
            size_t current_row = 0;

            try
            {
                std::ifstream datafile(filepath);

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
                            column_count = count_columns_from_file(current_line, sep);
                            has_headers = false;
                        }
                        else
                        {
                            // in case headers = false was originally passed
                            // use the first row to count columns instead
                            if (column_count == 0) { column_count = count_columns_from_file(current_line, sep); }
                            current_row += 1;
                        }
                    }
                }
                datafile.close();
            }
            catch (int e)
            {
            }

            row_count = current_row;
            this->data.resize(column_count*row_count);
            this->columns = column_count;
            this->rows = row_count;
        }

        bool filter_bool(std::vector<T> const& row_index_values, std::function<bool(std::vector<T>)> filter_conditions)
        {
            return filter_conditions(row_index_values);
        }

    public:
        std::vector<std::string> column_names;

        DataSet() {}

        // load data set when passing filename
        DataSet(std::string filepath, std::string sep = ",", bool has_headers = true)
        {
            this->load(filepath, sep, has_headers);
        }

        DataSet(size_t x, size_t y) {
            data.resize(x*y);
            rows = x;
            columns = y;
        }

        void resize(size_t x, size_t y)
        {
            data.resize(x*y);
            rows = x;
            columns = y;
        }

        size_t count_rows()
        {
            return rows;
        }

        void resize_rows(size_t r)
        {
            this->rows = r;
        }

        size_t count_columns()
        {
            return columns;
        }

        void resize_columns(size_t c)
        {
            this->columns = c;
        }

        // extract a cell from data
        T &operator()(size_t x, size_t y)
        {
            get_x = x;
            get_y = y;
            return data[x * columns + y];
        }

        // set a value in the data after calling operator
        // data.set(x, y, value);
        void set(size_t x, size_t y, T value)
        {
            data[x * columns + y] = value;
        }

        // set a given row with a vector of values
        // data.set_row(row_index, row_vector);
        void set_row(size_t x, std::vector<T> const& row_data)
        {
            for (size_t i = 0; i < row_data.size(); ++i)
            {
                data[x * columns + i] = row_data[i];
            }
        }

        // set a given column with a vector of values
        // data.set_column(column_index, column_vector);
        void set_column(size_t y, std::vector<T> const& column_data)
        {
            for (size_t i = 0; i < column_data.size(); ++i)
            {
                data[i * columns + y] = column_data[i];
            }
        }

        // set the column names with a string vector
        void set_column_names(std::vector<std::string> const& column_names)
        {
            this->column_names = column_names;
        }

        // extract a row as a vector from a row index
        // data.get_row(row_index)
        std::vector<T> get_row(size_t x)
        {
            std::vector<T> return_vector(columns);
            get_x = x;
            get_y = 0;
            
            for (size_t i = 0; i < columns; ++i)
            {
                return_vector[i] = data[get_x * columns + i];
            }

            return return_vector;
        }

        // extract specific rows via vector of indices
        DataSet<T> get_rows(std::vector<size_t> const& row_indices)
        {
            DataSet subset;
            subset.resize(row_indices.size(), this->count_columns());
            if (this->column_names.size() > 0)
            {
                subset.set_column_names(this->column_names);
            }

            size_t row_iter = 0;
            for (auto const& row : row_indices)
            {
                subset.set_row(row_iter, this->get_row(row));
                row_iter++;
            }

            return subset;
        }

        // extract a column as a vector from a vector index
        // data.get_column(column_index)
        std::vector<T> get_column(size_t y)
        {
            std::vector<T> return_vector(rows);
            get_x = 0;
            get_y = y;

            for (size_t i = 0; i < rows; ++i)
            {
                return_vector[i] = data[i * columns + get_y];
            }

            return return_vector;
        }

        // get column indices from a given vector of column names 
        std::vector<size_t> get_column_indices(std::vector<std::string> const& passed_columns)
        {
            std::vector<size_t> col_idx;
            for (std::string col_name : passed_columns)
            {
                auto it = std::find(column_names.begin(), column_names.end(), col_name);
                if (it == column_names.end())
                {
                    throw std::invalid_argument("Column name '" + col_name + "' was not found.");
                }
                col_idx.push_back(it - column_names.begin());
            }

            return col_idx;
        }

        // cast data set to specified type
        template <typename X>
        DataSet<X> cast()
        {
            DataSet<X> casted_dataset(this->count_rows(), this->count_columns());
            casted_dataset.set_column_names(this->column_names);

            // if new type is double and old type is string (std::stod)
            if constexpr (std::is_floating_point_v<X> && std::is_same_v<T, std::string>)
            {
                for (size_t r = 0; r < this->count_rows(); ++r)
                {
                    for (size_t c = 0; c < this->count_columns(); ++c)
                    {
                        casted_dataset.set(r, c, std::stod((*this)(r, c)));
                    }
                }
            }

            // if new type is integer and old type is string (std::stoi)
            else if constexpr (std::is_integral_v<X> && std::is_same_v<T, std::string>)
            {
                for (size_t r = 0; r < this->count_rows(); ++r)
                {
                    for (size_t c = 0; c < this->count_columns(); ++c)
                    {
                        casted_dataset.set(r, c, std::stoi((*this)(r, c)));
                    }
                }
            }

            // if new and old types are numeric and don't match
            else if constexpr (
                (std::is_floating_point_v<X> && std::is_integral_v<T>)
                || (std::is_integral_v<X> && std::is_floating_point_v<T>)
            )
                {
                    for (size_t r = 0; r < this->count_rows(); ++r)
                    {
                        for (size_t c = 0; c < this->count_columns(); ++c)
                        {
                            casted_dataset.set(r, c, (X)(*this)(r, c));
                        }
                    }
                }

            // if both types match
            // no point in casting if the types are the same
            else if constexpr (
                (std::is_floating_point_v<X> && std::is_floating_point_v<T>)
                || (std::is_integral_v<X> && std::is_integral_v<T>)
                || (std::is_same_v<X, std::string> && std::is_same_v<T, std::string>)
            )
            {
                throw std::invalid_argument("Original and new data type to cast are the same.");
            } 

            // if new type is string and old type is numeric
            else if constexpr (
                (std::is_same_v<X, std::string> && std::is_floating_point_v<T>)
                || (std::is_same_v<X, std::string> && std::is_integral_v<T>)
            )
            {
                for (size_t r = 0; r < this->count_rows(); ++r)
                {
                    for (size_t c = 0; c < this->count_columns(); ++c)
                    {
                        casted_dataset.set(r, c, std::to_string((*this)(r, c)));
                    }
                }
            }

            return casted_dataset;
        }

        // load from file
        void load(std::string filepath, std::string sep = ",", bool has_headers = true)
        {
            // get dimensions to resize data set before actually
            // loading file
            get_dims_from_file(filepath, sep, has_headers);

            this->has_headers = has_headers;

            std::string current_line;
            size_t current_row = 0;

            try
            {
                std::ifstream datafile(filepath);

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
        void load(std::vector<T> const& data, std::vector<std::string> const& columns)
        {
            this->resize(1, columns.size());
            this->column_names = columns;
            this->set_row(0, data);
        }

        // load from 1D vector (will be translated into column vector)
        void load(std::vector<T> const& data, std::string column_name)
        {
            std::vector<std::string> temp_column = { column_name };

            // set 1D vector as column vector
            this->resize(data.size(), 1);
            this->set_column(0, data);
            this->column_names = temp_column;
        }

        // print first N rows of a data set (default = 10)
        void head(size_t rows = 10)
        {
            std::string cutoff_str;
            // if column vector exists, print columns before data
            if (column_names.size() > 0)
            {
                for (size_t c = 0; c < column_names.size(); ++c)
                {

                    //std::cout << columns[c] << "\t\t";
                    if (column_names[c].length() < 15)
                    {
                        std::cout << column_names[c] << std::setfill(' ') << std::setw(15 - column_names[c].length());
                    }
                    else if (column_names[c].length() == 15)
                    {
                        std::cout << column_names[c];
                    }
                    else
                    {
                        cutoff_str = column_names[c];
                        cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
                        std::cout << cutoff_str;
                    }

                    std::cout << "\t";
                }
                std::cout << "\n";
                std::cout << std::setfill('-') << std::setw(15 * column_names.size());
            }
            std::cout << "\n";

            // check if type is double/integer
            if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
            {
                // print #[rows] of data
                for (size_t r = 0; r < rows; ++r)
                {
                    if (r >= this->count_rows())
                    {
                        break;
                    } // check if rows is larger than our data set
                    else
                    {
                        for (size_t c = 0; c < this->count_columns(); ++c)
                        {
                            
                            if (std::to_string((*this)(r, c)).length() < 15)
                            {
                                std::cout << std::to_string((*this)(r, c)) << std::setfill(' ') << std::setw(15 - std::to_string((*this)(r, c)).length());
                            }
                            else if (std::to_string((*this)(r, c)).length() == 15)
                            {
                                std::cout << std::to_string((*this)(r, c));
                            }
                            else
                            {
                                cutoff_str = std::to_string((*this)(r, c));
                                cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
                                std::cout << cutoff_str;
                            }

                            std::cout << "\t";
                        }
                        std::cout << std::endl;
                    }
                }
            }
            else
            // assuming a string if not double/integer
            {
                for (size_t r = 0; r < rows; ++r)
                {
                    if (r >= this->count_rows())
                    {
                        break;
                    } // check if rows is larger than our data set
                    else
                    {
                        for (size_t c = 0; c < this->count_columns(); ++c)
                        {
                            
                            if ((*this)(r, c).length() < 15)
                            {
                                std::cout << (*this)(r, c) << std::setfill(' ') << std::setw(15 - (*this)(r, c).length());
                            }
                            else if ((*this)(r, c).length() == 15)
                            {
                                std::cout << (*this)(r, c);
                            }
                            else
                            {
                                cutoff_str = (*this)(r, c);
                                cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
                                std::cout << cutoff_str;
                            }

                            std::cout << "\t";
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }

        // select subset of original data set by index (creates new data set)
        template <typename X>
        DataSet<X> select(std::vector<size_t> const& indices, bool inplace = false)
        {
            size_t new_size = indices.size();

            std::vector<std::string> new_columns(new_size);

            if (has_headers)
            {
                for (size_t i = 0; i < new_size; ++i)
                {
                    new_columns[i] = column_names[indices[i]];
                }
            }

            DataSet<X> subset(this->count_rows(), new_size);

            /*
            Here the user can convert data types while selecting columns.
            Note that these MUST be constexpr to evaluate at compile time
            otherwise you can run into errors with certain type mismatches.
            (Learned that the hard way)
            */

            // if new type is double and old type is string (std::stod)
            if constexpr (std::is_floating_point_v<X> && std::is_same_v<T, std::string>)
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_size; ++col)
                {
                    extracted_column = this->get_column(indices[col]);
                    // convert extracted column (which is of string data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](std::string const& val) { return std::stod(val); });
                    subset.set_column(col, converted_column);
                }
            }

            // if new type is integer and old type is string (std::stoi)
            else if constexpr (std::is_integral_v<X> && std::is_same_v<T, std::string>)
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_size; ++col)
                {
                    extracted_column = this->get_column(indices[col]);
                    // convert extracted column (which is of string data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](std::string const& val) { return std::stoi(val); });
                    subset.set_column(col, converted_column);
                }
            }

            // if new and old types are numeric and don't match
            else if constexpr (
                (std::is_floating_point_v<X> && std::is_integral_v<T>)
                || (std::is_integral_v<X> && std::is_floating_point_v<T>)
            )
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_size; ++col)
                {
                    extracted_column = this->get_column(indices[col]);
                    // convert extracted column (which is of integer data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](T const& val) { return (X)val; });
                    subset.set_column(col, converted_column);
                }
            }

            // if both types match
            else if constexpr (
                (std::is_floating_point_v<X> && std::is_floating_point_v<T>)
                || (std::is_integral_v<X> && std::is_integral_v<T>)
                || (std::is_same_v<X, std::string> && std::is_same_v<T, std::string>)
            )
            {
                std::vector<T> extracted_column;

                for (size_t col = 0; col < new_size; ++col)
                {
                    extracted_column = this->get_column(indices[col]);
                    subset.set_column(col, extracted_column);
                }
            } 

            // if new type is string and old type is numeric
            else if constexpr (
                (std::is_same_v<X, std::string> && std::is_floating_point_v<T>)
                || (std::is_same_v<X, std::string> && std::is_integral_v<T>)
            )
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_size; ++col)
                {
                    extracted_column = this->get_column(indices[col]);
                    // convert extracted column (which is of string data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](T const& val) { return std::to_string(val); });
                    subset.set_column(col, converted_column);
                }
            }

            subset.set_column_names(new_columns);

            if (inplace)
            {
                *this = subset;
            }

            return subset;
        }

        template <typename X>
        DataSet<X> select(std::vector<std::string> const& indices, bool inplace = false)
        {
            // get column indices from passed vector and call original select() method
            return select<X>(get_column_indices(indices), inplace);
        }

        // drop certain columns (the inverse of select())
        template <typename X>
        DataSet<X> drop(std::vector<size_t> const& indices, bool inplace = false)
        {
            size_t new_size = indices.size();
            size_t current_column_index = 0;
            std::vector<std::string> new_columns(this->count_columns() - new_size);
            std::vector<size_t> new_column_indices(this->count_columns() - new_size);

            if (has_headers)
            {
                for (size_t i = 0; i < this->count_columns(); ++i)
                {
                    auto it = std::find(indices.begin(), indices.end(), i);
                    if (it == indices.end())
                    {
                        new_columns[current_column_index] = this->column_names[i];
                        new_column_indices[current_column_index] = i;
                        current_column_index += 1;
                    }
                }
            }

            DataSet<X> subset(this->count_rows(), this->count_columns() - new_size);

            /*
            Here the user can convert data types while selecting columns.
            Note that these MUST be constexpr to evaluate at compile time
            otherwise you can run into errors with certain type mismatches.
            (Learned that the hard way)
            */

            // if new type is double and old type is string (std::stod)
            if constexpr (std::is_floating_point_v<X> && std::is_same_v<T, std::string>)
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_column_indices.size(); ++col)
                {
                    extracted_column = this->get_column(new_column_indices[col]);
                    // convert extracted column (which is of string data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](std::string const& val) { return std::stod(val); });
                    subset.set_column(col, converted_column);
                }
            }

            // if new type is integer and old type is string (std::stoi)
            else if constexpr (std::is_integral_v<X> && std::is_same_v<T, std::string>)
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_column_indices.size(); ++col)
                {
                    extracted_column = this->get_column(new_column_indices[col]);
                    // convert extracted column (which is of string data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](std::string const& val) { return std::stoi(val); });
                    subset.set_column(col, converted_column);
                }
            }

            // if new and old types are numeric and don't match
            else if constexpr (
                (std::is_floating_point_v<X> && std::is_integral_v<T>)
                || (std::is_integral_v<X> && std::is_floating_point_v<T>)
            )
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_column_indices.size(); ++col)
                {
                    extracted_column = this->get_column(new_column_indices[col]);
                    // convert extracted column (which is of integer data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](T const& val) { return (X)val; });
                    subset.set_column(col, converted_column);
                }
            }

            // if both types match
            else if constexpr (
                (std::is_floating_point_v<X> && std::is_floating_point_v<T>)
                || (std::is_integral_v<X> && std::is_integral_v<T>)
                || (std::is_same_v<X, std::string> && std::is_same_v<T, std::string>)
            )
            {
                std::vector<T> extracted_column;

                for (size_t col = 0; col < new_column_indices.size(); ++col)
                {
                    extracted_column = this->get_column(new_column_indices[col]);
                    subset.set_column(col, extracted_column);
                }
            } 

            // if new type is string and old type is numeric
            else if constexpr (
                (std::is_same_v<X, std::string> && std::is_floating_point_v<T>)
                || (std::is_same_v<X, std::string> && std::is_integral_v<T>)
            )
            {
                std::vector<T> extracted_column;
                std::vector<X> converted_column(this->count_rows());

                for (size_t col = 0; col < new_column_indices.size(); ++col)
                {
                    extracted_column = this->get_column(new_column_indices[col]);
                    // convert extracted column (which is of string data type)
                    // to a double vector and set column
                    std::transform(extracted_column.begin(), extracted_column.end(), converted_column.begin(),
                    [](T const& val) { return std::to_string(val); });
                    subset.set_column(col, converted_column);
                }
            }

            subset.set_column_names(new_columns);

            if (inplace)
            {
                *this = subset;
            }

            return subset;
        }

        template <typename X>
        DataSet<X> drop(std::vector<std::string> const& indices, bool inplace = false)
        {
            // get column indices from passed vector and call original drop() method
            return drop<X>(get_column_indices(indices), inplace);
        }

        DataSet<T> filter(std::function<bool(std::vector<T>)> filter_conditions, bool inplace = false)
        {
            std::vector<size_t> returned_rows;
            // iterate over data set and evaluate filter_conditions to check whether
            // or not to return a row
            for (size_t current_row = 0; current_row < this->count_rows(); ++current_row)
            {
                // if user-defined filter succeeds, record row index
                if (filter_bool(this->get_row(current_row), filter_conditions))
                {
                    returned_rows.push_back(current_row);
                }
            }

            // Use returned rows to size up data set and start adding records
            DataSet<T> filtered_data(returned_rows.size(), this->count_columns());
            filtered_data.set_column_names(this->column_names);
            for (size_t i = 0; i < returned_rows.size(); ++i)
            {
                filtered_data.set_row(i, this->get_row(returned_rows[i]));
            }

            if (inplace)
            {
                *this = filtered_data;
            }

            return filtered_data;
        }

        // sample a data set with or without replacement
        DataSet<T> sample(size_t n = 1, bool replace = false)
        {
            // set random seed
            srand(time(NULL));
            
            if (n < 1)
            {
                throw std::invalid_argument("You must sample at least one element.");
            }
            else if (n > this->count_rows() && !replace)
            {
                throw std::invalid_argument("You can't uniquely sample elements larger than your data set.");
            }

            std::unordered_map<size_t, size_t> unique_indices;
            std::vector<size_t> random_indices(n);
            size_t iter = 0;
            size_t random_index;

            while (iter < n)
            {
                random_index = rand() % (this->count_rows());
                if (!replace && unique_indices.count(random_index) == 0)
                {
                    unique_indices[random_index] = 1;
                    random_indices[iter] = random_index;
                    iter += 1;
                }
                else if (replace)
                {
                    random_indices[iter] = random_index;
                    iter += 1;
                }
            }

            // extract rows that were sampled
            DataSet<T> sampled_data(n, this->count_columns());
            for (size_t i = 0; i < n; ++i)
            {
                sampled_data.set_row(i, this->get_row(random_indices[i]));
            }
            sampled_data.set_column_names(this->column_names);

            return sampled_data;
        }

        // append/concat data sets together (if they have the same size)
        DataSet<T> append(DataSet<T> other_data, char type = 'r', bool inplace = false)
        {
            DataSet<T> appended_data;

            // making a copy of current data set in order to not mutate it
            std::vector<std::string> columns_copy;
            if (this->column_names.size() > 0)
            {
                columns_copy = this->column_names;
            }

            if (type == 'r')
            {
                // check if column sizes match
                if (this->count_columns() != other_data.count_columns())
                {
                    throw std::runtime_error("Dimensions when appending data sets don't match.");
                }
                
                appended_data.resize(this->count_rows() + other_data.count_rows(), this->count_columns());
               
                // copy first data set
                for (size_t i = 0; i < this->count_rows(); ++i)
                {
                    appended_data.set_row(i, this->get_row(i));
                }
                
                // copy second data set
                for (size_t i = 0; i < other_data.count_rows(); ++i)
                {
                    appended_data.set_row(i + this->count_rows(), other_data.get_row(i));
                }

                appended_data.set_column_names(this->column_names);
            }
            else if (type == 'c')
            {
                // check if row sizes match
                if (this->count_rows() != other_data.count_rows())
                {
                    throw std::runtime_error("Dimensions when appending data sets don't match.");
                }

                // check column collision (all columns need to be uniquely named)
                std::vector<std::string> col_intersection;
                if (columns_copy.size() > 0)
                {
                    std::sort(columns_copy.begin(), columns_copy.end());
                    std::sort(other_data.column_names.begin(), other_data.column_names.end());
                    std::set_intersection(columns_copy.begin(), columns_copy.end(),
                                        other_data.column_names.begin(), other_data.column_names.end(),
                                        back_inserter(col_intersection));
                }

                if (col_intersection.size() > 0)
                {
                    throw std::runtime_error("Columns must be uniquely named between both data sets when appending.");
                }

                appended_data.resize(this->count_rows(), this->count_columns() + other_data.count_columns());

                if (columns_copy.size() > 0)
                {
                    for (size_t i = 0; i < other_data.count_columns(); ++i)
                    {
                        columns_copy.push_back(other_data.column_names[i]);
                    }   
                }

                // copy first data set
                for (size_t i = 0; i < this->count_columns(); ++i)
                {
                    appended_data.set_column(i, this->get_column(i));
                }

                // copy second data set
                for (size_t i = 0; i < other_data.count_columns(); ++i)
                {
                    appended_data.set_column(i + this->count_columns(), other_data.get_column(i));
                }

                appended_data.set_column_names(columns_copy);
            }
            else
            {
                throw std::invalid_argument("Only 'r' (rows) and 'c' (columns) are allowed when appending data.");
            }

            if (inplace)
            {
                *this = appended_data;
            }

            return appended_data;
        }

        // rename a set of columns according to a map where the key is the
        // original name and value is the new name
        void rename(std::unordered_map<std::string, std::string> new_cols)
        {
            std::unordered_map<std::string, std::string>::iterator map_it;
            std::vector<std::string>::iterator vec_it;

            for (map_it = new_cols.begin(); map_it != new_cols.end(); ++map_it)
            {
                vec_it = std::find(this->column_names.begin(), this->column_names.end(), map_it->first);
                if (vec_it != this->column_names.end())
                {
                    this->column_names[vec_it - this->column_names.begin()] = map_it->second;
                }

                // if column in map is not found, ignore it
            }
        }

        // transpose a data set
        DataSet<T> transpose()
        {    
            DataSet<T> transposed_data;
            std::vector<std::string> new_column_names;

            // transpose 1D data sets
            if (this->count_columns() == 1 || this->count_rows() == 1)
            {
                // transpose column vector to row vector
                if (this->count_columns() == 1)
                {
                    transposed_data.resize(1, this->count_rows());
                    transposed_data.set_row(0, this->get_column(0));
                    // set generic column names
                    for (size_t i = 0; i < this->count_rows(); ++i)
                    {
                        new_column_names.push_back("col" + std::to_string(i));
                    }
                }

                // transpose row vector to column vector
                else if (this->count_rows() == 1)
                {
                    transposed_data.resize(this->count_columns(), 1);
                    transposed_data.set_column(0, this->get_row(0));
                    new_column_names.push_back("col1");
                }
            }
            else
            {
                // transpose 2D data sets
                // swap row and column indices (and sizes)
                transposed_data.resize(this->count_columns(), this->count_rows());
                for (size_t i = 0; i < this->count_rows(); ++i)
                {
                    for (size_t j = 0; j < this->count_columns(); ++j)
                    {
                        transposed_data.set(j, i, (*this)(i, j));
                    }
                }

                // set generic column names
                for (size_t i = 0; i < this->count_rows(); ++i)
                {
                    new_column_names.push_back("col" + std::to_string(i));
                }
            }

            transposed_data.set_column_names(new_column_names);

            return transposed_data;
        }

        // print common statistics on numeric data sets and print to console
        void describe()
        {
            if constexpr (!std::is_floating_point_v<T>)
            {
                throw std::runtime_error("describe() currently only supports double data types.\nPlease use cast() or select() to convert your data set.");
            }
            else if constexpr (std::is_floating_point_v<T>)
            {

                double _sum, _mean, _stdev, _min, _10p, _25p, _median, _75p, _90p, _max;
                std::string cutoff_str;
                std::cout << "             |  ";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {

                    if (this->column_names[c].length() < 15)
                    {
                        std::cout << this->column_names[c] << std::setfill(' ') << std::setw(15 - this->column_names[c].length());
                    }
                    else if (this->column_names[c].length() == 15)
                    {
                        std::cout << this->column_names[c];
                    }
                    else
                    {
                        cutoff_str = this->column_names[c];
                        cutoff_str.replace(cutoff_str.begin() + 12, cutoff_str.end(), "...");
                        std::cout << cutoff_str;
                    }

                    std::cout << "\t";
                }
                std::cout << "\n";
                std::cout << std::setfill('-') << std::setw(15 * this->column_names.size() + 15);

                Stats stats;
                std::cout << "\nSum:" << std::setfill(' ') << std::setw(10) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _sum = stats.sum(this->get_column(c));
                    print_describe_line(_sum);
                }

                std::cout << "\nMin:" << std::setfill(' ') << std::setw(10) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _min = stats.min(this->get_column(c));
                    print_describe_line(_min);
                }

                std::cout << "\nMax:" << std::setfill(' ') << std::setw(10) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _max = stats.max(this->get_column(c));
                    print_describe_line(_max);
                }

                std::cout << "\nMean:" << std::setfill(' ') << std::setw(9) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _mean = stats.mean(this->get_column(c));
                    print_describe_line(_mean);
                }

                std::cout << "\nStDev:" << std::setfill(' ') << std::setw(8) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _stdev = stats.stdev(this->get_column(c));
                    print_describe_line(_stdev);
                }

                std::cout << "\n10th %:" << std::setfill(' ') << std::setw(7) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _10p = stats.percentile(this->get_column(c), 0.1);
                    print_describe_line(_10p);
                }

                std::cout << "\n25th %:" << std::setfill(' ') << std::setw(7) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _25p = stats.percentile(this->get_column(c), 0.25);
                    print_describe_line(_25p);
                }

                std::cout << "\nMedian:" << std::setfill(' ') << std::setw(7) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _median = stats.median(this->get_column(c));
                    print_describe_line(_median);
                }

                std::cout << "\n75th %:" << std::setfill(' ') << std::setw(7) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _75p = stats.percentile(this->get_column(c), 0.75);
                    print_describe_line(_75p);
                }

                std::cout << "\n90th %:" << std::setfill(' ') << std::setw(7) << "|"
                        << "\t";
                for (size_t c = 0; c < this->column_names.size(); ++c)
                {
                    _90p = stats.percentile(this->get_column(c), 0.90);
                    print_describe_line(_90p);
                }

                std::cout << "\n";
            }
        }

        // split data set into "train/test" sets
        void split_data(double test_ratio, DataSet<T> &train, DataSet<T> &test)
        {
            // set random seed
            srand(time(NULL));

            size_t data_size = this->count_rows();
            size_t sample_iter = 0;
            std::unordered_map<size_t, size_t> index_map;
            size_t test_size;
            size_t rand_index;

            train.set_column_names(this->column_names);
            test.set_column_names(this->column_names);

            if (test_ratio > 0 && test_ratio < 1)
            {
                test_size = (size_t)(test_ratio * data_size);
                // size train/test data sets accordingly
                train.resize(data_size - test_size, this->count_columns());
                test.resize(test_size, this->count_columns());

                // populate test data set
                while (sample_iter < test_size)
                {
                    rand_index = rand() % data_size;
                    if (index_map.find(rand_index) == index_map.end())
                    {
                        test.set_row(sample_iter, this->get_row(rand_index));
                        index_map.insert({{rand_index, 1}});
                        sample_iter += 1;
                    }
                }

                // populate train data set by skipping the indices added from test data
                // probably a better way to go about this, but this is fine for now
                // could create a vector of test indices and remove it from the vector of 0, ..., data_size to get train indices
                size_t train_temp_iter = 0;
                for (size_t i = 0; i < data_size; ++i)
                {
                    if (index_map.find(i) == index_map.end())
                    {
                        train.set_row(train_temp_iter, this->get_row(i));
                        train_temp_iter += 1;
                    }
                }
            }
            else
            {
                throw std::invalid_argument("test_ratio parameter must be in interval (0, 1).");
            }
        }

        // Write DataSet to CSV file with custom delimiter and optionally print headers
        void to_csv(std::string file_name, std::string sep = ",", bool print_header = true)
        {
            std::ofstream ofile;
            ofile.open(file_name);
            std::string write_string = "";

            // write column names to CSV file (optional)
            if (print_header)
            {
                for (size_t h = 0; h < this->count_columns(); ++h)
                {
                    // don't add separator for last column
                    if (h == this->count_columns() - 1)
                    {
                        ofile << this->column_names[h];
                    }
                    else
                    {
                        ofile << this->column_names[h] << sep;
                    }
                }

                ofile << "\n";
            }


            // if data type is string
            if constexpr (std::is_same_v<T, std::string>)
            {
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    for (size_t col = 0; col < this->count_columns(); ++col)
                    {
                        if (col == this->count_columns() - 1)
                        {
                            // don't add separator for last column
                            write_string += (*this)(row, col);
                        }
                        else
                        {
                            write_string += (*this)(row, col) + sep;
                        }
                    }
                    
                    if (row == this->count_rows() - 1)
                    {
                        ofile << write_string; // don't add random breakline at end of file
                    }
                    else
                    {
                        ofile << write_string << "\n";
                    }

                    write_string = "";
                }
            }

            // if data type is numeric (must convert to string)
            else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
            {
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    for (size_t col = 0; col < this->count_columns(); ++col)
                    {
                        if (col == this->count_columns() - 1)
                        {
                            // don't add separator for last column
                            write_string += std::to_string((*this)(row, col));
                        }
                        else
                        {
                            write_string += std::to_string((*this)(row, col)) + sep;
                        }
                    }
                    
                    if (row == this->count_rows() - 1)
                    {
                        ofile << write_string; // don't add random breakline at end of file
                    }
                    else
                    {
                        ofile << write_string << "\n";
                    }

                    write_string = "";
                }
            }

            ofile.close();
        }

        // Count null (blank) values and print to console for each column
        // NOTE: Only works on std::string data sets
        void countna()
        {
            size_t counter;
            std::cout << "column name : null count\n--------------------------\n";

            for (size_t col = 0; col < this->count_columns(); ++col)
            {
                counter = 0;
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    if ((*this)(row, col) == "") { counter += 1; }
                }
                std::cout << this->column_names[col] << " : " << counter << "\n";
            }
        }

        // returns a vector of null counts (index corresponds to column index)
        std::vector<size_t> countna_vector()
        {
            size_t counter;
            std::vector<size_t> na_count(this->count_columns());

            for (size_t col = 0; col < this->count_columns(); ++col)
            {
                counter = 0;
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    if ((*this)(row, col) == "") { counter += 1; }
                }
                na_count[col] = counter;
            }

            return na_count;
        }

        // drop rows that contain any null values
        DataSet<T> dropna(bool inplace = false)
        {
            DataSet<T> subset;
            size_t na_counter = 0, row_iter = 0;

            bool contains_na = false;

            // count occurrences of null values to size new subset
            for (size_t row = 0; row < this->count_rows(); ++row)
            {
                for (size_t col = 0; col < this->count_columns(); ++col)
                {
                    if ((*this)(row, col)  == "") { na_counter += 1; }
                }
            }

            subset.resize(this->count_rows() - na_counter, this->count_columns());

            // re-iterate after sizing subset and drop rows
            for (size_t row = 0; row < this->count_rows(); ++row)
            {
                for (size_t col = 0; col < this->count_columns(); ++col)
                {
                    if ((*this)(row, col)  == "") { contains_na = true; }
                }
                // if no null values present, copy row into new data set
                if (!contains_na) {
                    subset.set_row(row_iter, this->get_row(row));
                    row_iter += 1;
                }

                // reset flag
                contains_na = false;
            }

            subset.set_column_names(this->column_names);

            if (inplace)
            {
                *this = subset;
            }

            return subset;
        }

        // replace null values with some value
        DataSet<T> replacena(const std::string replace_text, bool inplace = false)
        {
            DataSet<T> modified_data(this->count_rows(), this->count_columns());
            modified_data.set_column_names(this->column_names);

            if (inplace)
            {
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    for (size_t col = 0; col < this->count_columns(); ++col)
                    {
                        if ((*this)(row, col) == "")
                        {
                            this->set(row, col, replace_text);
                        }
                    }
                }

                return (*this);
            }
            else
            {
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    for (size_t col = 0; col < this->count_columns(); ++col)
                    {
                        if ((*this)(row, col) == "")
                        {
                            modified_data.set(row, col, replace_text);
                        }
                        else
                        {
                            modified_data.set(row, col, (*this)(row, col));
                        }
                    }
                }
            }

            return modified_data;
        }

        // replace arbitrary value in data set
        // unlike replacena(), this supports numeric data types as well
        DataSet<T> replace(T original_value, T replace_value, bool inplace = false, size_t occurences = 0)
        {
            DataSet<T> modified_data(this->count_rows(), this->count_columns());
            modified_data.set_column_names(this->column_names);
            size_t occurence_counter = 0;

            if (inplace)
            {
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    for (size_t col = 0; col < this->count_columns(); ++col)
                    {
                        if ((*this)(row, col) == original_value && occurences == 0) // no limit on occurences
                        {
                            this->set(row, col, replace_value);
                            occurence_counter += 1;
                        }
                        else if ((*this)(row, col) == original_value && occurence_counter < occurences)
                        {
                            this->set(row, col, replace_value);
                            occurence_counter += 1;
                        }
                    }
                }

                return (*this);
            }
            else
            {
                for (size_t row = 0; row < this->count_rows(); ++row)
                {
                    for (size_t col = 0; col < this->count_columns(); ++col)
                    {
                        if (occurences == 0) // no limit on occurences
                        {
                            if ((*this)(row, col) == original_value)
                            {
                                modified_data.set(row, col, replace_value);
                            }
                            else
                            {
                                modified_data.set(row, col, (*this)(row, col));
                            }
                        }
                        else
                        {
                           if ((*this)(row, col) == original_value && occurence_counter < occurences)
                            {
                                modified_data.set(row, col, replace_value);
                                occurence_counter += 1;
                            }
                            else
                            {
                                modified_data.set(row, col, (*this)(row, col));
                            } 
                        }
                    }
                }
            }

            return modified_data;
        }
};
#endif