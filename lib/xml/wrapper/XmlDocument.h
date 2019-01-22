#pragma once

#include "../rapidxml.hpp"
#include "../rapidxml_utils.hpp"
#include "XmlNode.h"

#include <vector>
#include <list>
#include <fstream>
#include <map>
#include <iostream>

using namespace rapidxml;

class XmlDocument {
private:
    xml_document<> doc;
    std::vector<char> buf;

    // recursive function call
    void print(XmlNode node, std::string indent, std::ostream& os) {
        if(node.empty() == false) {
            os << indent << node.name() << std::endl;

            {
                auto attr = node.attr();
                while(attr.empty() == false) {
                    os << indent+"    > " << attr.name() << " = " << attr.value() << std::endl;
                    attr = attr.next();
                }
            }

            print(node.child(), indent + "  ", os);
            print(node.next(), indent, os);

        } else {
            return;
        }
    }

public:
    XmlDocument(const std::string& filename) {
        std::ifstream is(filename);
        std::vector<char> buf((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        buf.push_back('\0');

        this->buf = buf;
        this->doc.parse<0>(&this->buf[0]);
    }

    XmlNode root(void) {
        return XmlNode(this->doc.first_node());
    }

    friend std::ostream& operator<<(std::ostream& os, XmlDocument& xmld) {
        xmld.print(xmld.root(), "", os);
        return os;
    }

};
