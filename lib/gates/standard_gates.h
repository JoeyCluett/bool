#pragma once

#include <initializer_list>
#include <iostream>
#include <vector>
#include <stdexcept>

struct logic_element {

    static std::vector<logic_element*> this_vector;

    static void simulate_single_delay(void) {
        for(auto* le : this_vector) le->fetch_inputs();
        for(auto* le : this_vector) le->evaluate();
    }

    static void simulate_to_steady(void) {
        int sz = this_vector.size();
        for(int i = 0; i < sz; i++) {
            simulate_single_delay();
        }
    }

    static void global_eval(void) {
        for(auto* le : this_vector)
            le->evaluate();
    }

    bool output_value;
    bool get_output(void) {
        return output_value;
    }

    static int to_true_bool(int i) {
        // bools sometimes lie
        return i ? 1 : 0;
    }

    // constructor always adds *this to vector for 
    // simulation purposes
    logic_element(void) {
        this_vector.push_back(this);
    }

    // each logic element internally evaluates its own 
    // inputs. this allows for feedback loops. this is 
    // also where the magic of simulation happens
    // returns true if state of gate changed. this allows 
    //the simulator to find a steady state automatically
    virtual bool evaluate(void) = 0;
    
    // all logic elements need to fetch their respective 
    // inputs BEFORE evaluating
    virtual void fetch_inputs(void) = 0;

    // optionally define a print function for the logic 
    // element. this should display inputs and outputs
    virtual void print(void) { return; }

};

std::vector<logic_element*> logic_element::this_vector;

struct and_t : public logic_element {
    logic_element* inputs[2] = {NULL, NULL};
    bool input_values[2];

    void fetch_inputs(void) override {
        for(int i : {0, 1}) {
            if(inputs[i] != NULL)
                input_values[i] = to_true_bool(inputs[i]->get_output());
        }
    }

    bool evaluate(void) override {
        //std::cout << "input a : " << input_values[0] << ", input b : " << input_values[1];
        bool tmp = input_values[0] && input_values[1];
        //std::cout << ", output: " << tmp << std::endl;

        if(tmp != output_value) {
            // changed
            output_value = tmp;
            return true;
        }
        return false;
    }

    void print(void) override {
        std::cout << "AND gate:\n"
            << "  inputs: " << input_values[0] << input_values[1]
            << "  output: " << output_value
            << std::endl;
    }
};

struct or_t : public logic_element {
    logic_element* inputs[2] = {NULL, NULL};
    bool input_values[2] = {false, false};

    void fetch_inputs(void) override {
        for(int i : {0, 1}) {
            if(inputs[i] != NULL)
                input_values[i] = to_true_bool(inputs[i]->get_output());
        }
    }

    bool evaluate(void) override {
        bool tmp = input_values[0] || input_values[1];
        if(tmp != output_value) {
            output_value = tmp;
            return true;
        }
        return false;
    }

    void print(void) override {
        std::cout << "OR gate:\n"
            << "  inputs: " << input_values[0] << input_values[1]
            << "  output: " << output_value
            << std::endl;
    }
};

struct not_t : public logic_element {
    logic_element* input = NULL;
    bool input_value = false;

    void fetch_inputs(void) override {
        if(input != NULL)
            input_value = to_true_bool(input->output_value);
    }

    bool evaluate(void) override {
        bool tmp  = input_value ? false : true;
        if(tmp != output_value) {
            output_value = tmp;
            return true;
        }
        else {
            output_value = tmp;
            return false;
        }
    }

    void print(void) override {
        std::cout << "NOT gate: \n"
            << "  input: " << input_value
            << "  output: " << output_value
            << std::endl;
    }

};

struct xor_t : public logic_element {
    logic_element* inputs[2] = {NULL, NULL};
    bool input_values[2] = {false, false};

    void fetch_inputs(void) override {
        for(int i : {0, 1}) {
            if(inputs[i] != NULL)
                input_values[i] = to_true_bool(inputs[i]->get_output());
        }
    }

    bool evaluate(void) override {
        bool tmp = (input_values[0] ^ input_values[1]);
        if(tmp != output_value) {
            output_value = tmp;
            return true;
        }
        else {
            output_value = tmp;
            return false;
        }
    }

    void print(void) override {
        std::cout << "XOR gate:\n"
            << "  inputs: " << input_values[0] << input_values[1]
            << "  output: " << output_value
            << std::endl;
    }

};

struct xnor_t : public logic_element {
    logic_element* inputs[2] = {NULL, NULL};
    bool input_values[2] = {false, false};

    void fetch_inputs(void) override {
        for(int i : {0, 1}) {
            if(inputs[i] != NULL)
                input_values[i] = to_true_bool(inputs[i]->get_output());
        }
    }

    bool evaluate(void) override {
        bool tmp = (input_values[0] == input_values[1]);
        if(tmp != output_value) {
            output_value = tmp;
            return true;
        }
        else {
            output_value = tmp;
            return false;
        }
    }

    void print(void) override {
        std::cout << "XOR gate:\n"
            << "  inputs: " << input_values[0] << input_values[1]
            << "  output: " << output_value
            << std::endl;
    }

};

struct logic_constant_t : public logic_element {

    logic_constant_t(bool b = false) {
        output_value = b;
    }

    bool evaluate(void) override {
        // its a constant, nothing ever happens
        return false;
    }

    void fetch_inputs(void) override {
        return;
    }

    void print(void) override {
        std::cout << "Constant:\n"
            << "  value: " << output_value
            << std::endl;
    }

};

struct logic_buffer_t : public logic_element {
    logic_element* input = NULL;
    bool input_value = false;

    void fetch_inputs(void) override {
        if(input != NULL)
            input_value = to_true_bool(input->output_value);
    }

    bool evaluate(void) override {
        if(input_value != output_value) {
            output_value = input_value;
            return true;
        }
        return false;
    }

    void print(void) override {
        std::cout << "Buffer:\n"
                << "  value: " << output_value
                << std::endl;
    }

};

logic_constant_t logic_high(true);
logic_constant_t logic_low(false);
