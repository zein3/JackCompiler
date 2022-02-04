#include <JackAnalyzer/analyzer.hpp>
#include <CompilationEngine/cengine.hpp>
#include <filesystem>
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;
namespace fs = filesystem;


void JackAnalyzer::analyzeFile(const fs::path &file) {
    string filename = file.string();
    filename = filename.substr(0, filename.find_last_of('.'));

    fs::path output = fs::path(filename + ".xml");

    ifstream in (file);
    ofstream out (output);

    CompilationEngine engine(in, out);
    engine.compileClass();
}

void JackAnalyzer::analyzeDir(const fs::path &dir) {
    for (const auto &entry : fs::directory_iterator(dir)) {
        // if file ends with .jack, compile it
        if (boost::algorithm::ends_with(entry.path().string(), ".jack")) {
            analyzeFile(entry.path());
        }
    }
}

bool JackAnalyzer::analyze(string arg) {
    // check if argument is a directory
    fs::path dir = fs::path(arg);
    auto dirStatus = fs::status(dir);
    if (dirStatus.type() == fs::file_type::directory) {
        analyzeDir(dir);
        return true;
    }

    // check if argument is a .jack file
    fs::path jackFile = fs::path(arg + ".jack");
    if (fs::exists(jackFile)) {
        analyzeFile(jackFile);
        return true;
    }

    cout << "File not found." << endl;
    return false;
}
