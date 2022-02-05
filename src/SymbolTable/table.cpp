#include <SymbolTable/table.hpp>
#include <SymbolTable/var.hpp>
#include <stdexcept>

using namespace std;


Var SymbolTable::find(string name) {
    auto resultSubroutine = subroutineTable.find(name);
    if (resultSubroutine != subroutineTable.end()) {
        return resultSubroutine->second;
    }

    auto resultClass = classTable.find(name);
    if (resultClass != classTable.end()) {
        return resultClass->second;
    }

    throw runtime_error("Error: use of undeclared variable");
}


void SymbolTable::startSubroutine() {
    subroutineTable.clear();
}

void SymbolTable::define(string name, string type, Kind kind) {
    Var newVar;
    newVar.type = type;
    newVar.kind = kind;
    newVar.index = varCount(kind);

    switch (kind) {
        case Kind::ARG:
        case Kind::VAR:
            subroutineTable.insert({name, newVar});
            break;
        case Kind::FIELD:
        case Kind::STATIC:
            classTable.insert({name, newVar});
            break;
    }
}

size_t SymbolTable::varCount(Kind kind) {
    size_t count = 0;
    switch(kind) {
        case Kind::ARG:
        case Kind::VAR: {
                for (auto it = subroutineTable.begin(); it != subroutineTable.end(); it++) {
                    if (it->second.kind == kind) {
                        count++;
                    }
                }
                break;
            }
        case Kind::FIELD:
        case Kind::STATIC: {
                for (auto it = classTable.begin(); it != classTable.end(); it++) {
                    if (it->second.kind == kind) {
                        count++;
                    }
                }
                break;
            }
    }

    return count;
}

Kind SymbolTable::kindOf(string name) {
    return find(name).kind;
}

string SymbolTable::typeOf(string name) {
    return find(name).type;
}

size_t SymbolTable::indexOf(string name) {
    return find(name).index;
}
