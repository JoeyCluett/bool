#include <iostream>
#include <standard_gates.h>
#include <adders.h>

using namespace std;

const char* bool_equiv(bool b) { return b ? "HIGH" : "LOW"; }

int main(int argc, char* argv[]) {

    int current_value = 0;

    LOGICAL_CONSTANT inputs[4];

    OR_t   or_gate;
    NOR_t  nor_gate;
    AND_t  and_gate;
    NAND_t nand_gate;
    XOR_t  xor_gate;
    XNOR_t xnor_gate;

    vector<logic_element_t*> ptr_list = {
            &or_gate, &nor_gate, &and_gate, 
            &nand_gate, &xor_gate, &xnor_gate};

    or_gate.A.src = inputs+0;
    nor_gate.A.src = inputs+0;
    and_gate.A.src = inputs+0;
    nand_gate.A.src = inputs+0;
    xor_gate.A.src = inputs+0;
    xnor_gate.A.src = inputs+0;

    or_gate.B.src = inputs+1;
    nor_gate.B.src = inputs+1;
    and_gate.B.src = inputs+1;
    nand_gate.B.src = inputs+1;
    xor_gate.B.src = inputs+1;
    xnor_gate.B.src = inputs+1;

    cout << "Total number of logic gates: " << logic_element_t::total() << endl;

    while(1) {
        getchar();
        current_value++;

        for(int i : {0, 1})
            inputs[i].output_value = current_value & (1 << i);

        cout << logic_element_t::simulate_to_steady() << " gate delays\n\n";
        //logic_element_t::simulate_single_delay();
        
        for(auto* le : ptr_list)
            le->print();
    }

    return 0;
}