#include <iostream>
#include <StateRepresentation.h>

using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "Usage:\n" << argv[0] << " <input file>\n";
        return 1;
    }

    cout << "Importing file..." << flush;
    StateRepresentation sr(argv[1]);
    cout << "DONE\n\n" << flush;

    cout << sr.info() << endl << endl;

    cout << sr << endl;

    cout << "Row reducing..." << flush;
    sr.rowReduce();
    cout << "DONE\n\n" << flush;

    cout << sr << endl;

    return 0;
}
