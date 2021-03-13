#ifndef FILTERDATA_H
#define FILTERDATA_H
#include <iostream>
#include <string.h>
#include <vector>
#include <math.h>
#include <string_view>
using namespace std;

class FilterData
{
private:
    int evaluationFinal;

    bool beginningGroup = false;
    std::vector<std::string> columns = {"col1", "col2", "col3", "col4", "col5"};

    // used for double comparison
    bool essentiallyEqual(double a, double b, double epsilon)
    {
        return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }

    // for string comparison
    bool ends_with(std::string_view str, std::string_view suffix)
    {
        return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    }

    bool starts_with(std::string_view str, std::string_view prefix)
    {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
    }

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

    int update_evaluation(int evaluation, int index, std::string current_logical, bool condition_matched)
    {
        if (condition_matched)
        {
            if (index == 0)
            {
                evaluation = 1;
            }
            else if (current_logical.compare("OR") == 0)
            {
                evaluation += 1;
            }
            else if (current_logical.compare("AND") == 0)
            {
                evaluation *= 1;
            }
        }
        else
        {
            if (index == 0)
            {
                evaluation = 0;
            }
            else if (current_logical.compare("OR") == 0)
            {
                evaluation += 0;
            }
            else if (current_logical.compare("AND") == 0)
            {
                evaluation *= 0;
            }
        }

        return evaluation;
    }

    int check_condition(
        int evaluation,
        std::vector<std::string> data_row,
        int col_index,
        std::vector<std::vector<std::string>> conditions,
        int condition_index,
        std::string current_logical)
    {
        if (conditions[condition_index][1].compare(">=") == 0)
        {
            if (isgreaterequal(stod(data_row[col_index]), stod(conditions[condition_index][2])))
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
        }
        else if (conditions[condition_index][1].compare("<=") == 0)
        {
            if (islessequal(stod(data_row[col_index]), stod(conditions[condition_index][2])))
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
        }
        else if (conditions[condition_index][1].compare("=") == 0)
        {
            if (data_row[col_index].compare(conditions[condition_index][2]) == 0)
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
        }
        else if (conditions[condition_index][1].compare("!=") == 0)
        {
            if (data_row[col_index].compare(conditions[condition_index][2]) != 0)
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
        }
        else if (conditions[condition_index][1].compare("contains") == 0)
        {
            if (data_row[col_index].find(conditions[condition_index][2]) != std::string::npos)
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
        }
        else if (conditions[condition_index][1].compare("not contains") == 0)
        {
            if (data_row[col_index].find(conditions[condition_index][2]) != std::string::npos)
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
        }
        else if (conditions[condition_index][1].compare("starts with") == 0 || conditions[condition_index][1].compare("begins with") == 0)
        {
            if (starts_with(data_row[col_index], conditions[condition_index][2]))
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
        }
        else if (conditions[condition_index][1].compare("ends with") == 0)
        {
            if (ends_with(data_row[col_index], conditions[condition_index][2]))
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, true);
            }
            else
            {
                evaluation = update_evaluation(evaluation, condition_index, current_logical, false);
            }
        }

        return evaluation;
    }

    // used when evaluating inner conditions inside groups
    std::string evaluate_condition(std::vector<std::string> data_row, std::vector<std::vector<std::string>> conditions)
    {
        int evaluation;
        int col_index;
        std::string current_logical = "";

        for (int c = 0; c < conditions.size(); ++c)
        {
            col_index = get_column_index(conditions[c][0]);
            evaluation = check_condition(evaluation, data_row, col_index, conditions, c, current_logical);

            // add any logicals at end of statement
            if (conditions[c].size() == 4)
            {
                current_logical = conditions[c][3];
            }
            else
            {
                current_logical = "";
            }
        }

        return to_string(evaluation);
    }

    int compute_conditions(std::vector<std::vector<std::string>> data, int data_index, std::vector<std::vector<std::string>> conditions)
    {
        int evaluation;
        int col_index;
        std::string current_logical = "";
        // evaluate condition list linearly after all groups have been computed
        for (int i = 0; i < conditions.size(); ++i)
        {
            // format: COL COND X LOGICAL (e.g col1 = 5 AND)
            if (conditions[i].size() == 4)
            {
                col_index = get_column_index(conditions[i][0]);
                evaluation = check_condition(evaluation, data[data_index], col_index, conditions, i, current_logical);

                current_logical = conditions[i][3];
            }
            // singular values (computed from groups)
            // excluding blank closing bracket ")""
            else if (conditions[i].size() == 1 && conditions[i][0].compare(")") != 0)
            {
                if (current_logical.compare("OR") == 0)
                {
                    evaluation += stoi(conditions[i][0]);
                }
                else if (current_logical.compare("AND") == 0)
                {
                    evaluation *= stoi(conditions[i][0]);
                }
            }
            // end group logicals (e.g ) AND, ) OR, etc)
            else if (conditions[i].size() == 2)
            {
                current_logical = conditions[i][1];
            }
            // format: COL COND X (e.g col1 = 5); no logicals (usually at end of statements)
            else if (conditions[i].size() == 3)
            {
                col_index = get_column_index(conditions[i][0]);
                evaluation = check_condition(evaluation, data[data_index], col_index, conditions, i, current_logical);
            }
        }
        return evaluation;
    }

    int evaluationMain;

    void condition_parser(std::vector<std::vector<std::string>> data, int data_index, std::vector<std::vector<std::string>> conditions)
    {
        // condition vector to compute inner groups
        std::vector<std::vector<std::string>> inner_conditions;
        int beginning_group_index, end_group_index;
        bool contains_group = false;
        int evaluation;
        int return_val;

        // final vector to return
        std::vector<std::vector<std::string>> filtered_data;

        // check for groups
        for (int i = 0; i < conditions.size(); ++i)
        {
            if (conditions[i][0].compare("(") == 0)
            {
                beginningGroup = true;
                beginning_group_index = i;
                contains_group = true;
            }
            else if (conditions[i][0].compare(")") == 0 && beginningGroup)
            {
                beginningGroup = false;
                end_group_index = i;
                std::string cond_eval = evaluate_condition(data[data_index], inner_conditions);
                std::vector<std::string> cond_eval_vec;
                cond_eval_vec.push_back(cond_eval);

                conditions.erase(conditions.begin() + beginning_group_index, conditions.begin() + end_group_index);
                conditions.insert(conditions.begin() + beginning_group_index, cond_eval_vec);
                condition_parser(data, data_index, conditions);
                break;
            }
            else if (conditions[i].size() == 3 || conditions[i].size() == 4)
            {
                // if we're inside a group, add them to inner_conditions
                if (beginningGroup)
                {
                    inner_conditions.push_back(conditions[i]);
                }
            }
        }

        if (!contains_group)
        {
            // evaluate condition list linearly after computing groups first
            evaluationFinal = compute_conditions(data, data_index, conditions);
        }
    }

public:
    std::vector<std::vector<std::string>> filter(std::vector<std::vector<std::string>> data, std::vector<std::vector<std::string>> conditions)
    {
        int eval;
        std::vector<std::vector<std::string>> filtered_data;

        // this method will set private member "evaluationFinal" to 0 or 1 to exclude/indclude records
        for (int i = 0; i < data.size(); ++i)
        {
            condition_parser(data, i, conditions);
            eval = evaluationFinal;
            if (eval > 0)
            {
                filtered_data.push_back(data[i]);
            }
        }

        return filtered_data;
    }
};
#endif