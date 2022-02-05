#include <CompilationEngine/cengine.hpp>
#include <SymbolTable/table.hpp>
#include <SymbolTable/var.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;


/* Begin Helper Function */

Kind keywordToKind(Keyword key) {
    switch(key) {
        case Keyword::STATIC:
            return Kind::STATIC;
        case Keyword::FIELD:
            return Kind::FIELD;
        case Keyword::VAR:
            return Kind::VAR;
        default:
            throw runtime_error("Can't convert keyword to kind.");
    }
}

/* End Helper Function */

/* Begin Private Methods */

bool CompilationEngine::isOp(char s) {
    if (s == '+' || s == '-' || s == '*' ||
            s == '/' || s == '&' ||
            s == '|' || s == '<' ||
            s == '>' || s == '=') {
        return true;
    }
    return false;
}

bool CompilationEngine::isUnaryOp(char s) {
    if (s == '-' || s == '~') {
        return true;
    }
    return false;
}

string CompilationEngine::keywordToStr(Keyword key) {
    auto result = Tokenizer::KEYWORDMAPPING.right.find(key);
    if (result == Tokenizer::KEYWORDMAPPING.right.end()) {
        throw runtime_error("Error: Unknown keyword");
    } else {
        return result->second;
    }
}

// handle keyword
void CompilationEngine::eat(Keyword key) {
    writeIndent();

    string keyVal = keywordToStr(key);
    if (tokenizer.tokenType() != Token::KEYWORD || tokenizer.keyWord() != key) {
        throw runtime_error("Error: Unexpected keyword " + keyVal);
    }

    output << "<keyword> " << keyVal << " </keyword>";
    output << endl;
    tokenizer.advance();
}

void CompilationEngine::eat(vector<Keyword> possibleKeyword) {
    Keyword key = tokenizer.keyWord();

    if (find(possibleKeyword.begin(), possibleKeyword.end(), key) == possibleKeyword.end()) {
        throw runtime_error("Error: Unexpected keyword " + keywordToStr(key));
    } else {
        eat(key);
    }
}

// handle symbol
void CompilationEngine::eat(char symbol) {
    writeIndent();

    if (tokenizer.tokenType() != Token::SYMBOL || tokenizer.symbol() != symbol) {
        throw runtime_error("Error: Expected symbol " + string(1, symbol));
    }

    output << "<symbol> ";
    // escape xml
    switch(symbol) {
        case '<':
            output << "&lt;";
            break;
        case '>':
            output << "&gt;";
            break;
        case '&':
            output << "&amp;";
            break;
        default:
            output << symbol;
            break;
    }
    output << " </symbol>";

    output << endl;
    tokenizer.advance();
}

// handle identifier, integer constant, and string constant
void CompilationEngine::eat(Token type) {
    writeIndent();

    if (tokenizer.tokenType() != type) {
        throw runtime_error("Error: Unexpected Token");
    }

    switch(type) {
        case Token::IDENTIFIER:
            output << "<identifier> " << tokenizer.identifier() << " </identifier>";
            break;
        case Token::INT_CONST:
            output << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>";
            break;
        case Token::STRING_CONST:
            output << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>";
            break;
        default:
            throw runtime_error("Error: expected identifier or constant");
            break;
    }

    output << endl;
    tokenizer.advance();
}

string CompilationEngine::eatType() {
    if (tokenizer.tokenType() == Token::IDENTIFIER) {
        string ret = tokenizer.identifier();
        eat(Token::IDENTIFIER);
        return ret;
    } else {
        if (tokenizer.tokenType() != Token::KEYWORD) {
            throw runtime_error("Expected type keyword");
        }

        Keyword varType = tokenizer.keyWord();
        if (varType != Keyword::INT && varType != Keyword::CHAR && varType != Keyword::BOOLEAN) {
            throw runtime_error("Expected type keyword");
        }

        string ret = keywordToStr(varType);
        eat(varType);
        return ret;
    }
}

void CompilationEngine::eatSubroutineCall() {
    eat(Token::IDENTIFIER);
    if (tokenizer.tokenType() != Token::SYMBOL) {
        throw runtime_error("Error: Expected . or ( in subroutine call");
    }

    if (tokenizer.symbol() == '.') {
        eat('.');
        eat(Token::IDENTIFIER);
    }

    eat('(');
    compileExpressionList();
    eat(')');
}

void CompilationEngine::eatBegin(string tag) {
    writeIndent();
    output << "<" << tag << ">" << endl;
    indent += INDENTSIZE;
}

void CompilationEngine::eatEnd(string tag) {
    indent -= INDENTSIZE;
    writeIndent();
    output << "</" << tag << ">" << endl;
}

void CompilationEngine::writeIndent() {
    output << string(indent, ' ');
}

/* End Private Methods */



/* Begin Public Methods */

void CompilationEngine::compileClass() {
    eatBegin("class");
    eat(Keyword::CLASS);
    eat(Token::IDENTIFIER);
    eat('{');

    // 0 or more class variable declaration
    while (true) {
        Token type = tokenizer.tokenType();
        if (type != Token::KEYWORD) break;
        Keyword key = tokenizer.keyWord();
        if (key != Keyword::STATIC && key != Keyword::FIELD) break;

        compileClassVarDec();
    }

    // 0 or more subroutine declaration
    while (true) {
        Token type = tokenizer.tokenType();
        if (type != Token::KEYWORD && type != Token::IDENTIFIER) break;

        compileSubroutineDec();
    }

    eat('}');
    eatEnd("class");

    sTable.printClassTable();
}

void CompilationEngine::compileClassVarDec() {
    eatBegin("classVarDec");

    // static or field
    Kind kind = keywordToKind(tokenizer.keyWord());
    eat(vector<Keyword> {Keyword::STATIC, Keyword::FIELD});
    string type = eatType();
    string name = tokenizer.identifier();
    eat(Token::IDENTIFIER);

    sTable.define(name, type, kind);

    // 0 or more of (',' varName)
    while (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == ',') {
        eat(',');
        eat(Token::IDENTIFIER);
    }

    eat(';');

    eatEnd("classVarDec");
}

void CompilationEngine::compileSubroutineDec() {
    eatBegin("subroutineDec");

    sTable.startSubroutine();

    // ( 'constructor' | 'function' | 'method' )
    eat(vector<Keyword> {Keyword::CONSTRUCTOR, Keyword::FUNCTION, Keyword::METHOD});

    // ( 'void' | type )
    if (tokenizer.tokenType() == Token::IDENTIFIER) {
        eat(Token::IDENTIFIER);
    } else {
        eat(vector<Keyword> {Keyword::VOID, Keyword::INT, Keyword::CHAR, Keyword::BOOLEAN});
    }

    eat(Token::IDENTIFIER);
    eat('(');
    compileParameterList();
    eat(')');
    compileSubroutineBody();

    sTable.printSubroutineTable();

    eatEnd("subroutineDec");
}

void CompilationEngine::compileParameterList() {
    eatBegin("parameterList");

    // if not type
    if (tokenizer.tokenType() != Token::KEYWORD && tokenizer.tokenType() != Token::IDENTIFIER) {
        eatEnd("parameterList");
        return;
    }

    string type = eatType();
    string name = tokenizer.identifier();
    eat(Token::IDENTIFIER);

    sTable.define(name, type, Kind::ARG);

    // 0 or more of (',' varName)
    while (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == ',') {
        eat(',');
        string type = eatType();
        string name = tokenizer.identifier();
        eat(Token::IDENTIFIER);

        sTable.define(name, type, Kind::ARG);
    }

    eatEnd("parameterList");
}

void CompilationEngine::compileSubroutineBody() {
    eatBegin("subroutineBody");
    
    eat('{');

    while (tokenizer.tokenType() == Token::KEYWORD && tokenizer.keyWord() == Keyword::VAR) {
        compileVarDec();
    }

    compileStatements();

    eat('}');

    eatEnd("subroutineBody");
}

void CompilationEngine::compileVarDec() {
    eatBegin("varDec");

    eat(Keyword::VAR);
    string type = eatType();
    string name = tokenizer.identifier();
    eat(Token::IDENTIFIER);

    sTable.define(name, type, Kind::VAR);

    // 0 or more of (',' varName)
    while (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == ',') {
        eat(',');
        string name = tokenizer.identifier();
        eat(Token::IDENTIFIER);
        sTable.define(name, type, Kind::VAR);
    }

    eat(';');

    eatEnd("varDec");
}

void CompilationEngine::compileStatements() {
    eatBegin("statements");

    while (tokenizer.tokenType() == Token::KEYWORD) {
        switch(tokenizer.keyWord()) {
            case Keyword::LET:
                compileLet();
                break;
            case Keyword::IF:
                compileIf();
                break;
            case Keyword::WHILE:
                compileWhile();
                break;
            case Keyword::DO:
                compileDo();
                break;
            case Keyword::RETURN:
                compileReturn();
                break;
            default:
                throw runtime_error("Error: Invalid statement");
                break;
        }
    }

    eatEnd("statements");
}

void CompilationEngine::compileLet() {
    eatBegin("letStatement");

    eat(Keyword::LET);
    eat(Token::IDENTIFIER);

    // handle possibility of an array
    if (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == '[') {
        eat('[');
        compileExpression();
        eat(']');
    }

    eat('=');
    compileExpression();
    eat(';');

    eatEnd("letStatement");
}

void CompilationEngine::compileIf() {
    eatBegin("ifStatement");

    eat(Keyword::IF);
    eat('(');
    compileExpression();
    eat(')');
    eat('{');
    compileStatements();
    eat('}');

    // handle possibility of an else statement
    if (tokenizer.tokenType() == Token::KEYWORD && tokenizer.keyWord() == Keyword::ELSE) {
        eat(Keyword::ELSE);
        eat('{');
        compileStatements();
        eat('}');
    }

    eatEnd("ifStatement");
}

void CompilationEngine::compileWhile() {
    eatBegin("whileStatement");

    eat(Keyword::WHILE);
    eat('(');
    compileExpression();
    eat(')');
    eat('{');
    compileStatements();
    eat('}');

    eatEnd("whileStatement");
}

void CompilationEngine::compileDo() {
    eatBegin("doStatement");

    eat(Keyword::DO);
    eatSubroutineCall();
    eat(';');

    eatEnd("doStatement");
}

void CompilationEngine::compileReturn() {
    eatBegin("returnStatement");

    eat(Keyword::RETURN);
    // check if there is an expression
    if (tokenizer.tokenType() != Token::SYMBOL || tokenizer.symbol() != ';') {
        compileExpression();
    }
    eat(';');

    eatEnd("returnStatement");
}

void CompilationEngine::compileExpression() {
    eatBegin("expression");

    compileTerm();
    while (tokenizer.tokenType() == Token::SYMBOL && isOp(tokenizer.symbol())) {
        eat(tokenizer.symbol());
        compileTerm();
    }

    eatEnd("expression");
}

void CompilationEngine::compileTerm() {
    eatBegin("term");

    switch(tokenizer.tokenType()) {
        case Token::INT_CONST:
            eat(Token::INT_CONST);
            break;
        case Token::STRING_CONST:
            eat(Token::STRING_CONST);
            break;
        case Token::IDENTIFIER:
            {
                // determine whether the term is varName or varName[expression] or subroutineCall
                tokenizer.advance();
                if (tokenizer.tokenType() == Token::SYMBOL) {
                    char sym = tokenizer.symbol();
                    if (sym == '(' || sym == '.') {
                        tokenizer.backtrack();
                        eatSubroutineCall();
                        break;
                    } else if (sym == '[') {
                        tokenizer.backtrack();
                        eat(Token::IDENTIFIER);
                        eat('[');
                        compileExpression();
                        eat(']');
                        break;
                    }
                }

                // if neither varName[expression] nor subroutineCall
                tokenizer.backtrack();
                eat(Token::IDENTIFIER);
                break;
            }
        case Token::SYMBOL:
            {
                // check if it is unary operator or parantheses
                if (isUnaryOp(tokenizer.symbol())) {
                    eat(tokenizer.symbol());
                    compileTerm();
                } else {
                    eat('(');
                    compileExpression();
                    eat(')');
                }
                break;
            }
        case Token::KEYWORD:
            eat(vector<Keyword> {Keyword::TRUE, Keyword::FALSE, Keyword::kNULL, Keyword::THIS});
            break;
    }

    eatEnd("term");
}

void CompilationEngine::compileExpressionList() {
    eatBegin("expressionList");

    // handle empty expression list
    if (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() != '(') {
        eatEnd("expressionList");
        return;
    }

    compileExpression();
    while (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == ',') {
        eat(',');
        compileExpression();
    }

    eatEnd("expressionList");
}

/* End Public Methods */
