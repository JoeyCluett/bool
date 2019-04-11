#pragma once

#include <XmlDocument.h>
#include <XmlNode.h>
#include "simulation_module.h"

void simulation_module_gate(
        XmlNode n, 
        simulation_module_t* sim, 
        simulation_module_map_t& sim_map) {

    #ifdef DEBUG_MACRO
    std::cout << "    module gate\n" << std::flush;
    #endif // DEBUG_MACRO

    try {
        n.verify_attrs({ "type", "name" }, true);
    } catch(std::exception& ex) {
        throw std::runtime_error("In module '" + 
            sim->module_name + "' error generating gate data: " + ex.what());
    }

    auto _name = n.attr("name").value();
    auto _type_iter = gate_type_conversion_lut.find(n.attr("type").value());

    if(_type_iter == gate_type_conversion_lut.end()) {
        throw std::runtime_error("In module '" + sim->module_name + 
        "' when evaluating gate type: '" + n.attr("type").value() + 
        "' is not a valid gate type");
    }
    else {
        
        // make sure gate with name doesnt already exist
        auto _gate_iter = sim->gate_map.find(_name);
        if(_gate_iter != sim->gate_map.end())
            throw std::runtime_error("In module '" + sim->module_name + 
            "' gate with name '" + _name + "' already exists");
        else
            // place the gate in the gate map
            sim->gate_map.insert({ _name, _type_iter->second });
    }

}


