#ifndef _SYMBOLTABLE_HPP_
#define _SYMBOLTABLE_HPP_

#include <SymbolTable/var.hpp>
#include <unordered_map>

using namespace std;


class SymbolTable {
    unordered_map <string, Var> classTable;
    unordered_map <string, Var> subroutineTable;

    Var *find(string name);
public:
    SymbolTable();
    
    void startSubroutine();
    void startClass();                                  /* clear the class table of field, leaving only static variables */
    void define(string name, string type, Kind kind);
    size_t varCount(Kind kind);

    Kind *kindOf(string name);
    string *typeOf(string name);
    size_t *indexOf(string name);

    /* Used for testing */
    void printClassTable();
    void printSubroutineTable();
};

#endif
