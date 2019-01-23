#pragma once

#include <standard_gates.h>

struct HalfAdder : public logic_element_t {
    XOR_t sum;
    AND_t carry;

    void set_A(logic_element_t* a_input) {
        sum.A.src   = a_input;
        carry.A.src = a_input;
    }

    void set_B(logic_element_t* b_input) {
        sum.B.src   = b_input;
        carry.B.src = b_input;
    }

    bool_t evaluate(void) override {
        // nothing more needs to be done here
        return false;
    }

    logic_element_t* get_sum(void) {
        return &sum;
    }

    logic_element_t* get_carry(void) {
        return &carry;
    }

};

struct FullAdder : public logic_element_t {

    HalfAdder upper_adder;
    HalfAdder lower_adder;
    OR_t      carry;

    bool_t evaluate(void) override { return false; }

    FullAdder(void) {
        upper_adder.set_B(lower_adder.get_sum());
        carry.A.src = lower_adder.get_carry();
        carry.B.src = upper_adder.get_carry();
    }

    logic_element_t* get_Co(void) { return &carry; }

    logic_element_t* get_sum(void) { return upper_adder.get_sum(); }

    void set_A(logic_element_t* a) { lower_adder.set_A(a); }

    void set_B(logic_element_t* b) { lower_adder.set_B(b); }

    void set_Ci(logic_element_t* c) { upper_adder.set_A(c); }

    void print(void) override {
        std::cout
            << upper_adder.sum.A.value
            << " "
            << lower_adder.sum.A.value
            << lower_adder.sum.B.value
            << " -> "
            << upper_adder.sum.output_value
            << carry.output_value << std::endl;
    }
};

struct FourBitAdder : public logic_element_t {

    FullAdder bits[4];

    FourBitAdder(void) {
        // connect all of the ripple-carry logic needed
        for(int i : {1, 2, 3})
            bits[i].set_Ci(bits[i-1].get_Co());
    }

    bool_t evaluate(void) override { return false; }

    void set_Ci(logic_element_t* ci) {
        bits[0].set_Ci(ci);
    }

    void set_A(
            logic_element_t* a0, logic_element_t* a1, 
            logic_element_t* a2, logic_element_t* a3) {
        bits[0].set_A(a0);
        bits[1].set_A(a1);
        bits[2].set_A(a2);
        bits[3].set_A(a3);
    }

    void set_B(
            logic_element_t* b0, logic_element_t* b1, 
            logic_element_t* b2, logic_element_t* b3) {
        bits[0].set_B(b0);
        bits[1].set_B(b1);
        bits[2].set_B(b2);
        bits[3].set_B(b3);
    }

    

};

