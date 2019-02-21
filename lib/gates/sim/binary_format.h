#pragma once

#include <iostream>
#include <fstream>
#include <string.h>
#include "SimulationModule.h"

union gate_type_info_t {
    struct {
        int or_gates;
        int nor_gates;
        int and_gates;
        int nand_gates;
        int xor_gates;
        int xnor_gates;
        int not_gates;
        int flipflops;
    } __attribute__((packed));
    char c_buf[32];
};

union signal_info_t {
    struct {
        u_int8_t src_type;
        int      src_index;
        u_int8_t dest_type;
        int      dest_index;
        char     dest_port;
    } __attribute__((packed));
    char c_buf[11];
};

union input_dest_t {
    struct {
        uint8_t dest_type;
        int     dest_index;
        char    dest_port;
    } __attribute__((packed));
    char c_buf[6];
};

struct input_info_t {
    std::string input_name;
    std::vector<input_dest_t> dest_arr;
};

union output_source_t {

    struct {
        u_int8_t src_type;
        int      src_index;
    } __attribute__((packed));

    char c_buf[5];
};

struct output_info_t {
    std::string output_name;
    output_source_t output_source;
};

std::ostream& operator<<(std::ostream& os, output_source_t& ost) {

    os << "  <-  ";

    switch(ost.src_type) {
        case 0x00:
            os << "OR"; break;
        case 0x01:
            os << "NOR"; break;
        case 0x02:
            os << "AND"; break;
        case 0x03:
            os << "NAND"; break;
        case 0x04:
            os << "XOR"; break;
        case 0x05:
            os << "XNOR"; break;
        case 0x06:
            os << "NOT"; break;
        case 0x07:
            os << "FlipFlop"; break;
        default:
            throw std::runtime_error("error in output_source_t : unknown destination type");
    }

    os << '[' << ost.src_index << "]\n";

    return os;
}

std::ostream& operator<<(std::ostream& os, output_info_t& oit) {

    os << oit.output_name << "  <-  ";

    switch(oit.output_source.src_type) {
        case 0x00:
            os << "OR"; break;
        case 0x01:
            os << "NOR"; break;
        case 0x02:
            os << "AND"; break;
        case 0x03:
            os << "NAND"; break;
        case 0x04:
            os << "XOR"; break;
        case 0x05:
            os << "XNOR"; break;
        case 0x06:
            os << "NOT"; break;
        case 0x07:
            os << "FlipFlop"; break;
        default:
            throw std::runtime_error("error in output_info_t : unknown destination type");
    }

    os << '[' << oit.output_source.src_index << "]\n";

    return os;
}

std::ostream& operator<<(std::ostream& os, input_dest_t& idt) {

    switch(idt.dest_type) {
        case 0x00:
            os << "OR"; break;
        case 0x01:
            os << "NOR"; break;
        case 0x02:
            os << "AND"; break;
        case 0x03:
            os << "NAND"; break;
        case 0x04:
            os << "XOR"; break;
        case 0x05:
            os << "XNOR"; break;
        case 0x06:
            os << "NOT"; break;
        case 0x07:
            os << "FlipFlop"; break;
        default:
            throw std::runtime_error("error in input_type_t : unknown destination type");
    }

    os << '[' << idt.dest_index << "]";

    switch(idt.dest_port) {
        case 'A':
        case 'B':
            os << "." << idt.dest_port << std::endl; break;
        case 'C':
            os << ".Clk\n"; break;
        case 'D':
            os << ".Din\n"; break;
        default:
            throw std::runtime_error("error in signal_info_t : unknown destination port");
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, signal_info_t& sit) {
    switch(sit.src_type) {
        case 0x00:
            os << "OR"; break;
        case 0x01:
            os << "NOR"; break;
        case 0x02:
            os << "AND"; break;
        case 0x03:
            os << "NAND"; break;
        case 0x04:
            os << "XOR"; break;
        case 0x05:
            os << "XNOR"; break;
        case 0x06:
            os << "NOT"; break;
        case 0x07:
            os << "FlipFlop"; break;
        default:
            throw std::runtime_error("error in signal_info_t : unknown source type");
    }

    os << '[' << sit.src_index << "] ";

    switch(sit.dest_type) {
        case 0x00:
            os << " ->  OR"; break;
        case 0x01:
            os << " ->  NOR"; break;
        case 0x02:
            os << " ->  AND"; break;
        case 0x03:
            os << " ->  NAND"; break;
        case 0x04:
            os << " ->  XOR"; break;
        case 0x05:
            os << " ->  XNOR"; break;
        case 0x06:
            os << " ->  NOT"; break;
        case 0x07:
            os << " ->  FlipFlop"; break;
        default:
            throw std::runtime_error("error in signal_info_t : unknown destination type");
    }

    os << '[' << sit.dest_index << ']';

    switch(sit.dest_port) {
        case 'A':
        case 'B':
            os << "." << sit.dest_port << std::endl; break;
        case 'C':
            os << ".Clk\n"; break;
        case 'D':
            os << ".Din\n"; break;
        default:
            throw std::runtime_error("error in signal_info_t : unknown destination port");
    }

    //os << "." << sit.dest_port << std::endl;

    return os;
}

static void calculate_gate_indices(std::map<std::string, logic_type>& gate_map, std::map<std::string, int>& gate_index_map) {
    gate_type_info_t current_gate_indices;
    for(int i = 0; i < 32; i++)
        current_gate_indices.c_buf[i] = 0x00;

    for(auto& gate_kv : gate_map) {
        switch(gate_kv.second) {
            case logic_type::OR:
                gate_index_map.insert({gate_kv.first, current_gate_indices.or_gates++}); break;
            case logic_type::NOR:
                gate_index_map.insert({gate_kv.first, current_gate_indices.nor_gates++}); break;
            case logic_type::AND:
                gate_index_map.insert({gate_kv.first, current_gate_indices.and_gates++}); break;
            case logic_type::NAND:
                gate_index_map.insert({gate_kv.first, current_gate_indices.nand_gates++}); break;
            case logic_type::XOR:
                gate_index_map.insert({gate_kv.first, current_gate_indices.xor_gates++}); break;
            case logic_type::XNOR:
                gate_index_map.insert({gate_kv.first, current_gate_indices.xnor_gates++}); break;
            case logic_type::NOT:
                gate_index_map.insert({gate_kv.first, current_gate_indices.not_gates++}); break;
            case logic_type::FLIPFLOP:
                gate_index_map.insert({gate_kv.first, current_gate_indices.flipflops++}); break;
            default:
                throw std::runtime_error("get_logic_type_of error : unknown logic_type");
        }
    }    
}

static auto get_name_and_port(std::string str) -> std::pair<std::string, std::string> {
    auto str_vec = SimulationModule::split_period_string(str);
    
    auto port = str_vec.back();

    str_vec.pop_back();

    auto name_str = str_vec[0];
    for(int i = 1; i < str_vec.size(); i++)
        name_str += "." + str_vec[i];

    return {name_str, port};
}

void create_binary_module(SimulationModule* sm, std::string str) {
    std::ofstream ofile(str, std::ios::binary);

    ofile.write(sm->get_module_name().c_str(), sm->get_module_name().size());

    {
        char c[1] = {0x00};
        ofile.write(c, 1);
    }

    // clear this struct
    gate_type_info_t gate_nums;
    for(int i = 0; i < 32; i++)
        gate_nums.c_buf[i] = 0x00;

    // find the number of each gate type
    auto& gate_map = sm->get_gate_map();
    std::map<std::string, int> gate_index_map;

    ::calculate_gate_indices(gate_map, gate_index_map);

    for(auto& gate_kv : gate_map) {
        if(gate_kv.second == logic_type::OR)        gate_nums.or_gates++;
        else if(gate_kv.second == logic_type::NOR)  gate_nums.nor_gates++;
        else if(gate_kv.second == logic_type::AND)  gate_nums.and_gates++;
        else if(gate_kv.second == logic_type::NAND) gate_nums.nand_gates++;
        else if(gate_kv.second == logic_type::XOR)  gate_nums.xor_gates++;
        else if(gate_kv.second == logic_type::XNOR) gate_nums.xnor_gates++;
        else if(gate_kv.second == logic_type::NOT)  gate_nums.not_gates++;
        else if(gate_kv.second == logic_type::FLIPFLOP) gate_nums.flipflops++;
        else throw std::runtime_error("Error in create_binary_module : unknown logic gate type");
    }

    // now we have the number of each gate type
    ofile.write(gate_nums.c_buf, 32);

    auto get_logic_type_of = [](logic_type lt) -> uint8_t {
        switch(lt) {
            case logic_type::OR: return 0x00;
            case logic_type::NOR: return 0x01;
            case logic_type::AND: return 0x02;
            case logic_type::NAND: return 0x03;
            case logic_type::XOR: return 0x04;
            case logic_type::XNOR: return 0x05;
            case logic_type::NOT: return 0x06;
            case logic_type::FLIPFLOP: return 0x07;
            default:
                throw std::runtime_error("get_logic_type_of error : unknown logic_type");
        }
    };

    auto& signal_map = sm->get_internal_map("signal");
    std::vector<signal_info_t> signal_vector;

    // iterate through every signal in the module and add a 
    // suitable signal_info_t struct to the signal vector
    for(auto& sig_kv : signal_map) {
        signal_info_t sinfo;

        auto src_gate_iter = gate_map.find(sig_kv.first);
        if(src_gate_iter == gate_map.end()) {
            throw std::runtime_error(
                "create_binary_module error : unable to find gate with name '" 
                + sig_kv.first + "'\n");
        }

        // gate exists, yay!

        // this info is the same for every destination
        sinfo.src_type  = get_logic_type_of(src_gate_iter->second);
        sinfo.src_index = gate_index_map.at(src_gate_iter->first);

        // iterate through every destination port in the dest list
        auto str_vec = SimulationModule::split_colon_string(sig_kv.second);
        for(auto& str : str_vec) {
            auto name_and_port = ::get_name_and_port(str);
            
            // for now, assume everything is a logic gate
            sinfo.dest_type = get_logic_type_of(gate_map.at(name_and_port.first));
            sinfo.dest_index = gate_index_map.at(name_and_port.first);

            const std::map<std::string, char> port_name_map = {
                {"A", 'A'},
                {"B", 'B'},
                {"D", 'D'},
                {"Din", 'D'},
                {"Clk", 'C'}
            };

            sinfo.dest_port = port_name_map.at(name_and_port.second);

            // add this signal to the signal array
            signal_vector.push_back(sinfo);
        }

    }

    {
        size_t sz = signal_vector.size();
        ofile.write((char*)&sz, 4);
    }

    for(auto& sinfo : signal_vector)
        ofile.write(sinfo.c_buf, 11);

    std::vector<input_info_t> input_vec;

    auto& input_map = sm->get_internal_map("input");

    // save the number of inputs
    {
        size_t sz = input_map.size();
        ofile.write((char*)&sz, 4);
    }

    // for every input in the module
    for(auto& inp : input_map) {
        input_info_t i_info;
        auto s_vec = SimulationModule::split_colon_string(inp.second);

        i_info.input_name = inp.first;

        for(auto str : s_vec) {
            input_dest_t input_dest;
            auto name_and_port = ::get_name_and_port(str);

            const std::map<std::string, char> port_name_map = {
                {"A", 'A'},
                {"B", 'B'},
                {"D", 'D'},
                {"Din", 'D'},
                {"Clk", 'C'}
            };

            // for now, assume everything is a logic gate
            input_dest.dest_type  = get_logic_type_of(gate_map.at(name_and_port.first));
            input_dest.dest_index = gate_index_map.at(name_and_port.first);
            input_dest.dest_port  = port_name_map.at(name_and_port.second);

            i_info.dest_arr.push_back(input_dest);
        }

        input_vec.push_back(i_info);
    }

    // place all of the inputs in the binary file
    for(auto& inp : input_vec) {
        // input name comes first
        ofile.write(inp.input_name.c_str(), inp.input_name.size()+1);
    
        // number of destination ports this input targets
        size_t sz = inp.dest_arr.size();
        ofile.write((char*)&sz, 4);

        // write every destination
        for(auto& in_dest : inp.dest_arr)
            ofile.write(in_dest.c_buf, 6);
    }

    // pack all of the outputs
    std::vector<output_info_t> output_vec;
    auto& output_map = sm->get_internal_map("output");
    for(auto& outp : output_map) {
        output_info_t output_info;

        output_info.output_name = outp.first;
        output_info.output_source.src_type = get_logic_type_of(gate_map.at(outp.second));
        output_info.output_source.src_index = gate_index_map.at(outp.second);

        output_vec.push_back(output_info);
    }

    // number of outputs we have
    {
        size_t sz = output_vec.size();
        ofile.write((char*)&sz, 4);
    }

    // write output information to binary file
    for(auto& oinfo : output_vec) {
        ofile.write(oinfo.output_name.c_str(), oinfo.output_name.size()+1);
        ofile.write(oinfo.output_source.c_buf, 5);
    }

    ofile.close();
}
