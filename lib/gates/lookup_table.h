#pragma once

#include <standard_gates.h>

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
