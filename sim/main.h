#pragma once

#include <XmlDocument.h>
#include <XmlNode.h>
#include <map>

enum class logic_type : int {
    NONE, OR, NOR, AND, NAND, XOR, XNOR, NOT, FLIPFLOP
};

// internal representation of 
class SimulationModule {
private:

    std::string module_name;

    const char* logic_type_name(logic_type lt) {
        switch(lt) {
            case logic_type::NONE:
                return "NONE"; break;
            case logic_type::OR:
                return "OR  "; break;
            case logic_type::NOR:
                return "NOR "; break;
            case logic_type::AND:
                return "AND "; break;
            case logic_type::NAND:
                return "NAND"; break;
            case logic_type::XOR:
                return "XOR "; break;
            case logic_type::XNOR:
                return "XNOR"; break;
            case logic_type::NOT:
                return "NOT "; break;
            case logic_type::FLIPFLOP:
                return "FLIP-FLOP"; break;
            default:
                throw std::runtime_error("Unknown logic_type");
        }
    }

    logic_type logic_type_from_str(std::string input) {
        const std::map<std::string, logic_type> logic_map = {
            {"OR", logic_type::OR}, {"NOR", logic_type::NOR}, 
            {"AND", logic_type::AND}, {"NAND", logic_type::NAND},
            {"XOR", logic_type::XOR}, {"XNOR", logic_type::XNOR},
            {"NOT", logic_type::NOT}
        };

        return logic_map.at(input);
    }

    // frequently need to split strings along colon seperators
    std::vector<std::string> split_colon_string(std::string input) {
        std::vector<std::string> output;
        std::string build_string = "";

        const int STATE_default = 0;
        const int STATE_sep = 1;
        int STATE_current = STATE_default;

        for(char c : input) {
            switch(STATE_current) {
                case STATE_default:
                    if(c == ':') {
                        STATE_current = STATE_sep;
                    }
                    else {
                        build_string.push_back(c);
                    }
                    break;
                case STATE_sep:
                    output.push_back(build_string);
                    build_string.clear();
                    STATE_current = STATE_default;
                    break;
                default:
                    throw std::runtime_error("idk what just happened here");
            }
        }

        output.push_back(build_string);
        return output;
    }

    // each logic gate in a given simulation is tracked 
    // individually, this simply assists other modules 
    // in finding fully-qualified names for each gate
    std::map<std::string, logic_type> gate_map;

    // need to track how each gate in a given module 
    // is connected. otherwise, the simulation will 
    // throw a segfault every time it runs. this map 
    // needs to be iterated through when a module is 
    // instantiated to connect all of the inner pieces
    std::map<std::string, std::string> signal_map;

    // connections to the outside world. these are the 
    // only things that are available to outside modules
    std::map<std::string, std::string> input_map;
    std::map<std::string, std::string> output_map;

public:
    // generates an internal representation of a given module
    SimulationModule(XmlNode n, std::map<std::string, SimulationModule*>& module_map) {
        
        // the XmlNode passed as argument should already be a ready-to-go XmlDocument
        this->module_name = n.attr("name").value();

        // jump to all of the child nodes
        n = n.child();
        while(!n.empty()) {

            if(n.name() == "gate") {
                //std::cout << "\nfound logic gate in file...\n";

                // throw exception if false
                n.hasAttrs({"type", "name"}, true);
                n.hasOnlyAttrs({"type", "name"}, true);

                auto _name = n.attr("name").value();
                auto _type = this->logic_type_from_str(n.attr("type").value());

                this->gate_map.insert({_name, _type});
            }
            else if(n.name() == "instance") {
                //std::cout << "\nfound instance in file...\n";
            
                n.hasAttrs({"type", "name"}, true);
                n.hasOnlyAttrs({"type", "name"}, true);

                auto _type = n.attr("type").value();
                auto _name = n.attr("name").value();

                // now we have a module type that we need to go look for
                // std::map will throw an exception if module name is not found
                SimulationModule* _mod = NULL;
                try {
                    _mod = module_map.at(_type);
                } catch(std::out_of_range& ex) {
                    throw std::runtime_error(
                        "In module '" + this->module_name + "' unable to find entity '"
                        + _type + "' in module map");
                }

                // we know where the external module is, we need to go find all the infomation in it
                auto& gm = _mod->get_gate_map();

                auto it = gm.begin();
                while(it != gm.end()) {
                    this->gate_map.insert({_name + "." + it->first, it->second});
                    it++;
                }
            }

            n = n.next();
        }

        // last step is to make this module available globally
        if(module_map.find(this->module_name) != module_map.end()) {
            throw std::runtime_error(
                "SimulationModule -> module with name '" + 
                module_name + "' already exists");
        }
        else {
            module_map.insert({this->module_name, this});
        }
    }

    auto get_gate_map(void) -> std::map<std::string, logic_type>& {
        return this->gate_map;
    }

    friend std::ostream& operator<<(std::ostream& os, SimulationModule& sm) {
        os << "module name: " << sm.module_name << std::endl;
        os << "\n\tgates:\n";

        auto it = sm.gate_map.begin();
        while(it != sm.gate_map.end()) {
            os << "\t\t" << sm.logic_type_name(it->second) << " : " << it->first << std::endl;
            it++;
        }

        // for now, just show all of the gate-level details

        os << "\n\n";

        return os;
    }

};
