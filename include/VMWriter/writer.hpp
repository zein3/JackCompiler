#ifndef _VMWRITER_HPP_
#define _VMWRITER_HPP_

#include <fstream>
#include <sstream>

using namespace std;


enum class Segment {
    CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP
};

enum class Command {
    ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT
};

enum class Buffer {
    STRING, FILE
};

ostream &operator<<(ostream &out, Segment seg);
ostream &operator<<(ostream &out, Command cmd);
Command charToCommand(char c);

class VMWriter {
    ostream &fileBuffer;
    ostringstream stringBuffer;
    ostream *output;

    VMWriter() = delete;
public:
    VMWriter(ostream &out) : fileBuffer {out}, output {&fileBuffer} {}

    void switchBuffer(Buffer bf);

    void writePush(Segment segment, int index);
    void writePop(Segment segment, int index);
    void writeArithmetic(Command command);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string name, int nArgs);
    void writeFunction(string name, int nLocals);
    void writeReturn();

    void writeNow();                                /* write the content of string buffer to file buffer, and empty string buffer */
};

#endif
