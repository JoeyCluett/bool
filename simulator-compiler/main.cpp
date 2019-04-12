#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>

// XML parsing utility
#include <XmlDocument.h>
#include <XmlNode.h>

#include "simulation_module.h"
#include "module_gate.h"
#include "module_instance.h"
#include "module_signal.h"
#include "module_input.h"
#include "new_module.h"

using namespace std;

// evaluates all of the import statements in the simulator file
void preprocess_simulator_files(std::string filename, std::ostream& os);

// create an XML file with all defined modules
//void create_global_xml_configuration(
//        simulation_module_map_t& module_map, 
//        std::string output_name);

int main(int argc, char* argv[]) {

/*    
    {
        string input = "(a:b:c):d:e:f";
        cout << input << endl;
       auto mat = ::split_config_string(input);
        cout << ::combine_config_string(mat) << endl;
    }
*/

    if(argc != 2) {
        cout << "Usage: " << argv[0] << " <top level simulation file>" << endl;
        return 1;
    }

    // global module map
    simulation_module_map_t module_map;

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
            //new SimulationModule(module, module_map);
            simulation_module_new(module, module_map);
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

    //create_global_xml_configuration(module_map, "/tmp/auto-generated-jsim.xml");

    // create packed binary formats of every module that was created
    //for(auto& mod : module_map)
    //    create_binary_module(mod.second, "/tmp/" + mod.second->get_module_name() + ".jsim");

    return 0;
}

std::vector<std::string> imported_files; // <module name="imported_file"/>

void preprocess_simulator_files(std::string filename, std::ostream& os) {
    XmlDocument doc(filename);
    auto module = doc.root().child();

    auto import_already_used = [](std::string cmp_str) -> bool {
        for(auto& str : imported_files)
            if(str == cmp_str)
                return true;
        return false;
    };  

    while(!module.empty()) {
        if(module.name() == "import") {
            // recursively evaluate all import statements exactly once

            cout << "importing (" << module.attr("name").value() << ") ...\n" << flush;

            if(!import_already_used(module.attr("name").value())) {
                imported_files.push_back(module.attr("name").value());
                preprocess_simulator_files(module.attr("name").value() + ".xml", os);
            }
        }
        else if(module.name() == "module") {
            module.format_output(os,   "    ");

            // print the modules as they are compiled
            //module.format_output(cout, "    ");
            //cout << flush;
        }
        else {
            stringstream ss;
            module.format_output(ss);

            throw std::runtime_error(
                "In file '" + filename + "', unknown tag '" 
                + module.name() + "' caught during preprocessing phase\n" 
                + ss.str());
        }

        module = module.next();
    }
}

/*
void create_global_xml_configuration(
        simulation_module_map_t& module_map, std::string output_name) {

    std::ofstream ofile(output_name);

    ofile << "<root>\n";
    
    for(auto& mod_name : SimulationModule::global_ordered_module_list)
        module_map.at(mod_name)->generate_xml_file(ofile, true);

    ofile << "</root>\n";

    ofile.close();
}
*/
