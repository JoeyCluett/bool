#include <iostream>

#include <sim/binary_format.h>
#include <sim/module_instance.h>

using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "Usage: \n  " << argv[0] << " <binary jsim file>\n";
        return 1;
    }

    ModuleInstance mi(argv[1]);

    mi.print();

    return 0;
}
