#include <iostream>
#include <standard_gates.h>

using namespace std;

const char* bool_equiv(bool b) { return b ? "HIGH" : "LOW"; }

int main(int argc, char* argv[]) {

    //and_t and_gate;
    or_t and_gate;

    int current_value = 0;

    while(1) {
        getchar();
        current_value++;

        // set the inputs to the AND gate appropriately
        and_gate.input_values[0] = current_value & (1 << 0);
        and_gate.input_values[1] = current_value & (1 << 1);

        logic_element::global_eval();

        and_gate.print();

    }

    return 0;
}