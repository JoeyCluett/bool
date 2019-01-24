#include <iostream>
#include <standard_gates.h>
#include <latches.h>

using namespace std;

int main(int argc, char* argv[]) {

    LOGICAL_CONSTANT inputs[2];
    
    D_FlipFlop latch;
    latch.set_Data(inputs+0);
    latch.set_Clock(inputs+1);

    int iter = 0;
    while(1) {
        getchar();

        for(int i : {0, 1})
            inputs[i].output_value = (iter & (1 << i)) ? 1 : 0;

        cout << logic_element_t::simulate_to_steady() << " gate delays...\n";

        latch.print();

        iter++;
    }

    return 0;
}