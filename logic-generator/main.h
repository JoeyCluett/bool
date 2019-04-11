#pragma once

#include <string>

enum class StackEntryType : int {
    none             = 0,
    gate_reference   = 1,
    input_reference  = 2
};

struct StackEntry {
    StackEntryType set;
    std::string str; // name of whatever reference is 
};

