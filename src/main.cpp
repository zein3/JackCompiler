#include <JackAnalyzer/analyzer.hpp>
#include <VMWriter/writer.hpp>
#include <iostream>
#include <fstream>

using namespace std;


int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage jc <filename or dirname>" << endl;
        return 1;
    }

    if (argc >= 3) {
        if (string(argv[2]) == "--print-xml") {
            JackAnalyzer::printXml = true;
        }
    }

    JackAnalyzer::analyze(argv[1]);
}
