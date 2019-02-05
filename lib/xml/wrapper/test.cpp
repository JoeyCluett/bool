#include <iostream>
#include "XmlDocument.h"
#include "XmlNode.h"

using namespace std;

int main(int argc, char* argv[]) {

    XmlDocument doc("config.xml");

    auto root_node = doc.root();

    root_node.format_output(cout);

    return 0;
}