#ifndef __JJC__XML__NODE__H__
#define __JJC__XML__NODE__H__

#include <string>

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

};

#endif // __JJC__XML__NODE__H__