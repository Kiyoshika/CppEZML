#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <type_traits>

template <class T>
class DataSet { 
    private:
        bool has_headers = true;

        std::vector<T> data;
        unsigned int columns, rows;

        unsigned int get_x, get_y;

        std::vector<std::string> get_unique_columns(std::vector<std::string> const &columns)
        {
            std::vector<std::string> columns_copy = column_names;
            std::sort(columns_copy.begin(), columns_copy.end());
            columns_copy.erase(std::unique(columns_copy.begin(), columns_copy.end()), columns_copy.end());

            return columns_copy;
        }

        // load rows into data matrix
        void split(std::string text, unsigned int current_row, std::string sep = ",")
        {
            bool inside_string = false;
            unsigned int start_index = 0;
            unsigned int column_counter = 0;

            for (size_t i = 0; i < text.length(); i++)
            {
                if ((text.substr(i, sep.length()).compare(sep)) == 0 && !inside_string)
                {
                    this->set(current_row, column_counter, (text.substr(start_index, i - start_index)));
                    start_index = i + sep.length();
                    column_counter += 1;
                }

                if (i == text.length() - 1)
                {
                    this->set(current_row, column_counter, (text.substr(start_index, text.length())));
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
            unsigned int start_index = 0;

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

            if (get_unique_columns(column_names).size() != column_names.size())
            {
                throw std::runtime_error("Columns must be uniquely named when loading");
            }
        }

        // a replica of split() for headers but used for
        // counting columns
        unsigned int count_columns_from_file(std::string text, std::string sep = ",")
        {
            bool inside_string = false;
            unsigned int start_index = 0;
            unsigned int column_counter = 0;

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

            if (get_unique_columns(column_names).size() != column_names.size())
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
            unsigned int row_count = 0, column_count = 0;

            this->has_headers = has_headers;

            std::string current_line;
            unsigned int current_row = 0;

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

    public:
        std::vector<std::string> column_names;

        DataSet() {}

        DataSet(unsigned int x, unsigned int y) {
            data.resize(x*y);
            rows = x;
            columns = y;
        }

        void resize(unsigned int x, unsigned int y)
        {
            data.resize(x*y);
            rows = x;
            columns = y;
        }

        unsigned int count_rows()
        {
            return rows;
        }

        void resize_rows(unsigned int r)
        {
            this->rows = r;
        }

        unsigned int count_columns()
        {
            return columns;
        }

        void resize_columns(unsigned int c)
        {
            this->columns = c;
        }

        // extract a cell from data
        T &operator()(unsigned int x, unsigned int y)
        {
            get_x = x;
            get_y = y;
            return data[x * columns + y];
        }

        // set a value in the data after calling operator
        // data.set(x, y, value);
        void set(unsigned int x, unsigned int y, T value)
        {
            data[x * columns + y] = value;
        }

        // set a given row with a vector of values
        // data.set_row(row_index, row_vector);
        void set_row(unsigned int x, std::vector<T> row_data)
        {
            for (size_t i = 0; i < row_data.size(); ++i)
            {
                data[x * columns + i] = row_data[i];
            }
        }

        // set a given column with a vector of values
        // data.set_column(column_index, column_vector);
        void set_column(unsigned int y, std::vector<T> column_data)
        {
            for (size_t i = 0; i < column_data.size(); ++i)
            {
                data[i * columns + y] = column_data[i];
            }
        }

        // set the column names with a string vector
        void set_column_names(std::vector<std::string> column_names)
        {
            this->column_names = column_names;
        }

        // extract a row as a vector from a row index
        // data.get_row(row_index)
        std::vector<T> get_row(unsigned int x)
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

        // extract a column as a vector from a vector index
        // data.get_column(column_index)
        std::vector<T> get_column(unsigned int y)
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

        // load from file
        void load(std::string filepath, std::string sep = ",", bool has_headers = true)
        {
            // get dimensions to resize data set before actually
            // loading file
            get_dims_from_file(filepath, sep, has_headers);

            this->has_headers = has_headers;

            std::string current_line;
            unsigned int current_row = 0;

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
        void load(std::vector<T> data, std::vector<std::string> columns)
        {
            this->data = data;
            this->columns_names = columns;
            this->rows = this->count_rows();
            this->columns = this->count_columns();
        }

        // load from 1D vector (will be translated into column vector)
        void load(std::vector<T> data, std::string column_name)
        {
            std::vector<std::string> temp_column = { column_name };
            
            // set dimensions
            this->columns = 1;
            this->rows = data.size();

            // set 1D vector as column vector
            this->data.resize(data.size());
            this->set_column(0, data);
            this->column_names = temp_column;
        }

        void head(int rows = 10)
        {
            std::string cutoff_str;
            // if column vector exists, print columns before data
            if (column_names.size() > 0)
            {
                for (int c = 0; c < column_names.size(); ++c)
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
                for (int r = 0; r < rows; ++r)
                {
                    if (r >= this->count_rows())
                    {
                        break;
                    } // check if rows is larger than our data set
                    else
                    {
                        for (int c = 0; c < this->count_columns(); ++c)
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
                for (int r = 0; r < rows; ++r)
                {
                    if (r >= this->count_rows())
                    {
                        break;
                    } // check if rows is larger than our data set
                    else
                    {
                        for (int c = 0; c < this->count_columns(); ++c)
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
        DataSet<X> select(std::vector<int> indices)
        {
            unsigned int new_size = indices.size();

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

            subset.resize_columns(new_size);
            subset.resize_rows(this->count_rows());
            subset.set_column_names(new_columns);

            return subset;
        }
};

int main()
{
    DataSet<std::string> mydata;
    mydata.load("small_classification_test.csv");

    std::vector<int> idx = {0, 2};
    DataSet<std::string> subset = mydata.select<std::string>(idx);
    
    std::vector<int> idx2 = {1};
    DataSet<std::string> subset2 = subset.select<std::string>(idx2);

    subset2.head();
    return 0;
}