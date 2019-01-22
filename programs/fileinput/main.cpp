#include <iostream>
#include <StateRepresentation.h>

using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "Usage:\n" << argv[0] << " <input file>\n";
        return 1;
    }

    StateRepresentation sr(argv[1]);
    cout << sr << endl << endl;

    sr.rowReduce();
    cout << sr << endl << endl;

    return 0;
}
