#pragma once

#include <standard_gates.h>

struct D_FlipFlop : public logic_element_t {
    input_port_t Din;

    D_FlipFlop(void) 
            : logic_element_t(false) {
        
    }

    

};
