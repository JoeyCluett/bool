#pragma once

#include <standard_gates.h>

struct half_adder_t : public logic_element {
    xor_t sum;
    and_t carry;

    // A, B
    logic_element* inputs[2];
    bool input_values[2] = {false, false};    

    logic_element* get_sum(void) {
        return &sum;
    }

    logic_element* get_carry(void) {
        return &carry;
    }

    void set_a_input(logic_element* element) {
        sum.inputs[0]   = element;
        carry.inputs[0] = element;
        inputs[0] = element;
    }

    void set_b_input(logic_element* element) {
        sum.inputs[1]   = element;
        carry.inputs[1] = element;
        inputs[1] = element;
    }

    // the inputs are set during construction. we only
    // fetch local copies for this module
    void fetch_inputs(void) override {
        for(int i : {0, 1}) {
            if(inputs[i] != NULL)
                input_values[i] = inputs[i]->output_value;
        }
    }

    bool evaluate(void) override { return false; }

    void print(void) override {
        std::cout << "Half adder:\n"
                << "  A: " << input_values[0] 
                << "  B: " << input_values[1] 
                << std::endl
                << "Output:\n  Sum: "
                << sum.output_value
                << "  Carry: "
                << carry.output_value
                << std::endl;
    }

};

struct full_adder_t : public logic_element {

    half_adder_t upper_adder;
    half_adder_t lower_adder;
    or_t         carry_out;

    // Ci, A, B
    logic_element* inputs[3];
    bool input_values[3] = {false, false, false};

    full_adder_t(void) {
        // internal logic flow
        upper_adder.set_b_input(lower_adder.get_sum());
        carry_out.inputs[0] = upper_adder.get_carry();
        carry_out.inputs[1] = lower_adder.get_carry();
    }

    logic_element* get_sum(void) { return upper_adder.get_sum(); }
    logic_element* get_carry(void) { return lower_adder.get_carry(); }

    void set_carry_in(logic_element* ci) {
        inputs[0] = ci;
        upper_adder.set_a_input(ci);
    }

    void set_a_input(logic_element* a) {
        inputs[1] = a;
        lower_adder.set_a_input(a);
    }

    void set_b_input(logic_element* b) {
        inputs[2] = b;
        lower_adder.set_b_input(b);
    }

    void fetch_inputs(void) override {
        // save inputs for our own use
        for(int i : {0, 1, 2}) {
            input_values[i] = inputs[i]->output_value;
        }
    }

    // handled on the individual gate level
    bool evaluate(void) override {
        return false;
    }

    void print(void) {
        std::cout << "Full adder:\n"
                << "  Ci: " << input_values[0]
                << "  A: " << input_values[1]
                << "  B: " << input_values[2]
                << "\nOutput:\n"
                << "  Sum: " << upper_adder.get_sum()->output_value
                << "  Co: " << lower_adder.get_carry()->output_value
                << std::endl;
    }

};
