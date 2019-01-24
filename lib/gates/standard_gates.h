#pragma once

#include <initializer_list>
#include <iostream>
#include <vector>
#include <stdexcept>

// tired of all the stupid zero/nonzero rules of bools
typedef int bool_t;

struct logic_element_t {

    static std::vector<logic_element_t*> this_vector;
    static size_t total_gates; // not all gates are important
    static size_t total(void) { return total_gates; }

    // maintains a single boolean value across function calls
    struct input_port {
        logic_element_t* src = NULL;
        bool_t value = 0;

        static std::vector<input_port*> this_vector;

        input_port(void) { this_vector.push_back(this); }
        
        static void fetch(void) {
            std::cout << "Fetching inputs...\n" << std::flush;
            int iter = 1;
            for(input_port* ip : this_vector) {
                std::cout << iter++ << " " << (void*)ip << std::endl << std::flush;
                if(ip->src != NULL) {
                    ip->value = (ip->src->output_value ? 1 : 0);
                } else {
                    throw std::logic_error("input_port not connected");
                }
            }
            std::cout << "DONE\n" << std::flush;
        }

        static auto begin(void) -> std::vector<input_port*>::iterator {
            return this_vector.begin();
        }

        static auto end(void) -> std::vector<input_port*>::iterator {
            return this_vector.end();
        }

    };

    // constructor always adds *this to vector for 
    // simulation purposes
    logic_element_t(bool add_to_ct = false) { 
        this_vector.push_back(this); 
        if(add_to_ct)
            total_gates++;
    }

    static void global_eval(void) {
        for(auto* le : this_vector)
            le->evaluate();
    }

    static void global_fetch(void) { input_port::fetch(); }

    // returns true if unsteady, false otherwise
    static bool_t simulate_single_delay(void) {
        global_fetch();
        bool_t unsteady = 0;
        for(auto* le : this_vector) {
            if(le->evaluate())
                unsteady = 1;
        }
        return unsteady;
    }

    // simulate to steady state, for single gate 
    // delays, use above method
    static int simulate_to_steady(void) {
        int total_iters = 0;
        while(simulate_single_delay()) {
            //std::cout << '#';
            total_iters++;
        }
        return total_iters;
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
size_t logic_element_t::total_gates = 0L;

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

auto* LOW  = &logic_low_t;
auto* HIGH = &logic_high_t;

struct NOT_t : public logic_element_t {
    input_port_t Y;

    NOT_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = Y.value ? 0 : 1;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout << "NOT : " << Y.value << " -> " << output_value << std::endl;
    }
};

struct AND_t : public logic_element_t {
    input_port_t A, B;
    
    AND_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = A.value & B.value;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout 
                << "AND(2) : "
                << A.value << B.value
                << " -> " << output_value << std::endl;
    }

};

struct AND_3_t : public logic_element_t {
    input_port_t inputs[3];
    AND_3_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = inputs[0].value & inputs[1].value & inputs[2].value;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout << "AND(3) : ";
        for(int i : {0, 1, 2})
            std::cout << inputs[i].value;
        std::cout << " -> " << output_value << std::endl;
    }

};

struct NAND_t : public logic_element_t {
    input_port_t A, B;

    NAND_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = (A.value & B.value) ? 0 : 1;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout 
                << "NAND(2) : "
                << A.value << B.value
                << " -> " << output_value << std::endl;
    }
};

struct OR_t : public logic_element_t {
    input_port_t A, B;

    OR_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = A.value | B.value;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout 
                << "OR(2) : "
                << A.value << B.value
                << " -> " << output_value << std::endl;
    }
};

struct OR_4_t : public logic_element_t {

    input_port_t inputs[4];

    OR_4_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = 0;

        tmp = 
            inputs[0].value | inputs[1].value | 
            inputs[2].value | inputs[3].value;

        if(tmp != output_value) {
            output_value = tmp;
            //std::cout << "OR4 changed...\n";
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout << "OR(4) : ";
        for(int i : {0, 1, 2, 3})
            std::cout << inputs[i].value;
        std::cout << " -> " << output_value << std::endl;
    }

};

struct NOR_t : public logic_element_t {

    input_port_t A, B;

    NOR_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = (A.value | B.value) ? 0 : 1;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout 
                << "NOR(2) : "
                << A.value << B.value
                << " -> " << output_value << std::endl;
    }
};

struct XOR_t : public logic_element_t {

    input_port_t A, B;

    XOR_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = A.value ^ B.value;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout 
                << "XOR(2) : "
                << A.value << B.value
                << " -> " << output_value << std::endl;
    }
};

struct XNOR_t : public logic_element_t {

    input_port_t A, B;

    XNOR_t(void) : logic_element_t(true) { ; }

    bool_t evaluate(void) override {
        bool_t tmp = (A.value ^ B.value) ? 0 : 1;
        if(tmp != output_value) {
            output_value = tmp;
            return 1;
        }
        return 0;
    }

    void print(void) {
        std::cout 
                << "XNOR(2) : "
                << A.value << B.value
                << " -> " << output_value << std::endl;
    }
};
