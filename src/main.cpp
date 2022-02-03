#include <CompilationEngine/cengine.hpp>
#include <iostream>

using namespace std;


int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage jc <filename or dirname>" << endl;
        return 1;
    }

    string name = argv[1];
    string inputName = name + ".jack";
    string outputName = name + "Out.xml";

    ifstream in(inputName);
    if (in.good()) {
        ofstream out(outputName);
        CompilationEngine engine (in, out);
        engine.compileClass();
    }

    //ifstream f(fname);
    //if (f.good()) {
        //// user asks to compile one file
        //Tokenizer tknzr (f);

        //string outName = name + "T.xml";
        //ofstream out(outName);
        //if (!out) {
            //return 1;
        //}
        
        //tknzr.writeOutput(out);
    //} else {
        //// user asks to compile a directory
    //}

    return 0;
}
