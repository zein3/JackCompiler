#ifndef _ANALYZER_HPP_
#define _ANALYZER_HPP_

#include <SymbolTable/table.hpp>
#include <filesystem>
#include <string>

using namespace std;
namespace fs = filesystem;


class JackAnalyzer {
    static void analyzeFile(const fs::path &file);
    static void analyzeDir(const fs::path &dir);
public:
    inline static bool printXml = false;
    inline static SymbolTable table;

    static bool analyze(string arg);
};

#endif
