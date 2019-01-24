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

        // extra input on OR_4_t
        o1.inputs[3].src = LOW;
    }

    void set_A(logic_element_t* le) { x1.A.src = le; a1.inputs[1].src = le; }
    void set_B(logic_element_t* le) { x1.B.src = le; a2.inputs[1].src = le; }
    void set_Ci(logic_element_t* le) { n1.Y.src = le; o1.inputs[1].src = le; }

    logic_element_t* get_Co(void) { return &o1; }
    logic_element_t* get_Ao(void) { return &a1; }
    logic_element_t* get_Bo(void) { return &a2; }

    bool_t get_Co_value(void) { return o1.output_value; }
    bool_t get_Ao_value(void) { return a1.output_value; }
    bool_t get_Bo_value(void) { return a2.output_value; }

    bool_t get_Ci_value(void) { return n1.Y.value; }
    bool_t get_Ai_value(void) { return x1.A.value; }
    bool_t get_Bi_value(void) { return x1.B.value; }

    void print(void) {

        bool_t a_output = get_Ao_value();
        bool_t b_output = get_Bo_value();

        bool_t c_input = get_Ci_value();

        std::cout << "   A  B  Ci\n";
        std::cout << "i: ";
        std::cout << get_Ai_value() << "  ";
        std::cout << get_Bi_value() << "  ";
        std::cout << get_Ci_value() << "  ";
        std::cout << "\n";

        std::cout << "o: ";
        std::cout << get_Ao_value() << "  ";
        std::cout << get_Bo_value() << "  ";
        std::cout << get_Co_value() << "\n\n";

        if(!c_input) {
            if(!a_output && !b_output)
                std::cout << "A == B\n";
            else if(a_output && !b_output)
                std::cout << "A > B\n";
            else if(!a_output && b_output)
                std::cout << "A < B\n";
            else
                throw std::runtime_error("Something bad happened in the comparator");
        } else {
            std::cout << "X X X X\n";
        }
    }

};

struct Comparator_4 {
    // 4 seperate comparators linked together
    bit_comparator_t bits[4];

    OR_4_t or_a;
    OR_4_t or_b;

    Comparator_4(void) {
        // connect carries together
        for(int i : {2, 1, 0})
            bits[i].set_Ci(bits[i+1].get_Co());
        bits[3].set_Ci(LOW);

        // connect the OR'd outputs together
        for(int i : {0, 1, 2, 3}) {
            or_a.inputs[i].src = bits[i].get_Ao();
            or_b.inputs[i].src = bits[i].get_Bo();
        }
    }

    void set_A(std::array<logic_element_t*, 4> arr) { 
        for(int i : {0, 1, 2, 3}) 
            bits[i].set_A(arr[i]); 
    }

    void set_B(std::array<logic_element_t*, 4> arr) {
        for(int i : {0, 1, 2, 3}) 
            bits[i].set_B(arr[i]); 
    }

    void set_Ci(logic_element_t* el) { bits[3].set_Ci(el); }

    logic_element_t* get_Ao(void) { return &or_a; }
    logic_element_t* get_Bo(void) { return &or_b; }
    logic_element_t* get_Co(void) { return bits[0].get_Co(); }

    bool_t get_Ao_value(void) { return or_a.output_value; }
    bool_t get_Bo_value(void) { return or_b.output_value; }

    bool_t get_Ci_value(void) { return bits[3].get_Ci_value(); }

    void print(void) {
        std::cout << "Ai : ";
        for(int i : {3, 2, 1, 0})
            std::cout << bits[i].get_Ai_value();
        std::cout << "\nBi : ";
        for(int i : {3, 2, 1, 0})
            std::cout << bits[i].get_Bi_value();
        std::cout << "\n\n";
        std::cout << "Ci : " << bits[3].get_Ci_value() << "\n\n";

        bool_t a_output = get_Ao_value();
        bool_t b_output = get_Bo_value();
        bool_t c_input = get_Ci_value();

        if(!c_input) {
            if(!a_output && !b_output)
                std::cout << "A == B\n";
            else if(a_output && !b_output)
                std::cout << "A > B\n";
            else if(!a_output && b_output)
                std::cout << "A < B\n";
            else
                throw std::runtime_error("Something bad happened in the comparator");
        }
        else {
            std::cout << "X X X X\n";
        }


    }

};


