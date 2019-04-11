#pragma once

#include <XmlDocument.h>
#include <XmlNode.h>
#include "simulation_module.h"
#include "simulation_util.h"

static void simulation_module_signal_from(
        XmlNode n, simulation_module_t* sim, 
        simulation_module_map_t& sim_map);

static void simulation_module_signal_to(
        XmlNode n, simulation_module_t* sim, 
        simulation_module_map_t& sim_map);

void simulation_module_signal( 
        XmlNode n, simulation_module_t* sim, 
        simulation_module_map_t& sim_map) {

    #ifdef DEBUG_MACRO
    std::cout << "    module signal\n" << std::flush;
    #endif // DEBUG_MACRO

    try {
        n.verify_attrs({ "to", "from" }, true);
    } catch(std::exception& ex) {
        throw std::runtime_error("In module '" + sim->module_name + 
        "' error generating signal data: " + ex.what());
    }

    ::simulation_module_signal_from(n, sim, sim_map);
    ::simulation_module_signal_to(n, sim, sim_map);
}

void simulation_module_signal_from(
        XmlNode n, simulation_module_t* sim, 
        simulation_module_map_t& sim_map) {

    auto _from = n.attr("from").value();

    

}

void simulation_module_signal_to(
        XmlNode n, simulation_module_t* sim, 
        simulation_module_map_t& sim_map) {

    auto _to = n.attr("to").value();

}

