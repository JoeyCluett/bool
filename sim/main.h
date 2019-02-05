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
            {"OR", logic_type::OR},   {"NOR", logic_type::NOR}, 
            {"AND", logic_type::AND}, {"NAND", logic_type::NAND},
            {"XOR", logic_type::XOR}, {"XNOR", logic_type::XNOR},
            {"NOT", logic_type::NOT}
        };

        return logic_map.at(input);
    }

    // frequently need to split strings along colon seperators
    std::vector<std::string> split_string_by(std::string input, char c_delim) {
        std::vector<std::string> output;
        std::string build_string = "";

        for(char c : input) {
            if(c == c_delim) {
                output.push_back(build_string);
                build_string.clear();
            }
            else {
                build_string.push_back(c);
            }
        }

        output.push_back(build_string);
        return output;
    }

    auto split_colon_string(std::string input) -> std::vector<std::string> {
        return split_string_by(input, ':');
    }

    auto split_period_string(std::string input) -> std::vector<std::string> {
        return split_string_by(input, '.');
    }

    // each logic gate in a given simulation is tracked 
    // individually, this simply assists other modules 
    // in finding fully-qualified names for each gate
    std::map<std::string, logic_type> gate_map;

    // every instance needs an associated type as well
    std::map<std::string, std::string> instance_map;

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

        auto combine_inputs = [](std::vector<std::string>& v) -> std::string {
            std::string result = v.at(0);

            for(int i = 1; i < v.size(); i++) {
                result.push_back(':');
                result += v.at(i);
            }

            return result;
        };

        // jump to all of the child nodes
        n = n.child();
        while(!n.empty()) {

            if(n.name() == "gate") {
                //std::cout << "found logic gate in file...\n";

                // throw exception if false
                n.hasAttrs({"type", "name"}, true);
                n.hasOnlyAttrs({"type", "name"}, true);

                auto _name = n.attr("name").value();
                auto _type = this->logic_type_from_str(n.attr("type").value());

                this->gate_map.insert({_name, _type});
            }
            else if(n.name() == "instance") {
                //std::cout << "found instance in file...\n";
            
                n.hasAttrs({"type", "name"}, true);
                n.hasOnlyAttrs({"type", "name"}, true);

                auto _type = n.attr("type").value();
                auto _name = n.attr("name").value();

                // we need ot put this instance in the map as well
                this->instance_map.insert({_name, _type});

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
            else if(n.name() == "input") {
                //std::cout << "found input in file...\n";

                n.hasAttrs({"name", "to"}, true);
                n.hasOnlyAttrs({"name", "to"}, true);

                auto _name = n.attr("name").value();
                auto _to   = n.attr("to").value();

                std::vector<std::string> final_to_targets;

                // input could have multiple targets
                auto str_vec = this->split_colon_string(_to);
                for(auto& str : str_vec) {
                    //std::cout << str << std::endl;
                    
                    auto inner_str_vec = this->split_period_string(str);
                    
                    auto& a = inner_str_vec.at(0);
                    auto& b = inner_str_vec.at(1);

                    // test if final token is an input from a module further upstream
                    auto it = this->instance_map.find(a);
                    if(it != this->instance_map.end()) {
                        // is present in the instance map. we need to fetch 
                        // the correct SimulationModule from the module map
                        SimulationModule* _mod = NULL;

                        try {
                            _mod = module_map.at(it->second);
                        } catch(std::out_of_range& ex) {
                            throw std::runtime_error("Unable to find required module");
                        }

                        // we have the module. evaluate all of the inputs
                        auto upstream_vec = _mod->get_input_vector(b);
                        for(auto& upstream_str : upstream_vec) {
                            upstream_str = a + "." + upstream_str;
                            final_to_targets.push_back(upstream_str);
                            //std::cout << upstream_str << std::endl;
                        }
                    }
                    else {
                        // inputs already target lowest level elements
                        final_to_targets.push_back(str);
                    }
                }

                this->input_map.insert({_name, combine_inputs(final_to_targets)});
                //this->input_map.insert({_name, _to});
            }
            else if(n.name() == "output") {
                
            }
            else if(n.name() == "signal") {
                //std::cout << "signal found in file\n";

                // every signal must have these attributes
                n.hasAttrs({"from", "to"}, true);
                n.hasOnlyAttrs({"from", "to"}, true);

                auto _from = n.attr("from").value();
                auto _to   = n.attr("to").value();

                //std::cout << "  from -> " << _from << std::endl;
                //std::cout << "  to   -> " << _to << std::endl;

                // ==================================================
                // fully qualify the source gate
                auto src = this->split_period_string(_from);
                // find the underlying type of whatever this is
                auto src_iter = this->instance_map.find(src.at(0));
                if(src_iter != this->instance_map.end()) {
                    // this is a nested type
                    
                }
                else {
                    // this is a logic gate element

                }

                // ==================================================
                // fully qualify all of the dest gate ports

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

    auto get_input_vector(std::string input) -> std::vector<std::string> {
        auto str = this->input_map.at(input);
        return this->split_colon_string(str);
    }

    friend std::ostream& operator<<(std::ostream& os, SimulationModule& sm) {
        os << "module name: " << sm.module_name << std::endl;
        os << "\n\tgates:\n";

        {
            auto it = sm.gate_map.begin();
            while(it != sm.gate_map.end()) {
                os << "\t\t" << sm.logic_type_name(it->second) << " : " << it->first << std::endl;
                it++;
            }
        }

        os << "\n\tinputs:\n";

        {
            auto it = sm.input_map.begin();
            while(it != sm.input_map.end()) {
                os << "\t\t" << it->first << " ->\n";

                auto str_vec = sm.split_colon_string(it->second);
                for(auto& s : str_vec)
                    os << "\t\t\t" << s << std::endl;

                it++;
            }
        }

        os << "\n\n";

        return os;
    }

};
