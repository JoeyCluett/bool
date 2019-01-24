#include <iostream>

// Logic sim stuff
#include <standard_gates.h>
#include <adders.h>
#include <comparator.h>

using namespace std;

int main(int argc, char* argv[]) {

    LOGICAL_CONSTANT inputs[8];

    Comparator_4 comparator;

    comparator.set_A({inputs+0, inputs+1, inputs+2, inputs+3});
    comparator.set_A({inputs+4, inputs+5, inputs+6, inputs+7});
    comparator.set_Ci(LOW);

    int iter = 0;
    while(true) {
        getchar();
        iter++;

        for(int i = 0; i < 8; i++)
            inputs[i].output_value = (iter & (1 << i)) ? 1 : 0;

        cout << logic_element_t::simulate_to_steady() << " cycles to steady...\n";

        comparator.print();
    }

    return 0;
}