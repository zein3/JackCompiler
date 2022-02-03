#ifndef _COMPILATIONENGINE_HPP_
#define _COMPILATIONENGINE_HPP_

#include <JackTokenizer/tokenizer.hpp>
#include <fstream>
#include <string>

using namespace std;


class CompilationEngine {
    size_t indent = 0;
    ostream &output;
    Tokenizer tokenizer;

    const size_t INDENTSIZE = 2;
    
    /* eat: create and verify the token and advance the tokenizer
     * to handle keyword: use the appropriate keyword enum
     * to handle char: put the symbol as a char
     * to handle the rest: just put the type
     */
    void eat(Keyword key);      /* handle keyword */
    void eat(char symbol);      /* handle symbol  */
    void eat(Token type);       /* handle identifier, integer constant, and string constant */

    void eatBegin(string tag);  /* create a beginning tag like this <whileStatement> and increase indent by 2 */
    void eatEnd(string tag);    /* create an ending tag like this </whileStatement> and decrease indent by 2 */
    void writeIndent();         /* create an indentation according to indent var */

    CompilationEngine() = delete;
public:
    CompilationEngine(istream &in, ostream &out) : output{out}, tokenizer{in} {}

    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
};

#endif
