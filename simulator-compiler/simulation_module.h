#pragma once

#include <map>
#include <string>
#include <iostream>

// comment to disable debug logging
#define DEBUG_MACRO

enum class logic_type_t : int { NONE, OR, NOR, AND, NAND, XOR, XNOR, NOT, FLIPFLOP };

const std::map<std::string, logic_type_t> gate_type_conversion_lut = {
    {"OR", logic_type_t::OR},   {"NOR", logic_type_t::NOR}, 
    {"AND", logic_type_t::AND}, {"NAND", logic_type_t::NAND},
    {"XOR", logic_type_t::XOR}, {"XNOR", logic_type_t::XNOR},
    {"NOT", logic_type_t::NOT}, {"FLIP-FLOP", logic_type_t::FLIPFLOP},
    {"FLIPFLOP", logic_type_t::FLIPFLOP}
};

auto get_logic_type_string(logic_type_t lt) -> std::string {
    switch(lt) {
        case logic_type_t::NONE:     return "NONE";
        case logic_type_t::OR:       return "OR";
        case logic_type_t::NOR:      return "NOR";
        case logic_type_t::AND:      return "AND";
        case logic_type_t::NAND:     return "NAND";
        case logic_type_t::XOR:      return "XOR";
        case logic_type_t::XNOR:     return "XNOR";
        case logic_type_t::NOT:      return "NOT";
        case logic_type_t::FLIPFLOP: return "FLIPFLOP";
        default: throw std::runtime_error("Unknown logic type");
    }
}

// forward declaration
struct simulation_module_t;
typedef std::map<std::string, simulation_module_t*> simulation_module_map_t;

// data declaration
struct simulation_module_t {

    // the name used to index this simulation module
    std::string module_name;

    // each logic gate in a given simulation is tracked 
    // individually, this simply assists other modules 
    // in finding fully-qualified names for each gate
    std::map<std::string, logic_type_t> gate_map; // map<name, type(enum)>

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

    // vector types are stored here. sizes must be 
    // compared everytime they are modified. the name 
    // and size of each inputvec are stored in .first, 
    //the content is stored in .second in a colon separated list
    std::map<std::string, std::string> inputvec_map;  // map<(name:size), destination(s)>
    std::map<std::string, std::string> outputvec_map; // map<(name:size), source(s)>

    // signalvec types are stored here. the size of the 
    // resulting config string is compared before being stored
    std::map<std::string, std::string> signalvec_map;
};

// this operation is used frequently. 
auto get_module_ptr_from_module_map(
        std::string name, simulation_module_t* _this, simulation_module_map_t& m) -> simulation_module_t* {

    auto iter = _this->instance_map.find(name);
    if(iter != _this->instance_map.end()) {
        // found it
        auto g_iter = m.find(iter->second); // global module map iterator
        if(g_iter != m.end()) {
            return g_iter->second; // module pointer
        }
        else {
            throw std::runtime_error("Unable to find module of type '" 
            + iter->second + "' in global module map");   
        }
    }
    else {
        throw std::runtime_error("Unable to find instance '" + name 
        + "' in module map");
    }
}

std::ostream& operator<<(std::ostream& os, simulation_module_t& sm) {
    os << "Module name: " << sm.module_name << std::endl;

    for(auto& g : sm.gate_map)
        os << "    " <<  g.first << " : " << ::get_logic_type_string(g.second) << std::endl;

    for(auto& sig: sm.signal_map)
        os << "    " << sig.first << "\n        " << sig.second << std::endl;

}

