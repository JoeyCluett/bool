#include <iostream>
#include <standard_gates.h>
#include <adders.h>

using namespace std;

const char* bool_equiv(bool b) { return b ? "HIGH" : "LOW"; }

int main(int argc, char* argv[]) {

    int current_value = 0;

    FullAdder ha;

    LOGICAL_CONSTANT lca, lcb, lcc;

    ha.set_A(&lca);
    ha.set_B(&lcb);
    ha.set_Ci(&lcc);

    while(1) {
        getchar();
        current_value++;

        // change the constants
        lca = current_value & 0x01;
        lcb = current_value & 0x02;
        lcc = current_value & 0x04;

        logic_element_t::simulate_to_steady();

        ha.print();
    }

    return 0;
}