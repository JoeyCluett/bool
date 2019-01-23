#include <iostream>
#include <standard_gates.h>
#include <adders.h>

using namespace std;

const char* bool_equiv(bool b) { return b ? "HIGH" : "LOW"; }

int main(int argc, char* argv[]) {

    int current_value = 0;

    cout << "Total number of logic gates: " << logic_element_t::total() << endl;

    while(1) {
        getchar();
        current_value++;

        cout << logic_element_t::simulate_to_steady() << " gate delays\n\n";
        //logic_element_t::simulate_single_delay();
        
    }

    return 0;
}