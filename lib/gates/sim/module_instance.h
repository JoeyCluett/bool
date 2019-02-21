#pragma once

/*
    This file contains everything needed to create a fully 
    functioning simulation based on the internal representation
*/

#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include "SimulationModule.h"
#include "binary_format.h"
#include "../standard_gates.h"
#include "../latches.h"

class ModuleInstance {
private:
    gate_type_info_t gate_type_info;

    OR_t*   or_gate_arr   = NULL;
    NOR_t*  nor_gate_arr  = NULL;
    AND_t*  and_gate_arr  = NULL;
    NAND_t* nand_gate_arr = NULL;
    XOR_t*  xor_gate_arr  = NULL;
    XNOR_t* xnor_gate_arr = NULL;
    NOT_t*  not_gate_arr  = NULL;
    D_FlipFlop* ff_arr    = NULL;

    int num_signals;
    std::string module_name;

    std::string read_null_string(std::ifstream& ifile) {
        std::string ret_str;

        while(ifile) {
            char c[1];
            ifile.read(c, 1);

            if(c[0] == '\0')
                return ret_str;
            else
                ret_str.push_back(c[0]);
        }
    }

    std::vector<signal_info_t> signal_vec;

public:
    ModuleInstance(std::string filename) {
        std::ifstream ifile(filename, std::ios::binary);

        this->module_name = this->read_null_string(ifile);
        ifile.read(gate_type_info.c_buf, 32);

/*
        or_gate_arr   = new OR_t[gate_type_info.or_gates];
        nor_gate_arr  = new NOR_t[gate_type_info.nor_gates];
        and_gate_arr  = new AND_t[gate_type_info.and_gates];
        nand_gate_arr = new NAND_t[gate_type_info.nand_gates];
        xor_gate_arr  = new XOR_t[gate_type_info.xor_gates];
        xnor_gate_arr = new XNOR_t[gate_type_info.xnor_gates];
        not_gate_arr  = new NOT_t[gate_type_info.not_gates];
        ff_arr        = new D_FlipFlop[gate_type_info.flipflops];
*/   

        ifile.read((char*)&this->num_signals, 4);

        for(int i = 0; i < this->num_signals; i++) {
            signal_info_t sinfo;
            ifile.read(sinfo.c_buf, 11);

            //std::cout << sinfo;
            this->signal_vec.push_back(sinfo);
        }

        ifile.close();
    }

    void print(void) {
        std::cout << "Module name: " << this->module_name << std::endl;

        std::cout << "\nNumber of gates:\n";
        std::cout << "  OR   : " << this->gate_type_info.or_gates << std::endl;
        std::cout << "  NOR  : " << this->gate_type_info.nor_gates << std::endl;
        std::cout << "  AND  : " << this->gate_type_info.and_gates << std::endl;
        std::cout << "  NAND : " << this->gate_type_info.nand_gates << std::endl;
        std::cout << "  XOR  : " << this->gate_type_info.xor_gates << std::endl;
        std::cout << "  XNOR : " << this->gate_type_info.xnor_gates << std::endl;
        std::cout << "  NOT  : " << this->gate_type_info.not_gates << std::endl;
        std::cout << "  DFF  : " << this->gate_type_info.flipflops << std::endl;

        std::cout << "\nNumber of signals: " << this->num_signals << std::endl;

        for(auto& sinfo : this->signal_vec) {
            std::cout << "  " << sinfo;
        }
    }

};

