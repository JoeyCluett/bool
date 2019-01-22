#pragma once

#include <initializer_list>
#include <iostream>
#include <vector>

struct logic_element {

    static std::vector<logic_element*> this_vector;

    static void simulate(void) {
        for(auto* le : this_vector) le->fetch_inputs();
        for(auto* le : this_vector) le->evaluate();
    }

    static void global_eval(void) {
        for(auto* le : this_vector)
            le->evaluate();
    }

    bool output_value;
    bool get_output(void) {
        return output_value;
    }

    // constructor always adds *this to vector for 
    // simulation purposes
    logic_element(void) {
        this_vector.push_back(this);
    }

    // each logic element internally evaluates its own 
    // inputs. this allows for feedback loops. this is 
    // also where the magic of simulation happens
    virtual void evaluate(void) = 0;
    
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
                input_values[i] = inputs[i]->get_output();
        }
    }

    void evaluate(void) override {
        output_value = input_values[0] && input_values[1];
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
                input_values[i] = inputs[i]->get_output();
        }
    }

    void evaluate(void) override {
        output_value = input_values[0] || input_values[1];
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
            input_value = input->output_value;
    }

    void evaluate(void) override {
        output_value = input_value ? false : true;
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
                input_values[i] = inputs[i]->get_output();
        }
    }

    void evaluate(void) override {
        output_value = (input_values[0] != input_values[1]);
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
                input_values[i] = inputs[i]->get_output();
        }
    }

    void evaluate(void) override {
        output_value = (input_values[0] == input_values[1]);
    }

    void print(void) override {
        std::cout << "XOR gate:\n"
            << "  inputs: " << input_values[0] << input_values[1]
            << "  output: " << output_value
            << std::endl;
    }

};
