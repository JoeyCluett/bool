#pragma once

#include <initializer_list>
#include <iostream>
#include <vector>
#include <stdexcept>

// tired of all the stupid zero/nonzero rules of bools
typedef int bool_t;

struct logic_element_t {

    static std::vector<logic_element_t*> this_vector;

    // maintains a single boolean value across function calls
    struct input_port {
        logic_element_t* src = NULL;
        bool_t value = 0;

        static std::vector<input_port*> this_vector;

        input_port(void) { this_vector.push_back(this); }
        
        static void fetch(void) {
            for(input_port* ip : this_vector) {
                if(ip != NULL)
                    ip->value = (ip->src->output_value ? 1 : 0);
            }
        }
    };

    // constructor always adds *this to vector for 
    // simulation purposes
    logic_element_t(void) { this_vector.push_back(this); }

    static void global_eval(void) {
        for(auto* le : this_vector)
            le->evaluate();
    }

    static void global_fetch(void) { input_port::fetch(); }

    static void simulate_single_delay(void) {
        global_fetch();
        for(auto* le : this_vector) le->evaluate();
    }

    static void simulate_to_steady(void) {
        int sz = this_vector.size();
        for(int i = 0; i < sz; i++) {
            simulate_single_delay();
        }
    }

    // used by EVERY logic gate
    bool_t output_value;

    // strictly speaking, bools are zero and non-zero
    static bool_t to_true_bool(bool_t i) {
        // bools sometimes lie
        return i ? 1 : 0;
    }

    // each logic element internally evaluates its own 
    // inputs. this allows for feedback loops. this is 
    // also where the magic of simulation happens
    // returns true if state of gate changed. this allows 
    //the simulator to find a steady state automatically
    virtual bool_t evaluate(void) = 0;

    // optionally define a print function for the logic 
    // element. this should display inputs and outputs
    virtual void print(void) { return; }

};

std::vector<logic_element_t*> logic_element_t::this_vector;
std::vector<logic_element_t::input_port*> logic_element_t::input_port::this_vector;

typedef logic_element_t::input_port input_port_t;

struct LOGICAL_CONSTANT : public logic_element_t {
    LOGICAL_CONSTANT(bool_t v = false) {
        output_value = v;
    }

    bool_t evaluate(void) override {
        return false;
    }

};

// used as global constants
LOGICAL_CONSTANT logic_high_t(1);
LOGICAL_CONSTANT logic_low_t(0);

struct AND_t : public logic_element_t {

    input_port_t A, B;

    bool_t evaluate(void) override {
        bool_t tmp = A.value & B.value;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }
};

struct OR_t : public logic_element_t {

    input_port_t A, B;

    bool_t evaluate(void) override {
        bool_t tmp = A.value | B.value;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }
};

struct XOR_t : public logic_element_t {

    input_port_t A, B;

    bool_t evaluate(void) override {
        bool_t tmp = A.value ^ B.value;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }
};

