#pragma once

#include <standard_gates.h>
#include <array>

struct bit_comparator_t {

    OR_4_t o1;
    AND_3_t a1, a2;
    NOT_t n1;
    XOR_t x1;

    bit_comparator_t(void) {
        // carry out
        o1.inputs[0].src = &a1;
        o1.inputs[2].src = &a2;

        // test Ai
        a1.inputs[0].src = &n1;
        a1.inputs[1].src = NULL; // Ai
        a1.inputs[2].src = &x1;

        // test Bi
        a2.inputs[0].src = &x1;
        a2.inputs[1].src = NULL; // Bi
        a2.inputs[2].src = &n1;
    }

    void set_A(logic_element_t* le) { x1.A.src = le; a1.inputs[1].src = le; }
    void set_B(logic_element_t* le) { x1.B.src = le; a2.inputs[1].src = le; }
    void set_Ci(logic_element_t* le) { n1.Y.src = le; o1.inputs[1].src = le; }

    logic_element_t* get_Co(void) { return &o1; }
    logic_element_t* get_Ao(void) { return &a1; }
    logic_element_t* get_Bo(void) { return &a2; }

    bool_t get_Co_value(void) { return get_Co()->output_value; }
    bool_t get_Ao_value(void) { return get_Ao()->output_value; }
    bool_t get_Bo_value(void) { return get_Bo()->output_value; }

    bool_t get_Ci_value(void) { return n1.Y.value; }
    bool_t get_Ai_value(void) { return x1.A.value; }
    bool_t get_Bi_value(void) { return x1.B.value; }

    void print(void) {
        std::cout << "  " << get_Ai_value() << get_Bi_value() << std::endl;
        std::cout << get_Ci_value() << "  " << get_Co_value() << std::endl;
        std::cout << "----\n";
        std::cout << "  " << get_Ao_value() << get_Bo_value() << std::endl;
    }

};


