#include <SymbolTable/table.hpp>
#include <SymbolTable/var.hpp>
#include <stdexcept>
#include <iostream>

using namespace std;


SymbolTable::SymbolTable() {
}

Var *SymbolTable::find(string name) {
    auto resultSubroutine = subroutineTable.find(name);
    if (resultSubroutine != subroutineTable.end()) {
        return &resultSubroutine->second;
    }

    auto resultClass = classTable.find(name);
    if (resultClass != classTable.end()) {
        return &resultClass->second;
    }

    return nullptr;
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

Kind *SymbolTable::kindOf(string name) {
    auto result = find(name);
    if (result) return &result->kind;
    else return nullptr;
}

string *SymbolTable::typeOf(string name) {
    auto result = find(name);
    if (result) return &result->type;
    else return nullptr;
}

size_t *SymbolTable::indexOf(string name) {
    auto result = find(name);
    if (result) return &result->index;
    else return nullptr;
}


/* Methods for testing */

ostream &operator<<(ostream &out, Kind kind) {
    switch(kind) {
        case Kind::ARG:
            out << "argument";
            break;
        case Kind::FIELD:
            out << "class variable";
            break;
        case Kind::STATIC:
            out << "static variable";
            break;
        case Kind::VAR:
            out << "local variable";
            break;
    }

    return out;
}

void SymbolTable::printClassTable() {
#if DEBUG
    cout << "Class Table: " << endl;
    for (auto it = classTable.begin(); it != classTable.end(); it++) {
        cout << it->first << ", " << it->second.type << ", " << it->second.kind << ", " << it->second.index << endl;
    }

    cout << endl << endl;
#endif
}

void SymbolTable::printSubroutineTable() {
#if DEBUG
    cout << "Subroutine Table: " << endl;
    for (auto it = subroutineTable.begin(); it != subroutineTable.end(); it++) {
        cout << it->first << ", " << it->second.type << ", " << it->second.kind << ", " << it->second.index << endl;
    }

    cout << endl << endl;
#endif
}
