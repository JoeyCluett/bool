#pragma once

#include <XmlDocument.h>
#include <XmlNode.h>
#include "simulation_module.h"
#include "simulation_util.h"

void simulation_module_input(
        XmlNode n, simulation_module_t* sim, 
        simulation_module_map_t& sim_map) {

    #ifdef DEBUG_MACRO
    std::cout << "    module input\n" << std::flush;
    #endif // DEBUG_MACRO

    try {
        n.verify_attrs({ "name", "to" }, true);
    } catch(std::exception& ex) {
        throw std::runtime_error("In module '" + sim->module_name + 
        "' error generating input data: '" + ex.what() + "'");
    }

    auto _name = n.attr("name").value();
    auto config_mat = ::split_config_string(n.attr("to").value());

    config_matrix_t final_config_matrix;

    for(auto& v : config_mat) {
        if(v.size() != 1) {
            throw std::runtime_error("In module '" + 
            sim->module_name + "' error in 'input' tag: malformed "
            "expression in 'to' attribute");
        }

        auto& str = v.at(0);
        // < name, destination_port >
        auto p = ::split_port_dest(str);

        // input-to -> instance-input
        {
            auto instance = sim->instance_map.find(p.first);
            if(instance != sim->instance_map.end()) {
                // this is for an instance input
                try {
                    auto mod_ptr = ::get_module_ptr_from_module_map(p.first, sim, sim_map);
                    auto iter = mod_ptr->input_map.find(p.second);
                    if(iter != mod_ptr->input_map.end()) {
                        auto input_config = ::split_config_string(iter->second);
                        for(auto& v : input_config) {
                            final_config_matrix.push_back({ p.first + "." + v[0] });
                        }
                    }
                    else
                        throw std::runtime_error(std::string("after finding submodule '") + 
                        p.first + "' in use, unable to find input '" + p.second + "'");
                } catch(std::runtime_error& err) {
                    throw std::runtime_error("Error in module '" + sim->module_name + "' : " + err.what());
                }

                // no point in trying other locations: we already found it
                continue;
            }
        }

        // input-to -> gate-input
        {
            
        }

    }
}


