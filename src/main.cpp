#include <JackAnalyzer/analyzer.hpp>
#include <iostream>

using namespace std;


int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage jc <filename or dirname>" << endl;
        return 1;
    }

    JackAnalyzer::analyze(argv[1]);
}
