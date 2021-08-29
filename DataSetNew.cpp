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
        int columns, rows;

        int get_x, get_y;

        std::vector<std::string> get_unique_columns(std::vector<std::string> const &columns)
        {
            std::vector<std::string> columns_copy = column_names;
            std::sort(columns_copy.begin(), columns_copy.end());
            columns_copy.erase(std::unique(columns_copy.begin(), columns_copy.end()), columns_copy.end());

            return columns_copy;
        }

        // load rows into data matrix
        void split(std::string text, int current_row, std::string sep = ",")
        {
            bool inside_string = false;
            int start_index = 0;
            int column_counter = 0;

            for (unsigned int i = 0; i < text.length(); i++)
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
            int start_index = 0;

            for (unsigned int i = 0; i < text.length(); i++)
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
        int count_columns_from_file(std::string text, std::string sep = ",")
        {
            bool inside_string = false;
            int start_index = 0;
            int column_counter = 0;

            for (unsigned int i = 0; i < text.length(); i++)
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
            int row_count = 0, column_count = 0;

            this->has_headers = has_headers;

            std::string current_line;
            int current_row = 0;

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

        DataSet(int x, int y) {
            data.resize(x*y);
            rows = x;
            columns = y;
        }

        void resize(int x, int y)
        {
            data.resize(x*y);
            rows = x;
            columns = y;
        }

        int count_rows()
        {
            return rows;
        }

        int count_columns()
        {
            return columns;
        }

        // extract a cell from data
        T &operator()(int x, int y)
        {
            get_x = x;
            get_y = y;
            return data[x * columns + y];
        }

        // set a value in the data after calling operator
        // data.set(x, y, value);
        void set(int x, int y, T value)
        {
            data[x * columns + y] = value;
        }

        // set a given row with a vector of values
        // data.set_row(row_index, row_vector);
        void set_row(int x, std::vector<T> row_data)
        {
            for (size_t i = 0; i < row_data.size(); ++i)
            {
                data[x * columns + i] = row_data[i];
            }
        }

        // set a given column with a vector of values
        // data.set_column(column_index, column_vector);
        void set_column(int y, std::vector<T> column_data)
        {
            for (size_t i = 0; i < column_data.size(); ++i)
            {
                data[i * columns + y] = column_data[i];
            }
        }

        // extract a row as a vector from a row index
        // data.get_row(row_index)
        std::vector<T> get_row(int x)
        {
            std::vector<T> return_vector(columns);
            get_x = x;
            get_y = -1;
            
            for (size_t i = 0; i < columns; ++i)
            {
                return_vector[i] = data[get_x * columns + i];
            }

            return return_vector;
        }

        // extract a column as a vector from a vector index
        // data.get_column(column_index)
        std::vector<T> get_column(int y)
        {
            std::vector<T> return_vector(rows);
            get_x = -1;
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
            int current_row = 0;

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
            this->columns = columns;
            this->rows = this->count_rows();
            this->columns = this->count_columns();
        }
};

int main()
{
    DataSet<std::string> mydata;
    mydata.load("small_classification_test.csv", ",", false);

    for (size_t r = 0; r < mydata.count_rows(); ++r)
    {
        for (size_t c = 0; c < mydata.count_columns(); ++c)
        {
            std::cout << mydata(r, c) << " ";
        }
        std::cout << "\n";
    }
    return 0;
}