#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <utility>

#include "simulation_module.h"

typedef std::vector<std::vector<std::string>> config_matrix_t;

bool is_positive_integer(std::string& s) {
    return (s.find_first_not_of("0123456789") == std::string::npos);
}

auto split_port_dest(std::string s) -> std::pair<std::string, std::string> {
    const int state_first = 0;
    const int state_second = 1;
    int current_state = state_first;

    std::string str_a, str_b;

    for(char c : s) {
        switch(current_state) {
            case state_first:
                if(c == '.')
                    current_state = state_second;
                else
                    str_a.push_back(c);
                break;
            case state_second:
                str_b.push_back(c);
                break;
            default:
                throw std::runtime_error("INTERNAL ERROR : UNKNOWN STATE IN ::split_port_dest");
        }
    }

    return { str_a, str_b };
}

auto split_colon_string(std::string input) -> std::vector<std::string> {
    // maintain backwards compatibility while adding paren functionality
    std::vector<std::string> output;
    std::string build_string = "";

    const int state_default = 0;
    const int state_multiple = 1;
    int state_current = state_default;

    for(char c : input) {
        switch(state_current) {
            case state_default:
                if(c == '(')
                    state_current = state_multiple;
                else if(c == ':') {
                    output.push_back(build_string);
                    build_string.clear();
                }
                else
                    build_string.push_back(c);
                break;
            case state_multiple:
                if(c == ')')
                    state_current = state_default;
                else
                    build_string.push_back(c);
                break;
            default:
                throw std::runtime_error("split_colon_string : unknown error");
        }
    }

    if(build_string.size() > 0)
        output.push_back(build_string);

    return output;
}

auto split_config_string(std::string input) -> std::vector<std::vector<std::string>> {
    std::vector<std::vector<std::string>> ret_obj;

    auto s_vec = split_colon_string(input);
    for(auto& str : s_vec)
        ret_obj.push_back(split_colon_string(str));

    return ret_obj;
}

auto combine_config_string(std::vector<std::vector<std::string>>& config_mat) -> std::string {
    std::string final_string;
    bool start = true;

    for(auto& s_vec : config_mat) {
        if(!start) final_string.push_back(':');

        if(s_vec.size() > 1) {
            final_string.push_back('(');
            final_string += s_vec.at(0);
            for(int i = 1; i < s_vec.size(); i++) final_string += (":" + s_vec[i]);
            final_string.push_back(')');
        }
        else
            final_string += s_vec.at(0);
        start = false;
    }

    return final_string;
}


