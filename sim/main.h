#pragma once

#include <XmlDocument.h>
#include <XmlNode.h>
#include <map>
#include <sstream>

enum class logic_type : int {
    NONE, OR, NOR, AND, NAND, XOR, XNOR, NOT, FLIPFLOP
};

// internal representation of a module as defined in jsim standard
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

    // compatible with XML jsim configuration file
    const char* logic_type_name_proper(logic_type lt) {
        switch(lt) {
            case logic_type::NONE:
                return "NONE"; break;
            case logic_type::OR:
                return "OR"; break;
            case logic_type::NOR:
                return "NOR"; break;
            case logic_type::AND:
                return "AND"; break;
            case logic_type::NAND:
                return "NAND"; break;
            case logic_type::XOR:
                return "XOR"; break;
            case logic_type::XNOR:
                return "XNOR"; break;
            case logic_type::NOT:
                return "NOT"; break;
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
            {"NOT", logic_type::NOT}, {"FLIP-FLOP", logic_type::FLIPFLOP}
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
    std::map<std::string, logic_type> gate_map; // map<name, type(enum)>

    // every instance needs an associated type as well
    std::map<std::string, std::string> instance_map; // map<name, type>

    // need to track how each gate in a given module 
    // is connected. otherwise, the simulation will 
    // throw a segfault every time it runs. this map 
    // needs to be iterated through when a module is 
    // instantiated to connect all of the inner pieces
    std::map<std::string, std::string> signal_map; // map<src, dest(s)>

    // connections to the outside world. these are the 
    // only things that are available to outside modules
    std::map<std::string, std::string> input_map;  // map<name, destination(s)>
    std::map<std::string, std::string> output_map; // map<name, source>

public:
    // used by the xml generation utility to create 
    //modules in the order they were internalized
    static std::vector<std::string> global_ordered_module_list;

    // primarily used by signal evaluator
    auto get_internal_map(std::string which_map) -> std::map<std::string, std::string>& {
        if(which_map == "instance")
            return this->instance_map;
        else if(which_map == "signal")
            return this->signal_map;

        throw std::runtime_error("INTERNAL ERROR -> map type requested does not exist");
    }

    // write this top-level entity in compatible jsim XML format
    void generate_xml_file(std::ostream& os, bool has_root = false) {
        if(!has_root)
            os << "<root>\n";
        os << "    <module name=\"" << this->module_name << "\">\n";

        for(auto& gate : this->gate_map) {
            os << "        <gate name=\"" << gate.first << "\" type=\"" 
                << logic_type_name_proper(gate.second) << "\"/>\n";
        }

        for(auto& sig : this->signal_map) {
            os << "        <signal from=\"" << sig.first << "\" to=\"" << sig.second << "\"/>\n";
        }

        // at this point, we no longer need instance tags for 
        // anything as everything is fully evaluated during the compilation pass
        for(auto& out : this->output_map) {
            os << "        <output name=\"" << out.first << "\" from=\"" << out.second << "\"/>\n";
        }

        for(auto& in : this->input_map) {
            os << "        <input name=\"" << in.first << "\" to=\"" << in.second << "\"/>\n";
        }

        os << "    </module>\n";
        if(!has_root)
            os << "</root>\n";
    }

    // generates an internal representation of a given module
    SimulationModule(XmlNode n, std::map<std::string, SimulationModule*>& module_map) {
        
        // the XmlNode passed as argument should already be a ready-to-go module
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

            // for debugging purposes
            //std::cout << n.name() << std::endl << std::flush;

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
                    std::stringstream ss;
                    n.format_output(ss);

                    throw std::runtime_error(
                        "In module '" + this->module_name + "' unable to find entity '"
                        + _type + "' in module map\n" + ss.str());
                }

                // we know where the external module is, we need to go find all the infomation in it
                auto& gm = _mod->get_gate_map();

                auto it = gm.begin();
                while(it != gm.end()) {
                    this->gate_map.insert({_name + "." + it->first, it->second});
                    it++;
                }

                // at this point, _mod still holds the currently referenced module

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
                //std::cout << "output found in file\n";

                n.hasAttrs({"from", "name"}, true);
                n.hasOnlyAttrs({"from", "name"}, true);

                auto _from = n.attr("from").value();
                auto _name = n.attr("name").value();

                // try to find output source in gate map before trying the other stuff

                auto gate_iter = this->gate_map.find(_from);
                if(gate_iter != this->gate_map.end())
                    goto label_output_is_gate; // _from doesnt need to be modified

                {
                    auto src = this->split_period_string(_from);
                    switch(src.size()) {
                        case 1:
                            // lowest level type
                            break;
                        case 2:
                            {
                                // search through lower level instance element
                                //std::cout << "searching through lower level elements\n";
                                auto instance_type = this->instance_map.find(src.at(0));
                                if(instance_type != this->instance_map.end()) {
                                    auto module_ref = module_map.find(instance_type->second);

                                    if(module_ref != module_map.end()) {
                                        // we have the module representation 
                                        // now. grab the output we need
                                        try {
                                            auto str = module_ref->second->get_output(src.at(1));
                                            str = src.at(0) + "." + str;
                                            _from = str; // _from should now be fully evaluated
                                        } catch(std::out_of_range& ex) {
                                            std::stringstream ss;
                                            n.format_output(ss);

                                            throw std::runtime_error("Error in module '" + this->module_name 
                                                + "', node 'output': unable to find output '"
                                                + src.at(1) + "' in external module '" + instance_type->second
                                                + "'\nOffending node:\n    " + ss.str());
                                        }
                                    }
                                    else {
                                        // this shouldnt ever happen and should be caught 
                                        // by a different part of the program
                                        throw std::runtime_error("Unable to find module of type '" +
                                        instance_type->second + "' in global module map");
                                    }
                                }
                                else {
                                    std::stringstream ss;
                                    n.format_output(ss);

                                    // using a port that doesnt exist
                                    throw std::runtime_error("In module '" + this->module_name
                                    + "' unable to find instance type of '" + src.at(0)
                                    + "\n" + ss.str());
                                }
                            }
                            break;
                        default:
                            {
                                std::stringstream ss;
                                n.format_output(ss);

                                throw std::runtime_error(
                                    "In module '" + this->module_name + 
                                    "' error found in 'output' node: 'from' attribute"
                                    " has invalid number of period-seperated components\n"
                                    + ss.str());
                            }
                    }
                }

        label_output_is_gate:

                this->output_map.insert({_name, _from});
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

                // test if the source is an existing gate instance
                // if it is, skip to the dest evaluation
                {
                    auto gate_iter = this->gate_map.find(_from);
                    if(gate_iter != this->gate_map.end())
                        // I KNOW I KNOW I KNOW
                        goto signal_source_gate_finished; // _from doesnt need to be modified
                }

                // ==================================================
                // fully qualify the source gate

                try {
                    auto src = this->split_period_string(_from);
                    switch(src.size()) {
                        case 1:
                            // already the lowest level element
                            break;
                        case 2:
                            {
                                // search through lower level instance element
                                //std::cout << "searching through lower level elements\n";
                                auto instance_type = this->instance_map.find(src.at(0));
                                if(instance_type != this->instance_map.end()) {
                                    auto module_ref = module_map.find(instance_type->second);

                                    if(module_ref != module_map.end()) {
                                        // we have the module representation 
                                        // now. grab the output we need
                                        try {
                                            auto str = module_ref->second->get_output(src.at(1));
                                            str = src.at(0) + "." + str;
                                            _from = str; // _from should now be fully evaluated
                                        } catch(std::out_of_range& ex) {
                                            std::stringstream ss;
                                            n.format_output(ss);

                                            throw std::runtime_error("Error in module '" + this->module_name 
                                                + "', node 'signal': unable to find output '"
                                                + src.at(1) + "' in external module '" + instance_type->second
                                                + "'\nOffending node:\n    " + ss.str());
                                        }
                                    }
                                    else {
                                        // this shouldnt ever happen and should be caught 
                                        // by a different part of the program
                                        throw std::runtime_error("Unable to find module of type '" +
                                        instance_type->second + "' in global module map");
                                    }
                                }
                                else {
                                    std::stringstream ss;
                                    n.format_output(ss);

                                    // using a port that doesnt exist
                                    throw std::runtime_error("In module '" + this->module_name
                                    + "' unable to find instance type of '" + src.at(0)
                                    + "\n" + ss.str());
                                }
                            }
                            break;
                        default:
                            {
                                std::stringstream ss;
                                n.format_output(ss);

                                throw std::runtime_error(
                                    "In module '" + this->module_name + 
                                    "' error found in 'signal' node: 'from' attribute"
                                    " has invalid number of period-seperated components\n"
                                    + ss.str());
                            }
                    }

                }
                catch(std::runtime_error& sre) {
                    // if the above fails, use the entire 
                    // source string to search for the target in the gate map
                }

        // dont want to use a goto but will if i need to
        signal_source_gate_finished:

                std::vector<std::string> to_dest_str;

                // ==================================================
                // fully qualify all of the dest gate ports
                auto src = this->split_colon_string(_to);
                for(auto src_str : src) {
                    // now deal with each element individually
                    auto str = this->split_period_string(src_str);
                    
                    // what type this element is
                    auto instance_type = this->instance_map.find(str.at(0));
                    if(instance_type != this->instance_map.end()) {
                        // a composite type. need details
                        auto module_ref = module_map.find(instance_type->second);
                        if(module_ref != module_map.end()) {
                            // found the correct module
                            try {

                                auto input_vec = module_ref->second->get_input_vector(str.at(1));

                                for(auto& s : input_vec) {
                                    to_dest_str.push_back(str.at(0) + "." + s);
                                }

                            } catch(std::out_of_range& ex) {
                                std::stringstream ss;
                                n.format_output(ss);

                                throw std::runtime_error("error in module '"
                                + this->module_name + "' signal node: input '" 
                                + str.at(1) + "' of external module '" + instance_type->second +
                                "' doesn't exist\nOffending node:\n    " + ss.str());
                            }
                        }
                        else {
                            // somthing went wrong. DO SOMETHING!
                            std::stringstream ss;
                            n.format_output(ss);

                            throw std::runtime_error("error in module '"
                                + this->module_name + "': unable to find "
                                "nested entity of type '" + instance_type->second
                                + "' in global module map");
                        }

                    }
                    else {
                        // simple gate type
                        to_dest_str.push_back(src_str);
                    }

                }

                this->signal_map.insert({_from, combine_inputs(to_dest_str)});

            }

            n = n.next();
        }

        // fully qualify all signals inside each sub-module. this also 
        // makes them available to any future parent modules
        for(auto& mod : this->instance_map) {
            auto instance_name = mod.first;
            auto instance_type = mod.second;

            std::map<std::string, std::string>* signal_map_ptr = NULL;

            try {
                // get the module from global module map
                auto* mod_ptr = module_map.at(instance_type);
                signal_map_ptr = &mod_ptr->get_internal_map("signal");
            } 
            catch(std::out_of_range& ex) {
                throw std::runtime_error("SimulationModule -> when trying to evaluate "
                "submodule signals, local module name '" + instance_type + "' not found in global module table");
            }

            // now we have the signal map. prepend the instance name to each entry (k,v)
            for(auto& signal_kv : *signal_map_ptr) {
                auto signal_name = instance_name + "." + signal_kv.first;

                // set of signal destinations
                auto signal_value_set = this->split_colon_string(signal_kv.second);
                for(auto& str : signal_value_set)
                    str = instance_name + "." + str;

                // submodule signal names and destinations are now fully 
                // qualified. place them in the parent module signal map
                this->signal_map.insert({signal_name, combine_inputs(signal_value_set)});
            }

        }

        // last step is to make this module available globally
        if(module_map.find(this->module_name) != module_map.end()) {
            throw std::runtime_error(
                "SimulationModule -> module with name '" + 
                module_name + "' already exists");
        }
        else {
            module_map.insert({this->module_name, this});
            this->global_ordered_module_list.push_back(this->module_name);
        }
    }

    auto get_gate_map(void) -> std::map<std::string, logic_type>& {
        return this->gate_map;
    }

    auto get_input_vector(std::string input) -> std::vector<std::string> {
        auto str = this->input_map.at(input);
        return this->split_colon_string(str);
    }

    auto get_output(std::string input) -> std::string {
        auto str = this->output_map.at(input);
        return str;
    }

    friend std::ostream& operator<<(std::ostream& os, SimulationModule& sm) {
        os << "module name: " << sm.module_name << std::endl;
        os << "\n\tgates: " << sm.gate_map.size() << "\n";

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

        os << "\n\toutputs:\n";

        {
            auto it = sm.output_map.begin();
            while(it != sm.output_map.end()) {
                os << "\t\t" << it->first << " <- " << it->second << std::endl;
                it++;
            }
        }

        os << "\n\tsignals: " << sm.signal_map.size() << "\n";

        {
            auto it = sm.signal_map.begin();
            while(it != sm.signal_map.end()) {
                os << "\t\t" << it->first << " ->\n";

                auto str_vec = sm.split_colon_string(it->second);
                for(auto& str : str_vec)
                    os << "\t\t\t" << str << std::endl;
                it++;
            }
        }

        os << "\n\n";

        return os;
    }

};

std::vector<std::string> SimulationModule::global_ordered_module_list;
