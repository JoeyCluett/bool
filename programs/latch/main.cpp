#include <iostream>
#include <standard_gates.h>
#include <latches.h>

using namespace std;

int main(int argc, char* argv[]) {

    LOGICAL_CONSTANT inputs[2];
    
    D_FlipFlop latch;
    latch.set_D(inputs+1);
    latch.set_Clk(inputs+0);

    cout << "Size of logic_element: " << sizeof(logic_element_t) << endl;

    int iter = 0;
    while(1) {
        getchar();

        for(int i : {0, 1})
            inputs[i].output_value = (iter & (1 << i)) ? 1 : 0;

        //cout << logic_element_t::simulate_to_steady() << " gate delays...\n";
        cout << logic_element_t::simulate() << " gate delays...\n";

        latch.print();

        iter++;
    }

    return 0;
}