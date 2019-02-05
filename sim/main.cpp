#include <iostream>
#include <fstream>

// XML parsing utility
#include <XmlDocument.h>
#include <XmlNode.h>

// the base logic simulator utility
#include <standard_gates.h>

// modules for simulation
#include "main.h"

using namespace std;

// evaluates all of the import statements in the simulator file
void preprocess_simulator_files(std::string filename, std::ostream& os);

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "Usage: " << argv[0] << " <top level simulation file>" << endl;
        return 1;
    }

    std::map<std::string, SimulationModule*> module_map;

    std::ofstream ofile("/tmp/jsimcompiler_input_file.xml");
    ofile << "<root>\n";

    preprocess_simulator_files(argv[1], ofile);

    ofile << "</root>\n\n";
    ofile.close();

    XmlDocument doc("/tmp/jsimcompiler_input_file.xml");
    auto module = doc.root().child();

    cout << "Building logic modules...";

    while(!module.empty()) {
        
        if(module.name() == "module")
            // we need this to NOT be destroyed when it goes out of scope
            new SimulationModule(module, module_map);
        else
            throw std::runtime_error("Unknown tag: " + module.name());

        module = module.next();
    }

    cout << "DONE\n\n\n";

    // print the details of the modules placed here
    auto it = module_map.begin();
    while(it != module_map.end()) {

        cout << *(it->second);

        it++;
    }

    return 0;
}

void preprocess_simulator_files(std::string filename, std::ostream& os) {
    XmlDocument doc(filename);
    auto module = doc.root().child();

    while(!module.empty()) {
        if(module.name() == "import") {
            preprocess_simulator_files(module.attr("name").value() + ".xml", os);
        }
        else if(module.name() == "module") {
            module.format_output(os, "    ");
        }

        module = module.next();
    }
}