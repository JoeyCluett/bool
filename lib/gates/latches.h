#pragma once

#include <standard_gates.h>

struct D_FlipFlop : public logic_element_t {
    logic_element_t* Din = NULL;
    logic_element_t* clk = NULL;
    LOGICAL_CONSTANT Q;
    LOGICAL_CONSTANT Q_not;

    bool_t current_clk;
    bool_t current_din;
    bool_t prev_clk = 0;

    D_FlipFlop(void) : logic_element_t(true) {
        Q.output_value = 0;
        Q_not.output_value = 1;
    }

    logic_element_t* get_Q(void) { return &Q; }
    logic_element_t* get_Q_not(void) { return &Q_not; }

    void set_D(logic_element_t* le) { Din = le; }
    void set_Clk(logic_element_t* le) { clk = le; }

    void pre_fetch(void) override {
        current_clk = clk->output_value ? 1 : 0;
        current_din = Din->output_value ? 1 : 0;
    }

    // called once every simulation cycle
    void pre_evaluate(void) override {

        // a behavioral description of a d flip-flop
        if(current_clk == 1 && prev_clk == 0) {
            // rising edge
            Q.output_value = current_din ? 1 : 0;
            Q_not.output_value = current_din ? 0 : 1;
        }

        // we need to track this regardless of what happens
        prev_clk = current_clk;
    }

    // overload the print function
    void print(void) override {
        std::cout << "Din: " << current_din << std::endl;
        std::cout << "Clk: " << current_clk << std::endl;
        std::cout << "Q:  " << Q.output_value << std::endl;
        std::cout << "Q`: " << Q_not.output_value << std::endl;
    }

};

// a clock element with an enable input, when the enable 
// is high, provides a periodic clock signal
struct Clock : public logic_element_t {
    input_port_t Enable;
    LOGICAL_CONSTANT clock_output;
    bool_t enable_input;

    void set_Enable(logic_element_t* le) { Enable.src = le; }
    logic_element_t* get_Output(void) { return &clock_output; }

    void pre_fetch(void) override {
        enable_input = Enable.src->output_value;
    }

    void pre_evaluate(void) override {
        // as long as enable is high
        if(enable_input)
            clock_output.output_value = clock_output.output_value ? 0 : 1;
    }

};
