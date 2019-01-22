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
    };

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
        // will verify there are no repeated states in 
        // the list. not sure how im gonna implement this yet
        return;
    }

    auto split_next_state(std::string str) -> std::pair<int, int> {
        std::string first;
        std::string second;

        const int STATE_first_string = 0;
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

    int index_of_state(int state) {
        for(int i = 0; i < this->state_list.size(); i++) {
            if(this->state_list[i].current_state == state)
                return i;
        }

        throw std::runtime_error("StateRepresentation::index_of_state -> state not in state_list");
    }

public:
    StateRepresentation(std::string input_file_name);

    friend std::ostream& operator<<(std::ostream& os, StateRepresentation& sr) {
        os << "Description: " << sr.description << std::endl;
        os << "Type: ";
        switch(sr.type) {
            case machine_type::mealy:
                os << "mealy\n\n"; break;
            case machine_type::moore:
                os << "moore\n\n"; break;
            case machine_type::none:
                os << "NONE\n\n"; break; // this should never happen
        }

        for(auto& st : sr.state_list) {
            if(sr.type == machine_type::moore) {
                os << st.current_state << "  |  " << st.next_if_zero << "  "
                << st.next_if_one << "  |  " << st.z << std::endl;
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

        if(this->type == machine_type::moore) {
            auto& s_list = this->state_list;

            // iterate through the states
            for(int i = 0; i < this->state_list.size(); i++) {
                // -1 is used as a sentinal value while iterating
                std::pair<int, int> current_pair = {this->state_list[i].current_state, -1};

                for(int j = i; j < this->state_list.size(); j++) {
                    auto& s1 = s_list[i];
                    auto& s2 = s_list[j];

                    if(s1.next_if_zero == s2.next_if_zero &&
                            s1.next_if_one == s2.next_if_one &&
                            s1.z == s2.z) {
                        
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
        }
        else {
            throw std::runtime_error("StateRepresentation::rowReduce -> only moore machines currently supported");
        }

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
        do {

            int id     = std::stoi(current_node.attr("id").value());
            auto trans = this->split_next_state(current_node.attr("transition").value());
            int out    = std::stoi(current_node.attr("output").value());

            single_state_t s;
            s.current_state = id;
            s.next_if_zero = trans.first;
            s.next_if_one  = trans.second;
            s.z = out;

            this->state_list.push_back(s);

            current_node = current_node.next();
        } while(!current_node.empty());
    }
    else {
        throw std::runtime_error("StateRepresentation -> currently, only moore machines are supported");
    }

}
