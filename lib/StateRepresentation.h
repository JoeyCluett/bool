#pragma once

// this class takes an XML file as input and supplies 
// a more compact intermediate representation

#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <iostream>

// useful tool for iterating through XML documents
#include <XmlDocument.h>
#include <XmlNode.h>

enum class machine_type : int {
    mealy, moore, none
};

class StateRepresentation {
private:
    machine_type type = machine_type::none;
    std::string description = "";

public:
    // combine the above info and return as a string
    std::string info(void) {
        std::string str = "Description: " + this->description;
        str += "\nType: ";

        switch(this->type) {
            case machine_type::mealy: return str + "MEALY";
            case machine_type::moore: return str + "MOORE";
            default: return str + "NONE";
        }
    }

private:
    // used internally to represent a single
    struct single_state_t {
        int current_state = 0;
        int next_if_zero  = 0;
        int next_if_one   = 0;

        // save a few bytes by using a union here. it wont really affect anything
        union {
            int z = 0; // moore output
            int z_if_zero; // mealy output 0
        };

        int z_if_one = 0;  // mealy output 1

        machine_type type = machine_type::none;

        friend std::ostream& operator<<(std::ostream& os, single_state_t& sst) {
            os << "State: " << sst.current_state << ", Next state: (input = 0 : " << sst.next_if_zero << ", input = 1 : " << sst.next_if_one << ")";

            if(sst.type == machine_type::mealy)
                os << ", Output: (input = 0: " << sst.z_if_zero << ", input = 1: " << sst.z_if_one << ")\n";
            else if(sst.type == machine_type::moore)
                os << ", Output: " << sst.z << std::endl;
            else
                throw std::runtime_error("single_state_t operator<< : unknown state");

            return os;
        }
    };

    bool states_equivalent(single_state_t& s1, single_state_t& s2) {
        // regardless of machine type, these things must always 
        // be equal for the states to be equivalent
        if(!(s1.next_if_zero == s2.next_if_zero && s1.next_if_one == s2.next_if_one))
            return false;

        switch(this->type) {
            case machine_type::mealy:
                return ((s1.z_if_one == s2.z_if_one) && (s1.z_if_zero == s2.z_if_zero));
            case machine_type::moore:
                return (s1.z == s2.z);
            default:
                throw std::runtime_error("StateRepresentation states_equal : unknown machine type");
        }
    }

    std::vector<single_state_t> state_list;
    std::vector<std::pair<int, int>> global_alias_list;

    // verify that this node has all required attributes
    void verify_node(XmlNode node) {
        if(node.attr("id").empty())
            throw std::runtime_error("nodes must contain an 'id' attribute");
        if(node.attr("transition").empty())
            throw std::runtime_error("nodes must contain a 'transition' attribute");
        if(node.attr("output").empty())
            throw std::runtime_error("nodes must contain an 'output' attribute");
    }

    void verify_finished_node_list(void) {
        // every state entry should only occur once: report repeated states
        // every state needs to be positive: report negative states
        auto l_sz = this->state_list.size();

        for(int i = 0; i < l_sz; i++) {

            if(this->state_list.at(i).current_state < 0)
                throw std::runtime_error("State machine failed verification: "
                    "state entries must have positive current state");

            for(int j = i+1; j < l_sz; j++) {

                single_state_t& si = this->state_list[i];
                single_state_t& sj = this->state_list[j];

                if(si.current_state == sj.current_state)
                    throw std::runtime_error("State machine failed verification: "
                    "current state entries must not be repeated");
            }
        }

        return;
    }

    auto split_next_state(std::string str) -> std::pair<int, int> {
        std::string first;
        std::string second;

        const int STATE_first_string  = 0;
        const int STATE_second_string = 1;
        int STATE_current = STATE_first_string;

        for(char c : str) {
            switch(STATE_current) {
                case STATE_first_string:
                    if(c == ':')
                        STATE_current = STATE_second_string;
                    else
                        first.push_back(c);
                    break;
                case STATE_second_string:
                    second.push_back(c);
                    break;
                default:
                    throw std::runtime_error("error splitting transition attribute");
            }
        }

        return {
            std::stoi(first),
            std::stoi(second)
        };
    }

    auto split_output(std::string str) -> std::pair<int, int> {
        return split_next_state(str);
    }

    int index_of_state(int state) {
        for(int i = 0; i < this->state_list.size(); i++) {
            if(this->state_list[i].current_state == state)
                return i;
        }

        throw std::runtime_error("StateRepresentation index_of_state : state not in state_list");
    }

public:
    StateRepresentation(std::string input_file_name);

    friend std::ostream& operator<<(std::ostream& os, StateRepresentation& sr) {
        /*os << "Description: " << sr.description << std::endl;
        os << "Type: ";
        switch(sr.type) {
            case machine_type::mealy:
                os << "mealy\n\n"; break;
            case machine_type::moore:
                os << "moore\n\n"; break;
            case machine_type::none:
                os << "NONE\n\n"; break; // this should never happen
        }*/

        auto str5 = [](int i) -> std::string {
            std::string s = std::to_string(i);
            while(s.size() < 5)
                s += ' ';
            return s;
        };

        switch(sr.type) {
            case machine_type::moore:
                os << "Current state |  Next state   | Output\n";
                os << "              |  x=0    x=1   |    z  \n";
                os << "--------------+---------------+--------\n";
                break;
            case machine_type::mealy:
                os << "Current state |  Next state   | Output\n";
                os << "              |  x=0    x=1   | x=0    x=1\n";
                os << "--------------+---------------+------------\n";
                break;
            default:
                os << "NO STATES TO PRINT\n";
        }

        for(auto& st : sr.state_list) {
            if(sr.type == machine_type::moore) {
                os << "      " << str5(st.current_state) << "   |   " << str5(st.next_if_zero) << "  "
                << str5(st.next_if_one) << "|    " << str5(st.z) << std::endl;
            }
            else if(sr.type == machine_type::mealy) {
                os << "      " << str5(st.current_state) << "   |   " << str5(st.next_if_zero) << "  "
                << str5(st.next_if_one) << "|  " << str5(st.z_if_zero) << "  "
                << str5(st.z_if_one) << std::endl;
            }
        }

        os << std::endl;

        if(sr.global_alias_list.size() > 0) {
            os << "Global alias list:\n";
            for(auto pr : sr.global_alias_list)
                os << "  " << pr.first << " == " << pr.second << std::endl;
            os << std::endl;
        }

        return os;
    }

    void rowReduce(void);

};

void StateRepresentation::rowReduce(void) {
    std::vector<std::pair<int, int>> alias_list;
    std::vector<int> erased_states;

    do {
        // will be populated if there are still aliased rows
        alias_list.clear();

        //if(this->type == machine_type::moore) {
            auto& s_list = this->state_list;

            // iterate through the states
            for(int i = 0; i < this->state_list.size(); i++) {
                // -1 is used as a sentinal value while iterating
                std::pair<int, int> current_pair = {this->state_list[i].current_state, -1};

                // inner search loop
                for(int j = i; j < this->state_list.size(); j++) {
                    auto& s1 = s_list[i];
                    auto& s2 = s_list[j];

                    //if(s1.next_if_zero == s2.next_if_zero &&
                    //        s1.next_if_one == s2.next_if_one &&
                    //        s1.z == s2.z) {

                    if(this->states_equivalent(s1, s2)) {

                        // basically, find every similar state pair 
                        // and save them for later
                        current_pair.second = s2.current_state;

                        // obviously, every state will alias with 
                        // itself. ignore those cases
                        if(current_pair.first != current_pair.second) {
                            alias_list.push_back(current_pair);
                        }
                    }
                }
            }

            // now we can prune the state list
            for(auto& pr : alias_list) {

                //std::cout << pr.first << " : " << pr.second << std::endl;

                auto check_erased = [&](int st) -> bool {
                    for(int i : erased_states)
                        if(i == st)
                            return true;
                    return false;
                };

                if(check_erased(pr.second))
                    continue; // lets avoid a disaster here, gents

                int s1_index = this->index_of_state(pr.first);
                int s2_index = this->index_of_state(pr.second);

                // the .second state is always pruned
                this->state_list.erase(this->state_list.begin() + s2_index);
                erased_states.push_back(pr.second);

                // now replace all occurances of that state with the alias
                for(auto& st : this->state_list) {
                    if(st.next_if_zero == pr.second)
                        st.next_if_zero = pr.first;

                    if(st.next_if_one == pr.second)
                        st.next_if_one = pr.first;
                }
            }
        //}
        //else {
        //    throw std::runtime_error("StateRepresentation::rowReduce -> only moore machines currently supported");
        //}

        for(auto pr : alias_list)
            this->global_alias_list.push_back(pr);

    } while(alias_list.size() > 0);
}

StateRepresentation::StateRepresentation(std::string input) {
    XmlDocument doc(input);
    auto root = doc.root();

    if(root.name() != "diagram")
        throw std::runtime_error("StateRepresentation -> root node must be named 'diagram'");

    if(!root.next().empty())
        throw std::runtime_error("StateRepresentation -> only one diagram per XML file");

    auto root_type = root.attr("type");
    if(root_type.empty())
        throw std::runtime_error("StateRepresentation -> root node does not contain a 'type' attribute");

    if(root_type.value() == "moore")
        this->type = machine_type::moore;
    else if(root_type.value() == "mealy")
        this->type = machine_type::mealy;
    else
        throw std::runtime_error("StateRepresentation -> machine must be either 'moore' or 'mealy'");

    // description is not required but is recommended
    root_type = root.attr("desc");
    if(!root_type.empty())
        this->description = root_type.value();

    auto current_node = root.child();

    // now we have the root node taken care of (optional description)
    if(this->type == machine_type::moore) {
        //std::cout << "Machine type: Moore\n" << std::flush;

        do {

            //std::cout << "Node: "; current_node.format_output(std::cout) << std::endl << std::flush;
            //std::cout << "  id : " << current_node.attr("id").value() << std::endl << std::flush;

            int id     = std::stoi(current_node.attr("id").value());
            auto trans = this->split_next_state(current_node.attr("transition").value());
            int out    = std::stoi(current_node.attr("output").value());

            single_state_t s;
            s.current_state = id;
            s.next_if_zero = trans.first;
            s.next_if_one  = trans.second;
            s.z            = out;
            s.type         = machine_type::moore;

            this->state_list.push_back(s);

            // iterate through all of the nodes
            current_node = current_node.next();
        } while(!current_node.empty());

    }
    else { // machine type is mealy
        //std::cout << "Machine type: Mealy\n" << std::flush;

        do {

            //std::cout << "Node: "; current_node.format_output(std::cout) << std::endl << std::flush;
            //std::cout << "  id : #" << current_node.attr("id").value() << "#\n" << std::flush;

            int id     = std::stoi(current_node.attr("id").value());
            auto trans = this->split_next_state(current_node.attr("transition").value());
            auto out   = this->split_output(current_node.attr("output").value());

            single_state_t s;

            s.current_state = id;
            s.next_if_zero  = trans.first;
            s.next_if_one   = trans.second;
            s.z_if_zero     = out.first;
            s.z_if_one      = out.second;
            s.type          = machine_type::mealy;

            this->state_list.push_back(s);

            // iterate through all of the nodes
            current_node = current_node.next();
        } while(!current_node.empty());

    }

    this->verify_finished_node_list();
}
