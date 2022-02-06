#include <VMWriter/writer.hpp>
#include <iostream>
#include <fstream>

using namespace std;


ostream &operator<<(ostream &out, Segment seg) {
    switch (seg) {
        case Segment::ARG:
            out << "argument";
            break;
        case Segment::CONST:
            out << "constant";
            break;
        case Segment::LOCAL:
            out << "local";
            break;
        case Segment::STATIC:
            out << "static";
            break;
        case Segment::TEMP:
            out << "temp";
            break;
        case Segment::THIS:
            out << "this";
            break;
        case Segment::THAT:
            out << "that";
            break;
        case Segment::POINTER:
            out << "pointer";
            break;
    }

    return out;
}

ostream &operator<<(ostream &out, Command cmd) {
    switch(cmd) {
        case Command::ADD:
            out << "add";
            break;
        case Command::SUB:
            out << "sub";
            break;
        case Command::NEG:
            out << "neg";
            break;
        case Command::NOT:
            out << "not";
            break;
        case Command::EQ:
            out << "eq";
            break;
        case Command::GT:
            out << "gt";
            break;
        case Command::LT:
            out << "lt";
            break;
        case Command::AND:
            out << "and";
            break;
        case Command::OR:
            out << "or";
            break;
    }

    return out;
}

Command charToCommand(char c) {
    switch(c) {
        case '+':
            return Command::ADD;
        case '-':
            return Command::SUB;
        case '&':
            return Command::AND;
        case '|':
            return Command::OR;
        case '<':
            return Command::LT;
        case '>':
            return Command::GT;
        case '=':
            return Command::EQ;
        default:
            throw runtime_error("Unknown symbol " + string(1, c));
    }
}


void VMWriter::switchBuffer(Buffer bf) {
    switch (bf) {
        case Buffer::FILE:
            output = &fileBuffer;
            break;
        case Buffer::STRING:
            output = &stringBuffer;
            break;
    }
}

void VMWriter::writePush(Segment segment, int index) {
    *output << "  push " << segment << " " << index << endl;
}

void VMWriter::writePop(Segment segment, int index) {
    *output << "  pop " << segment << " " << index << endl;
}

void VMWriter::writeArithmetic(Command command) {
    *output << "  " << command << endl;
}

void VMWriter::writeLabel(string label) {
    *output << "label " << label << endl;
}

void VMWriter::writeGoto(string label) {
    *output << "  goto " << label << endl;
}

void VMWriter::writeIf(string label) {
    *output << "  if-goto " << label << endl;
}

void VMWriter::writeCall(string name, int nArgs) {
    *output << "  call " << name << " " << nArgs << endl;
}

void VMWriter::writeFunction(string name, int nLocals) {
    *output << "function " << name << " " << nLocals << endl;
}

void VMWriter::writeReturn() {
    *output << "  return" << endl;
}

void VMWriter::writeNow() {
    fileBuffer << stringBuffer.str();
    stringBuffer.str("");
}
