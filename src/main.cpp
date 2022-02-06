#include <JackAnalyzer/analyzer.hpp>
#include <VMWriter/writer.hpp>
#include <iostream>
#include <fstream>

using namespace std;


// test vm writer
void testVMWriter() {
    ofstream f (std::filesystem::path("tes"));
    VMWriter vm {f};

    vm.writePush(Segment::ARG, 0);
    vm.switchBuffer(Buffer::STRING);
    vm.writePush(Segment::ARG, 2);
    vm.switchBuffer(Buffer::FILE);
    vm.writePush(Segment::ARG, 1);
    vm.writeNow();
    /*
     *  Expected output:
     *  push argument 0
     *  push argument 1
     *  push argument 2
     *  push argument 0
     *  push argument 1
     *  push argument 2
     */

    vm.writePush(Segment::ARG, 0);
    vm.switchBuffer(Buffer::STRING);
    vm.writePush(Segment::ARG, 2);
    vm.switchBuffer(Buffer::FILE);
    vm.writePush(Segment::ARG, 1);
    vm.writeNow();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage jc <filename or dirname>" << endl;
        return 1;
    }

    if (argc >= 3) {
        if (string(argv[2]) == "--print-xml") {
            cout << "here!" << endl;
            JackAnalyzer::printXml = true;
        }
    }

    JackAnalyzer::analyze(argv[1]);
}
