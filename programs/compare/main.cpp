#include <iostream>

// Logic sim stuff
#include <standard_gates.h>
#include <adders.h>
#include <comparator.h>

using namespace std;

int main(int argc, char* argv[]) {

    LOGICAL_CONSTANT inputs[3];

    bit_comparator_t comparator;

    comparator.set_A(inputs + 0);
    comparator.set_B(inputs + 1);
    comparator.set_Ci(inputs + 2);

    int iter = 0;
    while(true) {
        getchar();
        iter++;

        for(int i : {0, 1, 2})
            inputs[i].output_value = (iter & (1 << i)) ? 1 : 0;

        cout << logic_element_t::simulate_to_steady() << " cycles to steady...\n";

        comparator.print();
    }

    return 0;
}