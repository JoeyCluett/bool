#pragma once

#include <standard_gates.h>

struct SR_Latch {

    NOR_t left, right;

    SR_Latch(void) { 
        left.B.src = &right; 
        right.A.src = &left; 
    }

    logic_element_t* get_Q(void) { return &left; }
    logic_element_t* get_QNot(void) { return &right; }

    bool_t get_Q_value(void) { return left.output_value; }
    bool_t get_QNot_value(void) { return right.output_value; }

    void set_S(logic_element_t* el) { right.B.src = el; }
    void set_R(logic_element_t* el) { left.A.src = el; }

    bool_t get_S_value(void) { return right.B.value; }
    bool_t get_R_value(void) { return left.A.value; }
};

struct SR_Latch_Enable {
    SR_Latch latch;
    AND_t left, right;

    SR_Latch_Enable(void) {
        latch.set_R(&left);
        latch.set_S(&right);
    }      

    void set_S(logic_element_t* el) { right.B.src = el; }
    void set_R(logic_element_t* el) { left.A.src = el; }
    void set_E(logic_element_t* el) { 
        left.B.src = el; 
        right.A.src = el;
    }

};

struct D_Latch {
    SR_Latch latch;
    NOT_t n;

    D_Latch(void) { latch.set_R(&n); }

    void set_D(logic_element_t* el) { n.Y.src = el; latch.set_S(el); }

};

struct D_Latch_Enable {
    SR_Latch latch;
    NAND_t a1, a2;
    NOT_t n;

    D_Latch_Enable(void) {
        a1.B.src = &n;
        latch.set_R(&a2);
        latch.set_S(&a1);
    }

    void set_D(logic_element_t* el) { n.Y.src = el; a2.A.src = el; }
    void set_E(logic_element_t* el) { a1.A.src = el; a2.B.src = el; }

    bool_t get_D_value(void) { return n.Y.value; }
    bool_t get_E_value(void) { return a2.B.value; }
    bool_t get_Q_value(void) { return latch.get_Q_value(); }
    bool_t get_QNot_value(void) { return latch.get_QNot_value(); }

    void print(void) {
        std::cout << "D: " << get_D_value() << ", E:  " << get_E_value() << std::endl;
        std::cout << "Q: " << get_Q_value() << ", Q': " << get_QNot_value() << std::endl;
    }

};
