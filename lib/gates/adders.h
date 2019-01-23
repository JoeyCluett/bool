#pragma once

#include <standard_gates.h>
#include <array>

struct HalfAdder {
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

    logic_element_t* get_sum(void) { return &sum; }
    logic_element_t* get_carry(void) { return &carry; }

    bool_t get_sum_value(void) { return sum.output_value; }
    bool_t get_carry_value(void) { return carry.output_value; }
    bool_t get_A_input_value(void) { return sum.A.value; }
    bool_t get_B_input_value(void) { return sum.B.value; }

};

struct FullAdder {

    HalfAdder upper_adder;
    HalfAdder lower_adder;
    OR_t      carry;

    FullAdder(void) {
        upper_adder.set_B(lower_adder.get_sum());
        carry.A.src = lower_adder.get_carry();
        carry.B.src = upper_adder.get_carry();
    }

    // logical output gates
    logic_element_t* get_Co(void) { return &carry; }
    logic_element_t* get_sum(void) { return upper_adder.get_sum(); }

    // set inputs
    void set_A(logic_element_t* a) { lower_adder.set_A(a); }
    void set_B(logic_element_t* b) { lower_adder.set_B(b); }
    void set_Ci(logic_element_t* c) { upper_adder.set_A(c); }

    // retrieve output values
    bool_t get_A_input_value(void) { return lower_adder.get_A_input_value(); }
    bool_t get_B_input_value(void) { return lower_adder.get_B_input_value(); }
    bool_t get_Co_value(void) { return carry.output_value; }
    bool_t get_Ci_value(void) { return upper_adder.get_A_input_value(); }
    bool_t get_sum_value(void) { return upper_adder.get_sum_value(); }

    void print(void) {
        std::cout
            << get_Ci_value()
            << " "
            << get_A_input_value()
            << get_B_input_value()
            << " -> "
            << get_sum_value()
            << get_Co_value() << std::endl;
    }
};

struct FourBitAdder_RC {

    FullAdder bits[4];

    FourBitAdder_RC(void) {
        // connect all of the ripple-carry logic needed
        for(int i : {1, 2, 3})
            bits[i].set_Ci(bits[i-1].get_Co());
    }

    logic_element_t* get_Co(void) { return bits[3].get_Co(); }
    logic_element_t* get_Si(int bit) { return bits[bit].get_sum(); }

    bool_t get_Co_value(void) { get_Co()->output_value; }
    bool_t get_Si_value(int bit) { get_Si(bit)->output_value; }

    bool_t get_A_value(int bit) { return bits[bit].get_A_input_value(); }
    bool_t get_B_value(int bit) { return bits[bit].get_B_input_value(); }

    void set_Ci(logic_element_t* ci) { bits[0].set_Ci(ci); }
    void set_A(std::array<logic_element_t*, 4> a) {
        for(int i : {0, 1, 2, 3}) bits[i].set_A(a[i]);
    }
    void set_B(std::array<logic_element_t*, 4> b) {
        for(int i : {0, 1, 2, 3}) bits[i].set_B(b[i]);
    }

    void print(void) {
        std::cout << "  ";
        for(int i : {3, 2, 1, 0})
            std::cout << bits[i].lower_adder.sum.A.value;
        std::cout << std::endl;
    
        std::cout << "  ";
        for(int i : {3, 2, 1, 0})
            std::cout << bits[i].lower_adder.sum.B.value;
        std::cout << std::endl;

        std::cout << "     " << bits[0].upper_adder.sum.A.value << std::endl;
        std::cout << "-------\n";

        std::cout << bits[3].get_Co_value() << ' ';
        for(int i : {3, 2, 1, 0})
            std::cout << bits[i].upper_adder.get_sum_value();
        std::cout << std::endl;
    }

};

struct EightBitAdder_RC {
    FourBitAdder_RC lsb;
    FourBitAdder_RC msb;

    EightBitAdder_RC(void) {
        // connect carries from adder modules
        msb.set_Ci(lsb.get_Co());
    }

    logic_element_t* get_Co(void) { return msb.get_Co(); }
    logic_element_t* get_Si(int bit) {
        if(bit >= 4) // msb
            return msb.get_Si(bit-4);
        else // lsb
            return lsb.get_Si(bit);
    }

    bool_t get_Co_value(void) { return get_Co()->output_value; }
    bool_t get_Si_value(int bit) { return get_Si(bit)->output_value; }
    bool_t get_Ci_value(void) { return lsb.bits[0].upper_adder.get_A_input_value(); }

    bool_t get_A_value(int bit) {
        return (bit >= 4) ? 
            msb.get_A_value(bit-4)
            : lsb.get_A_value(bit); }
    bool_t get_B_value(int bit) {
        return (bit >= 4) ? 
            msb.get_B_value(bit-4)
            : lsb.get_B_value(bit); }

    void set_Ci(logic_element_t* le) { lsb.set_Ci(le); }
    void set_A(std::array<logic_element_t*, 8> a) {
        lsb.set_A({a[0], a[1], a[2], a[3]});
        msb.set_A({a[4], a[5], a[6], a[7]}); }
    void set_B(std::array<logic_element_t*, 8> b) {
        lsb.set_B({b[0], b[1], b[2], b[3]});
        msb.set_B({b[4], b[5], b[6], b[7]}); }
    
    void print(void) {
        std::cout << "  ";
        for(int i = 7; i >= 0; i--)
            std::cout << get_A_value(i);
        std::cout << std::endl;

        std::cout << "  ";
        for(int i = 7; i >= 0; i--)
            std::cout << get_B_value(i);
        std::cout << std::endl;

        std::cout << "         " << get_Ci_value() << std::endl;

        std::cout << "----------\n";

        std::cout << get_Co_value() << " ";
        for(int i = 7; i >= 0; i--)
            std::cout << get_Si_value(i);
        std::cout << std::endl;

    }

};
