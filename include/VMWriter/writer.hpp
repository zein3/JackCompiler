#ifndef _VMWRITER_HPP_
#define _VMWRITER_HPP_

#include "SymbolTable/var.hpp"
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
Segment kindToSegment(Kind k);

class VMWriter {
    /*
     *  file buffer stream and string buffer stream is created
     *  because when creating a function in jack vm language
     *  we need to know the number of local variables, yet
     *  there is no way to tell until we actually compile
     *  the subroutine bode, therefore before compiling
     *  the body, we switch to the string buffer, and write there
     *  then after we finish that, we switch to the file buffer (the real output stream)
     *  write the function declaration, and then call writeNow() to
     *  get the string buffer stream and put it into the file buffer
     *
     *  TLDR: put code for body in temporary var stringBuffer, output code for
     *  function declaration, add stringBuffer after function declaration
     */
    ostream &fileBuffer;                            /* buffer for file, don't use this stream directly */
    ostringstream stringBuffer;                     /* buffer for string, don't use this stream directly */
    ostream *output;
    size_t uniq = 0;                                /* starting id for generating a unique id */

    inline static const size_t INDENT_SIZE = 4;

    VMWriter() = delete;

    size_t getUniq();                               /* generate a unique id */
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
    string generateLabel();                         /* generate unique lable */
};

#endif
