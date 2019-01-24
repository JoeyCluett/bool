#include <iostream>

// Logic sim stuff
#include <standard_gates.h>
#include <adders.h>
#include <comparator.h>
#include <util.h>

using namespace std;

int main(int argc, char* argv[]) {

    LOGICAL_CONSTANT inputs[9];

    Comparator_4 comparator;

    comparator.set_A({inputs+0, inputs+1, inputs+2, inputs+3}); // ptrs
    comparator.set_B({inputs+4, inputs+5, inputs+6, inputs+7}); // ftw
    comparator.set_Ci(inputs+8);

    auto start_time = UsecTimestamp() / 1000;

    int iter = 0;
    while(iter < 100000) {
        //getchar();
        iter++;

        for(int i = 0; i < 9; i++)
            inputs[i].output_value = (iter & (1 << i)) ? 1 : 0;

        cout << endl << logic_element_t::simulate_to_steady() << " delays to steady...\n";

        comparator.print();
    }

    auto end_time = UsecTimestamp() / 1000;

    cout << "Total time for simulation: " << (end_time - start_time) << " milliseconds\n" << endl;

    return 0;
}