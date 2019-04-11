#pragma once

#include <XmlDocument.h>
#include <XmlNode.h>
#include "simulation_module.h"

void simulation_module_instance(
        XmlNode n, 
        simulation_module_t* sim, 
        simulation_module_map_t& sim_map) {

    #ifdef DEBUG_MACRO
    std::cout << "    module instance\n" << std::flush;
    #endif // DEBUG_MACRO

    try {
        n.verify_attrs({ "type", "name" }, true);
    } catch(std::exception& ex) {
        throw std::runtime_error("In module '" + sim->module_name + 
        "' error generating instance data: '" + ex.what() + "'");
    }

    auto _name = n.attr("name").value();
    auto _type = n.attr("type").value();
    auto _type_iter = sim_map.find(_type);

    if(_type_iter == sim_map.end()) {
        throw std::runtime_error("In module '" + 
        sim->module_name + "' instance of type '" + 
        n.attr("type").value() + " does not exist in global module map");
    }

    // we have a reference to the module needed
    {
        auto _instance_iter = sim->instance_map.find(_name);
        if(_instance_iter != sim->instance_map.end())
            throw std::runtime_error("In module '" + sim->module_name + 
            "' instance with name '" + _name + "' already exists");
    }
    
    // not sure why this info is saved but ya' neva' know
    sim->instance_map.insert({ _name, _type_iter->first });  
    
    // need to find all of the gates associated with this instance
    for(auto& g : _type_iter->second->gate_map)
        sim->gate_map.insert({ _name + "." + g.first, g.second });
}


