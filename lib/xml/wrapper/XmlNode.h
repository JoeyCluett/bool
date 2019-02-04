#ifndef __JJC__XML__NODE__H__
#define __JJC__XML__NODE__H__

#include <string>
#include <initializer_list>
#include <stdexcept>
#include <map>

#include "../rapidxml.hpp"
#include "../rapidxml_utils.hpp"

using namespace rapidxml;

class XmlAttribute {
private:
    xml_attribute<>* _attr;

public:
    XmlAttribute(xml_attribute<>* _attr) : _attr(_attr) { }

    std::string name(void) {
        if(_attr == NULL)
            return "";
        else
            return this->_attr->name();
    }

    std::string value(void) {
        if(_attr == NULL)
            return "";
        else
            return this->_attr->value();
    }

    XmlAttribute next(void) {
        return XmlAttribute(this->_attr->next_attribute());
    }

    bool empty(void) {
        return (_attr == NULL);
    }

};

class XmlNode {
private:
    xml_node<>* _node;

public:
    XmlNode(xml_node<>* _node) : _node(_node) { }

    XmlNode child(const std::string& name) {
        return XmlNode(this->_node->first_node(name.c_str()));
    }

    XmlNode child(void) {
        return XmlNode(this->_node->first_node());
    }

    XmlNode next(void) {
        return XmlNode(this->_node->next_sibling());
    }

    XmlAttribute attr(const std::string& attr_name) {
        return XmlAttribute(this->_node->first_attribute(attr_name.c_str()));
    }

    XmlAttribute attr(void) {
        return XmlAttribute(this->_node->first_attribute());
    }

    bool empty(void) {
        return (_node == NULL);
    }

    std::string name(void) {
        return this->_node->name();
    }

    // just a convenience function to make sure a given node 
    // has all of the attributes needed by the programmer
    bool hasAttrs(std::vector<std::string> str_vec, bool should_throw = false) {
        for(auto& str : str_vec) {
            auto attr_node = this->attr(str);
            if(attr_node.empty()) {
                // attribute is missing, optionaly throw an exception
                if(should_throw) {
                    throw std::runtime_error("XmlNode::hasAttrs -> attribute '" 
                            + str + "' is missing\n");
                } else {
                    return false;
                }
            }
        }

        return true;
    }

    // ensure that there arent any extra attributes that the 
    // programmer did not request
    bool hasOnlyAttrs(std::vector<std::string> str_vec, bool should_throw = false) {
        // throw all of the attributes in a string:bool map
        std::map<std::string, bool> str_bool_map;
        for(auto& str : str_vec)
            str_bool_map.insert({str, true});

        auto attr_node = this->attr();
        while(!attr_node.empty()) {

            if(str_bool_map.find(attr_node.name()) == str_bool_map.end()) {
                // attribute is not in vector, optionally throw
                if(should_throw) {
                    throw std::runtime_error("XmlNode::hasOnlyAttrs -> extra attribute '"
                            + attr_node.name() + "' found");
                } else {
                    return false;
                }
            }

            attr_node = attr_node.next();
        }

        return true;
    }

};

#endif // __JJC__XML__NODE__H__