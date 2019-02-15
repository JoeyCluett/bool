#pragma once

#include <standard_gates.h>
#include <vector>

struct bool_array_t {
    // global storage of bits
    static std::vector<bool> global_bit_table;
    size_t sz = 0;
    int start_index = -1;

    bool_array_t(std::vector<bool> bits) {
        this->start_index = global_bit_table.size();
        this->sz = bits.size();

        for(bool b : bits)
            global_bit_table.push_back(b);
    }

};

// all data for every bool_array_t is stored 
// here. this helps keep the memory footprint low
std::vector<bool> bool_array_t::global_bit_table;

// compare 2 bool arrays directly
inline bool operator==(bool_array_t& lhs, bool_array_t& rhs) {
    if(&lhs == &rhs)
        return true; // objects are always equivalent to themselves

    if(lhs.sz != rhs.sz)
        return false; // to be equivalent, must have same number of elements

    std::vector<bool>& bool_vec = bool_array_t::global_bit_table;

    // easy comparisons are done
    int sz = lhs.sz;
    for(int i = 0; i < sz; i++) {
        if(bool_vec[lhs.start_index + i] != bool_vec[rhs.start_index + i])
            return false;
    }

    return true;
}

// compare a bool_array_t and a std::vector<bool>
inline bool operator==(bool_array_t& lhs, std::vector<bool>& rhs) {
    if(lhs.sz != rhs.size())
        return false;

    int sz = lhs.sz;
    for(int i = 0; i < sz; i++) {
        if(rhs[i] != bool_array_t::global_bit_table[lhs.start_index + i])
            return false;
    }

    return true;
}

bool operator==(std::vector<bool>& lhs, bool_array_t& rhs) {
    return rhs == lhs;
}

struct lookup_table : public logic_element_t {

    size_t input_size = 0, output_size = 0;
    logic_element_t** input_array = NULL;
    LOGICAL_CONSTANT* output_array = NULL;

    lookup_table(size_t input_size, size_t output_size) {
        this->input_size = input_size;
        input_array = new logic_element_t*[input_size];

        this->output_size = output_size;
        output_array = new LOGICAL_CONSTANT[output_size];
    }

    logic_element_t* get_output(int index) {
        return (output_array + index);
    }

    void set_input(int index, logic_element_t* le_ptr) {
        input_array[index] = le_ptr;
    }

};
