#ifndef _VMWRITER_HPP_
#define _VMWRITER_HPP_

#include <fstream>

using namespace std;


enum class Segment {
    CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP
};

enum class Command {
    ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT
};

void operator<<(ostream &out, Segment seg);
void operator<<(ostream &out, Command cmd);

class VMWriter {
    ostream &output;

    VMWriter() = delete;
public:
    VMWriter(ostream &out) :output {out} {}
    void writePush(Segment segment, int index);
    void writePop(Segment segment, int index);
    void writeArithmetic(Command command);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string name, int nArgs);
    void writeFunction(string name, int nLocals);
    void writeReturn();
};

#endif
