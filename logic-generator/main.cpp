#include <iostream>
#include <fstream>
#include <map>
#include <unordered_set>
#include <vector>

#include "main.h"

// for debugging purposes
//#define DEBUG

using namespace std;

struct LogicConfiguration {
    string name;

    // map<gate_output, gate_input>
    map<string, string> signals;

    // map<gate_name, gate_type>
    map<string, const int> gates;

    // set<input_name, vector<targets>>
    map<string, vector<string>> inputs;

    // set<output_name, gate_source>
    map<string, string> outputs;
};

ostream& operator<<(ostream& os, LogicConfiguration& lc);

// reverse polish notation
void evaluate_logical_equation_rpn(vector<string> tl, string target, LogicConfiguration& lc);

// infix notation
void evaluate_logical_equation_infix(vector<string> tl, string target, LogicConfiguration& lc);

// generate final xml file
void generate_xml_output(LogicConfiguration& lc, string filename);

vector<string> token_list;

int main(int argc, char* argv[]) {

    if(argc != 3) {
        cout << "Usage: \n    " << argv[0] << " <input file> <output file>\n\n";
        return 1;
    }

    {
        ifstream is(argv[1]);
        string s;

        const int state_default = 0;
        const int state_comments = 1;
        int current_state = state_default;

        // remove all comments
        while(is >> s) {
            switch(current_state) {
                case state_default:
                    if(s == "#!")
                        current_state = state_comments;
                    else
                        token_list.push_back(s);
                    break;
                case state_comments:
                    if(s == "!#")
                        current_state = state_default;
                    break;
                default:
                    throw runtime_error("Unknown state in tokenizer");
            }
        }

        is.close();
    }

    LogicConfiguration lc;

    const int state_default     = 0;
    const int state_name        = 1;
    const int state_input_list  = 2;
    const int state_output_list = 3;
    const int state_logic       = 4;
    int current_state = state_default;

    const int input_state_type = 0;
    const int input_state_name = 1;
    int current_input_state = input_state_type;

    const int output_state_type = 0;
    const int output_state_name = 1;
    int current_output_state = output_state_type;

    const int logic_state_type   = 0;
    const int logic_state_dest   = 1;
    const int logic_state_assign = 2;
    const int logic_state_body   = 3;
    int current_logic_state = logic_state_type;

    vector<string> logic_equation;
    string logic_goes_to  = ""; // output that equation targets
    string logic_eqn_type = ""; // either "RPN" or "INFIX"

    for(auto& s : token_list) {
        switch(current_state) {
            case state_default:
                #ifdef DEBUG
                cout << "state default\n";
                #endif

                if(s == "NAME")
                    current_state = state_name;
                else if(s == "INPUTSTART")
                    current_state = state_input_list;
                else if(s == "OUTPUTSTART")
                    current_state = state_output_list;
                else if(s == "LOGICSTART")
                    current_state = state_logic;
                else 
                    throw runtime_error("Unknown token: " + s);
                break;
            case state_name:
                #ifdef DEBUG
                cout << "state name\n";
                #endif

                lc.name = s;
                current_state = state_default;
                break;
            case state_input_list:
                switch(current_input_state)
                {
                    case input_state_type:
                        #ifdef DEBUG
                        cout << "state input_list.type\n";
                        #endif

                        if(s == "INPUTEND")
                            current_state = state_default;
                        else if(s != "BIT")
                            throw runtime_error("Unsupported input type. Currently only 'BIT' is supported. Sorry!");
                        else
                            current_input_state = input_state_name;
                        break;
                    case input_state_name:
                        #ifdef DEBUG
                        cout << "state input_list.name\n";
                        #endif

                        lc.inputs.insert({s, {}});
                        current_input_state = input_state_type;
                        break;
                    default:
                        throw runtime_error("In state_input_list, unknown nested state");
                }
                break;
            case state_output_list:
                switch(current_output_state) 
                {
                    case output_state_type:
                        #ifdef DEBUG
                        cout << "state output_list.type\n";
                        #endif

                        if(s == "OUTPUTEND")
                            current_state = state_default;
                        else if(s != "BIT")
                            throw runtime_error("Unsupported output type. Currently only 'BIT' is supported");
                        else
                            current_output_state = output_state_name;
                        break;
                    case output_state_name:
                        #ifdef DEBUG
                        cout << "state outout_list.name\n";
                        #endif

                        lc.outputs.insert({s, ""});
                        current_output_state = output_state_type;
                        break;
                    default:
                        throw runtime_error("In state_output_list, unknown nested state");
                }
                break;
            case state_logic:
                switch(current_logic_state) {
                    case logic_state_type:
                        #ifdef DEBUG
                        cout << "state logic.type\n";
                        #endif

                        if(s == "LOGICEND") {
                            current_state = state_default;
                        }
                        else if(s != "RPN" && s != "INFIX")
                            throw runtime_error("Unexpected type found in logical expression. Expecting 'RPN' or 'INFIX'");
                        else {
                            logic_eqn_type = s;
                            current_logic_state = logic_state_dest;
                        }
                        break;
                    case logic_state_dest:
                        #ifdef DEBUG
                        cout << "state logic.dest\n";
                        #endif

                        {
                            auto iter = lc.outputs.find(s);
                            if(iter == lc.outputs.end())
                                throw runtime_error("In logical expression, target '" + s + "' not found in output list");
                            logic_goes_to = s;
                            current_logic_state = logic_state_assign;
                        }
                        break;
                    case logic_state_assign:
                        #ifdef DEBUG
                        cout << "state logic.assign\n";
                        #endif

                        if(s != "<=")
                            throw runtime_error("In logical expression, expecting '<='. Found '" + s + "'");
                        current_logic_state = logic_state_body;
                        break;
                    case logic_state_body:
                        #ifdef DEBUG
                        cout << "state logic.body\n";
                        #endif

                        if(s == "END") {
                            if(logic_eqn_type == "RPN")
                                ::evaluate_logical_equation_rpn(logic_equation, logic_goes_to, lc);
                            else if(logic_eqn_type == "INFIX")
                                ::evaluate_logical_equation_infix(logic_equation, logic_goes_to, lc);
                            
                            logic_equation.clear();
                            current_logic_state = logic_state_type;
                        }
                        else
                            logic_equation.push_back(s);
                        
                        break;
                    default:
                        throw runtime_error("In state_logic, unknown nested state");
                }
                break;
            default:
                throw runtime_error("Unknown state in logic generator");
        }
    }

    cout << lc << endl;

    generate_xml_output(lc, argv[2]);

    return 0;
}

void generate_xml_output(LogicConfiguration& lc, string filename) {
    ofstream ofs(filename);

    ofs << "<root>\n";
    ofs << "    <module name=\"" << lc.name << "\">\n";

    // output gate names and types
    for(auto& g : lc.gates) {
        ofs << "        <gate type=\"";

        switch(g.second) {
            case 0: ofs << "AND"; break;
            case 1: ofs << "XOR"; break;
            case 2: ofs << "OR"; break;
            default: throw runtime_error("When generating XML file, unknown gate type found");
        }

        ofs << "\" name=\"" << g.first << "\"/>\n";
    }

    // output signal names
    for(auto& s : lc.signals)
        ofs << "        <signal from=\"" << s.first << "\" to=\"" << s.second << "\"/>\n";

    // output input names
    for(auto& i : lc.inputs) {
        ofs << "        <input name=\"" << i.first << "\" to=\"";
        for(int j = 0; j < i.second.size(); j++) {
            if(j) ofs << ":";
            ofs << i.second[j];
        }
        ofs << "\"/>\n";
    }

    // output output names and sources
    for(auto& o : lc.outputs)
        ofs << "        <output name=\"" << o.first << "\" from=\"" << o.second << "\"/>\n";

    ofs << "    </module>\n";
    ofs << "</root>\n";

    ofs.close();
}

ostream& operator<<(ostream& os, LogicConfiguration& lc) {

    os << "\nConfig name: " << lc.name << endl;

    os << "  Gates:\n";
    for(auto& a : lc.gates) {
        os << "    " << a.first << " : ";
        switch(a.second) {
            case 0: os << "AND\n"; break;
            case 1: os << "XOR\n"; break;
            case 2: os << "OR\n"; break;
            default:
                throw runtime_error("Unknown logic type in configuration");
        }
    }

    os << "\n  Signals:\n";
    for(auto& a : lc.signals)
        os << "    " << a.first << " -> " << a.second << endl;

    os << "\n  Outputs:\n";
    for(auto& a : lc.outputs)
        os << "    " << a.first << " <- " << a.second << endl;

    os << "\n  Inputs:\n";
    for(auto& a : lc.inputs) {
        os << "    " << a.first << " -> ";
        for(auto& b : a.second)
            os << b << ' ';
        os << endl;
    }

    return os;
}

void evaluate_logical_equation_infix(vector<string> tl, string target, LogicConfiguration& lc) {

    #ifdef DEBUG
    cout << "    GENERATING RPN EQUATION FROM INFIX EXPRESSION...\n" << flush;
    #endif

    const int and_type = 0;
    const int xor_type = 1;
    const int or_type  = 2;
    const map<string, const int> logical_function_map = {
        { "and", and_type }, { "&", and_type }, { "AND", and_type }, { "*", and_type },
        { "or", or_type   }, { "|", or_type  }, { "OR", or_type   }, { "+", or_type },
        { "xor", xor_type }, { "^", xor_type }, { "XOR", xor_type },
    };

    vector<string> output_stack;
    vector<string> op_stack; // &, |, ^

    for(auto& s : tl) {

        auto iter = logical_function_map.find(s);
        if(iter == logical_function_map.end()) { // this is an operand or parentheses
            if(s == "(") {
                op_stack.push_back(s);
            }
            else if(s == ")") {
                while(op_stack.back() != "(") {
                    output_stack.push_back(op_stack.back());
                    op_stack.pop_back();

                    if(op_stack.size() == 0)
                        throw runtime_error("When generating RPN equation, "
                        "malformed expression encountered");

                }
                op_stack.pop_back();
            }
            else {
                // operands get pushed onto output right away
                output_stack.push_back(s);
            }
        }
        else { // this is an operator
            // because all operators have the 'same' precedence, we push 
            // them through as though every operator has lower precedence
            // we can also take a shortcut here because every time an 
            // operator is encountered, the previous operator (assuming 
            // it exists) will be placed on the output stack
            if(op_stack.size() == 0) {
                op_stack.push_back(s);
            }
            else if(logical_function_map.find(op_stack.back()) != logical_function_map.end()) {
                // operator is already on the op_stack. place it on output 
                // and replace with current operator
                output_stack.push_back(op_stack.back());
                op_stack.back() = s;
            }
            else {
                op_stack.push_back(s);
            }
        }

        #ifdef DEBUG
        cout << "    Operator stack: ";
        for(auto& s : op_stack)
            cout << s << ' ';
        cout << "\n    Output stack:   ";
        for(auto& s : output_stack)
            cout << s << ' ';
        cout << "\n\n";
        #endif

    }

    while(op_stack.size() > 0) {
        if(logical_function_map.find(op_stack.back()) 
                == logical_function_map.end())
            throw runtime_error("When generting RPN equation, malformed expression encountered");

        output_stack.push_back(op_stack.back());
        op_stack.pop_back();
    }

    #ifdef DEBUG
    cout << "Generated RPN equation...\n    ";
    for(auto& s : output_stack)
        cout << s << ' ';
    cout << endl;
    #endif 

    ::evaluate_logical_equation_rpn(output_stack, target, lc);
}

void evaluate_logical_equation_rpn(vector<string> tl, string target, LogicConfiguration& lc) {
    const int and_type = 0;
    const int xor_type = 1;
    const int or_type  = 2;
    const map<string, const int> logical_function_map = {
        { "and", and_type }, { "&", and_type }, { "AND", and_type }, { "*", and_type },
        { "or", or_type   }, { "|", or_type  }, { "OR", or_type   }, { "+", or_type },
        { "xor", xor_type }, { "^", xor_type }, { "XOR", xor_type },
    };

    static int current_gate_index = 0;
    vector<StackEntry> st;

    for(auto& str : tl) {
        auto iter = logical_function_map.find(str);
        if(iter == logical_function_map.end()) {
            auto g_iter = lc.inputs.find(str);
            if(g_iter == lc.inputs.end())
                throw std::runtime_error("In logical expression, unable "
                "to find token in logical function map or configuration input map");

            // we found the input. just push it onto the stack
            st.push_back(StackEntry({ StackEntryType::input_reference, str }));
        }
        else {
            
            if(st.size() < 2)
                throw runtime_error("When generating logic map, encountered malformed "
                "logical expression");

            // this token is a logical operator
            string gate_name = string("g-") + to_string(current_gate_index++);
            lc.gates.insert({ gate_name, iter->second }); // save the gate for later reference

            auto i0 = *(st.end()-1);
            auto i1 = *(st.end()-2);

            st.pop_back(); st.pop_back();

            // four possible ways this could go
            if(i0.set == StackEntryType::gate_reference && i1.set == StackEntryType::gate_reference) {
                // two gates connected to another gate
                // this requires 2 internal signals
                lc.signals.insert({ i0.str, gate_name + ".A" });
                lc.signals.insert({ i1.str, gate_name + ".B" });
            }
            else if(i0.set == StackEntryType::input_reference && i1.set == StackEntryType::gate_reference) {
                // one input and one gate connected to another gate
                // requires one internal signal and one input dest addition
                lc.inputs.at(i0.str).push_back(gate_name + ".A");
                lc.signals.insert({ i1.str, gate_name + ".B" });
            }
            else if(i0.set == StackEntryType::input_reference && i1.set == StackEntryType::input_reference) {
                lc.inputs.at(i0.str).push_back(gate_name + ".A");
                lc.inputs.at(i1.str).push_back(gate_name + ".B");
            }
            else { // i0.set == StackEntryType::gate_reference && i1.set == StackEntryType::input_reference
                lc.signals.insert({ i0.str, gate_name + ".A" });
                lc.inputs.at(i1.str).push_back(gate_name + ".B");
            }

            st.push_back({ StackEntryType::gate_reference, gate_name });
        }
    }

    // all signals and inputs are done. now we need to connect 
    // the correct gate to the output

    if(st.size() != 1)
        throw runtime_error("When assigning output, encountered malformed logical "
        "expression. Too many gates/too few operators");

    if(st[0].set != StackEntryType::gate_reference)
        throw runtime_error("When assigning output, malformed expression presented input_reference when gate_reference required");

    lc.outputs.at(target) = st[0].str;
}
