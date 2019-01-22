#include <iostream>
#include "XmlDocument.h"
#include "XmlNode.h"

using namespace std;

int main(int argc, char* argv[]) {

    XmlDocument doc("config.xml");

    cout << doc << endl;

    return 0;
}