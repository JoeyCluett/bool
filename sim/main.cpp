#include <iostream>

// XML parsing utility
#include <XmlDocument.h>
#include <XmlNode.h>

// the base logic simulator utility
#include <standard_gates.h>

// modules for simulation
#include "main.h"

using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "Usage: " << argv[0] << " <top level simulation file>" << endl;
        return 1;
    }

    std::map<std::string, SimulationModule*> module_map;

    XmlDocument doc(argv[1]);
    auto module = doc.root().child();

    cout << "Building logic modules...";

    while(!module.empty()) {
        
        new SimulationModule(module, module_map);

        module = module.next();
    }

    cout << "DONE\n";

    // print the details of the modules placed here
    auto it = module_map.begin();
    while(it != module_map.end()) {

        cout << *(it->second);

        it++;
    }

    return 0;
}
