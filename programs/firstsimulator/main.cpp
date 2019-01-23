#include <iostream>
#include <standard_gates.h>
#include <adders.h>

using namespace std;

const char* bool_equiv(bool b) { return b ? "HIGH" : "LOW"; }

int main(int argc, char* argv[]) {

    int current_value = 0;


    // AAAA, BBBB, Ci
    LOGICAL_CONSTANT inputs[9];

    FourBitAdder fba;

    cout << "Attaching logical inputs...";
    fba.set_A({inputs+0, inputs+1, inputs+2, inputs+3});
    fba.set_B({inputs+4, inputs+5, inputs+6, inputs+7});
    fba.set_Ci(inputs+8);
    cout << "DONE\n";

    while(1) {
        getchar();
        current_value++;

        // change the constants
        for(int i = 0; i < 9; i++)
            inputs[i].output_value = current_value & (1 << i);

        logic_element_t::simulate_to_steady();
        //logic_element_t::simulate_single_delay();

        fba.print();
    }

    return 0;
}