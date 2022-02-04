#ifndef _ANALYZER_HPP_
#define _ANALYZER_HPP_

#include <filesystem>
#include <string>

using namespace std;
namespace fs = filesystem;


class JackAnalyzer {
    static void analyzeFile(const fs::path &file);
    static void analyzeDir(const fs::path &dir);
public:
    static bool analyze(string arg);
};

#endif
