#include <iostream>
#include <standard_gates.h>
#include <adders.h>

using namespace std;

const char* bool_equiv(bool b) { return b ? "HIGH" : "LOW"; }

int main(int argc, char* argv[]) {

    int current_value = 0;

    logic_constant_t w, x, y, z;

    or_t or_1, or_2;
    and_t and_1;

    and_1.inputs[0] = &or_1;
    and_1.inputs[1] = &or_2;

    or_1.inputs[0] = &w;
    or_1.inputs[1] = &x;
    or_2.inputs[0] = &y;
    or_2.inputs[1] = &z;

    while(1) {
        getchar();
        current_value++;

        w.output_value = current_value & (1 << 0);
        x.output_value = current_value & (1 << 1);
        y.output_value = current_value & (1 << 2);
        z.output_value = current_value & (1 << 3);

        //logic_element::simulate_single_delay();
        logic_element::simulate_to_steady();

        cout << or_1.input_values[0] << or_1.input_values[1]
            << "  " << or_2.input_values[0] << or_2.input_values[1]
            << "  -> " << and_1.output_value << endl;
        cout << ' ' << and_1.input_values[0] << "   " << and_1.input_values[1]
            << endl;

        //cout << input_a.output_value << input_b.output_value << carry_in.output_value 
        //        << "  " << and_gate.output_value << endl;

        //adder.print();
    }

    return 0;
}