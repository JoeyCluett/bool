#include <iostream>
#include <standard_gates.h>
#include <latches.h>

#define NUM_LATCHES 16

using namespace std;

int main(int argc, char* argv[]) {

//    LOGICAL_CONSTANT clock_input;
    Clock clock_input;
    LOGICAL_CONSTANT d_in;
    
    D_FlipFlop flip_flop[NUM_LATCHES];

    for(int i = 1; i < NUM_LATCHES; i++)
        // chain data i/o together
        flip_flop[i].set_D(flip_flop[i-1].get_Q());

    for(int i = 0; i < NUM_LATCHES; i++)
        //flip_flop[i].set_Clk(&clock_input);
        flip_flop[i].set_Clk(clock_input.get_Output());

    // first data input
    flip_flop[0].set_D(&d_in);

    // set the first input
    /*clock_input.output_value = 1;
    d_in.output_value = 1;
    logic_element_t::simulate();
    d_in.output_value = 0;
    clock_input.output_value = 0;
    logic_element_t::simulate();
    */

    // have the shift register loop a single bit through
    flip_flop[0].set_D(flip_flop[NUM_LATCHES-1].get_Q());

    int iter = 0;
    while(1) {
        getchar();

        // clock every other cycle
        clock_input.output_value = iter & 1;
        logic_element_t::simulate();
        iter++;
        clock_input.output_value = iter & 1;

        cout << logic_element_t::simulate() << " gate delays...\n";

        for(int i = 0; i < NUM_LATCHES; i++)
            cout << flip_flop[i].Q.output_value;
        cout << "\n\n";

        iter++;
    }

    return 0;
}