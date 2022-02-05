#include <VMWriter/writer.hpp>
#include <iostream>
#include <fstream>

using namespace std;


ostream &operator<<(ostream &out, Segment seg) {
    switch (seg) {
        case Segment::ARG:
            break;
        case Segment::CONST:
            break;
        case Segment::LOCAL:
            break;
        case Segment::STATIC:
            break;
        case Segment::TEMP:
            break;
        case Segment::THIS:
            break;
        case Segment::THAT:
            break;
        case Segment::POINTER:
            break;
    }

    return out;
}

ostream &operator<<(ostream &out, Command cmd) {
    switch(cmd) {
        case Command::ADD:
            break;
        case Command::SUB:
            break;
        case Command::NEG:
            break;
        case Command::NOT:
            break;
        case Command::EQ:
            break;
        case Command::GT:
            break;
        case Command::LT:
            break;
        case Command::AND:
            break;
        case Command::OR:
            break;
    }

    return out;
}
