#pragma once

// XML utility libs
#include <XmlDocument.h>
#include <XmlNode.h>

// everything needed to create fully defined simulation modules
#include "simulation_module.h"
#include "module_gate.h"
#include "module_instance.h"
#include "module_signal.h"

auto simulation_module_new(XmlNode n, simulation_module_map_t& sim_map) -> simulation_module_t* {
    auto* ptr = new simulation_module_t;

    {
        auto name_attr = n.attr("name");
        if(name_attr.empty())
            throw std::runtime_error("Module name not defined");
        else
            ptr->module_name = name_attr.value();
    }

    // start iterating through child tags
    n = n.child();
    while(!n.empty()) {

        if(n.name() == "gate")
            simulation_module_gate(n, ptr, sim_map);
        else if(n.name() == "instance")
            simulation_module_instance(n, ptr, sim_map);
        else if(n.name() == "signal")
            ;
        else if(n.name() == "input")
            ;
        else if(n.name() == "output")
            ;
        else
            throw std::runtime_error("Unknown tag in module '" + ptr->module_name + "'");

        n = n.next();
    }

    sim_map.insert({ ptr->module_name, ptr });
    return ptr;
}

