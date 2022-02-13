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

string CompilationEngine::generateLabel() {
    return className + vm.generateLabel();
}

// handle keyword
void CompilationEngine::eat(Keyword key) {
    writeIndent();

    string keyVal = keywordToStr(key);
    if (tokenizer.tokenType() != Token::KEYWORD || tokenizer.keyWord() != key) {
        throw runtime_error(string("Error: Unexpected keyword " + keyVal));
    }

    output << "<keyword> " << keyVal << " </keyword>";
    output << endl;
    tokenizer.advance();
}

Keyword CompilationEngine::eat(vector<Keyword> possibleKeyword) {
    Keyword key = tokenizer.keyWord();

    if (find(possibleKeyword.begin(), possibleKeyword.end(), key) == possibleKeyword.end()) {
        throw runtime_error("Error: Unexpected keyword " + keywordToStr(key));
    } else {
        eat(key);
        return key;
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
string CompilationEngine::eat(Token type) {
    string value;
    writeIndent();

    if (tokenizer.tokenType() != type) {
        throw runtime_error("Error: Unexpected Token");
    }

    switch(type) {
        case Token::IDENTIFIER:
            value = tokenizer.identifier();
            output << "<identifier> " << value << " </identifier>";
            break;
        case Token::INT_CONST:
            value = to_string(tokenizer.intVal());
            output << "<integerConstant> " << value << " </integerConstant>";
            break;
        case Token::STRING_CONST:
            value = tokenizer.stringVal();
            output << "<stringConstant> " << value << " </stringConstant>";
            break;
        default:
            throw runtime_error("Error: expected identifier or constant");
            break;
    }

    output << endl;
    tokenizer.advance();
    
    return value;
}

string CompilationEngine::eatType() {
    if (tokenizer.tokenType() == Token::IDENTIFIER) {
        string ret = tokenizer.identifier();
        eat(Token::IDENTIFIER);
        return ret;
    } else {
        if (tokenizer.tokenType() != Token::KEYWORD) {
            throw runtime_error("Expected keyword");
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

string CompilationEngine::eatSubroutineCall() {
    string callName = eat(Token::IDENTIFIER);
    size_t nArgs = 0;
    if (tokenizer.tokenType() != Token::SYMBOL) {
        throw runtime_error("Error: Expected . or ( in subroutine call");
    }

    if (tokenizer.symbol() == '.') { /* is it a call to another class? */
        eat('.');
        string fnName = eat(Token::IDENTIFIER);

        // is it a call to an object's method or a class's function?
        // if it exists in the symbol table, then it is object's method
        string *objClass = sTable.typeOf(callName);
        Kind *objKind = sTable.kindOf(callName);
        size_t *objIndex = sTable.indexOf(callName);
        if (objClass != nullptr) {
            callName = *objClass + "." + fnName;
            // push object pointer and add nArgs by 1
            vm.writePush(kindToSegment(*objKind), *objIndex);
            nArgs++;
        } else {
            callName += "." + fnName;
        }
    } else {
        callName = className + "." + callName;
        // push the address of this
        // assume this is a method
        vm.writePush(Segment::POINTER, 0);
        nArgs++;
    }

    eat('(');
    nArgs += compileExpressionList();
    eat(')');
    
    // write code for function calls
    vm.writeCall(callName, nArgs);

    return callName;
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
    sTable.startClass();

    eatBegin("class");
    eat(Keyword::CLASS);
    className = tokenizer.identifier();
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
    string name = eat(Token::IDENTIFIER);

    sTable.define(name, type, kind);

    // 0 or more of (',' varName)
    while (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == ',') {
        eat(',');
        name = eat(Token::IDENTIFIER);
        sTable.define(name, type, kind);
    }

    eat(';');

    eatEnd("classVarDec");
}

void CompilationEngine::compileSubroutineDec() {
    // Parse
    eatBegin("subroutineDec");

    // clear subroutine table and switch vm writer buffer to string buffer
    sTable.startSubroutine();
    vm.switchBuffer(Buffer::STRING);

    // ( 'constructor' | 'function' | 'method' )
    Keyword ftype = eat(vector<Keyword> {Keyword::CONSTRUCTOR, Keyword::FUNCTION, Keyword::METHOD});

    // ( 'void' | type )
    if (tokenizer.tokenType() == Token::IDENTIFIER) {
        eat(Token::IDENTIFIER);
    } else {
        eat(vector<Keyword> {Keyword::VOID, Keyword::INT, Keyword::CHAR, Keyword::BOOLEAN});
    }

    // add the 'this' variable in symbol table if subroutine is a method
    if (ftype == Keyword::METHOD) {
        sTable.define("this", className, Kind::ARG);
    }

    string subroutineName = eat(Token::IDENTIFIER);
    eat('(');
    compileParameterList();
    eat(')');

    compileSubroutineBody();
    sTable.printSubroutineTable();

    eatEnd("subroutineDec");

    
    // Write code
    vm.switchBuffer(Buffer::FILE);
    // declare function
    vm.writeFunction(className + "." + subroutineName, sTable.varCount(Kind::VAR));
    switch(ftype) {
        case Keyword::CONSTRUCTOR:
            vm.writePush(Segment::CONST, sTable.varCount(Kind::FIELD));
            vm.writeCall("Memory.alloc", 1);
            vm.writePop(Segment::POINTER, 0);
            break;
        case Keyword::METHOD:
            // this = arg[0]
            vm.writePush(Segment::ARG, 0);
            vm.writePop(Segment::POINTER, 0);
            break;
        case Keyword::FUNCTION:
            break;
        default:
            break;
    }

    // write body
    vm.writeNow();
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

    bool accessingArray = false;

    eat(Keyword::LET);
    string varName = eat(Token::IDENTIFIER);

    string *varType = sTable.typeOf(varName);
    Kind *varKind = sTable.kindOf(varName);
    size_t *varIndex = sTable.indexOf(varName);
    if (varType == nullptr || varKind == nullptr || varIndex == nullptr) {
        throw runtime_error(string("use of undeclared variable " + varName));
    }

    // handle possibility of an array
    if (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == '[') {
        accessingArray = true;
        vm.writePush(kindToSegment(*varKind), *varIndex);

        eat('[');
        compileExpression();
        eat(']');

        vm.writeArithmetic(Command::ADD);
    }

    eat('=');
    compileExpression();
    eat(';');

    if (accessingArray) {
        vm.writePop(Segment::TEMP, 0);
        vm.writePop(Segment::POINTER, 1);
        vm.writePush(Segment::TEMP, 0);
        vm.writePop(Segment::THAT, 0);
    } else {
        vm.writePop(kindToSegment(*varKind), *varIndex);
    }

    eatEnd("letStatement");
}

void CompilationEngine::compileIf() {
    eatBegin("ifStatement");

    string label1 = generateLabel();
    string label2 = generateLabel();

    eat(Keyword::IF);
    eat('(');
    compileExpression();

    vm.writeArithmetic(Command::NOT);
    vm.writeIf(label1);

    eat(')');
    eat('{');
    compileStatements();

    vm.writeGoto(label2);

    eat('}');

    vm.writeLabel(label1);

    // handle possibility of an else statement
    if (tokenizer.tokenType() == Token::KEYWORD && tokenizer.keyWord() == Keyword::ELSE) {
        eat(Keyword::ELSE);
        eat('{');
        compileStatements();
        eat('}');
    }

    vm.writeLabel(label2);

    eatEnd("ifStatement");
}

void CompilationEngine::compileWhile() {
    eatBegin("whileStatement");

    string label1 = generateLabel();
    string label2 = generateLabel();

    vm.writeLabel(label1);

    eat(Keyword::WHILE);
    eat('(');
    compileExpression();
    eat(')');

    vm.writeArithmetic(Command::NOT);
    vm.writeIf(label2);

    eat('{');
    compileStatements();
    eat('}');
    vm.writeGoto(label1);

    vm.writeLabel(label2);

    eatEnd("whileStatement");
}

void CompilationEngine::compileDo() {
    eatBegin("doStatement");

    eat(Keyword::DO);
    eatSubroutineCall();
    eat(';');

    // subroutine is assumed to be void in a do statement
    // therefore it must not return any variable
    vm.writePop(Segment::TEMP, 0);

    eatEnd("doStatement");
}

void CompilationEngine::compileReturn() {
    eatBegin("returnStatement");

    eat(Keyword::RETURN);
    // check if there is an expression
    if (tokenizer.tokenType() != Token::SYMBOL || tokenizer.symbol() != ';') {
        compileExpression();
    } else {
        vm.writePush(Segment::CONST, 0);
    }
    eat(';');

    vm.writeReturn();

    eatEnd("returnStatement");
}

void CompilationEngine::compileExpression() {
    eatBegin("expression");

    compileTerm();
    while (tokenizer.tokenType() == Token::SYMBOL && isOp(tokenizer.symbol())) {
        char op = tokenizer.symbol();
        eat(op);
        compileTerm();

        // write arithmetic command
        switch(op) {
            case '*':
                vm.writeCall("Math.multiply", 2);
                break;
            case '/':
                vm.writeCall("Math.divide", 2);
                break;
            default:
                vm.writeArithmetic(charToCommand(op));
                break;
        }
    }

    eatEnd("expression");
}

void CompilationEngine::compileTerm() {
    eatBegin("term");

    switch(tokenizer.tokenType()) {
        case Token::INT_CONST: {
            string n = eat(Token::INT_CONST);
            vm.writePush(Segment::CONST, stoi(n));
            break;
        }
        case Token::STRING_CONST: {
            string str = eat(Token::STRING_CONST);
            size_t strlen = str.length();
            
            // create a string array with the appropriate length
            vm.writePush(Segment::CONST, strlen);
            vm.writeCall("String.new", 1);

            // append char to string array
            for (size_t i = 0; i < strlen; i++) {
                // push char c
                vm.writePush(Segment::CONST, int(str[i]));
                // call String.appendChar
                vm.writeCall("String.appendChar", 2);
            }
            break;
        }
        case Token::IDENTIFIER: {
            // determine whether the term is varName or varName[expression] or subroutineCall
            tokenizer.advance();
            if (tokenizer.tokenType() == Token::SYMBOL) {
                char sym = tokenizer.symbol();
                if (sym == '(' || sym == '.') {
                    tokenizer.backtrack();
                    eatSubroutineCall();
                    break;  /* break from the switch statement */
                } else if (sym == '[') {
                    tokenizer.backtrack();

                    string arrName = eat(Token::IDENTIFIER);
                    string *arrType = sTable.typeOf(arrName);
                    Kind *arrKind = sTable.kindOf(arrName);
                    size_t *arrLocation = sTable.indexOf(arrName);

                    // Error checking
                    if (arrType == nullptr || arrKind == nullptr || arrLocation == nullptr) {
                        throw runtime_error(string("Use of undeclared array " + arrName));
                    }

                    vm.writePush(kindToSegment(*arrKind), *arrLocation);

                    eat('[');
                    compileExpression();
                    eat(']');

                    vm.writeArithmetic(Command::ADD);
                    vm.writePop(Segment::POINTER, 1);
                    vm.writePush(Segment::THAT, 0);

                    break;
                }
            }

            // if neither varName[expression] nor subroutineCall
            tokenizer.backtrack();
            string varName = eat(Token::IDENTIFIER);

            // get the location of variable in memory
            size_t *varIndex = sTable.indexOf(varName);
            Kind *varKind = sTable.kindOf(varName);
            if (varIndex == nullptr) {
                throw runtime_error(string("Undeclared variable: " + varName));
            }
            // write the code to push the variable
            vm.writePush(kindToSegment(*varKind), *varIndex);

            break;
        }
        case Token::SYMBOL: {
                // check if it is unary operator or parantheses
                if (isUnaryOp(tokenizer.symbol())) {
                    char op = tokenizer.symbol();
                    eat(op);
                    compileTerm();
                    switch(op) {
                        case '-':
                            vm.writeArithmetic(Command::NEG);
                            break;
                        case '~':
                            vm.writeArithmetic(Command::NOT);
                            break;
                        default:
                            break;
                    }
                } else {
                    eat('(');
                    compileExpression();
                    eat(')');
                }
                break;
        }
        case Token::KEYWORD: {
            Keyword k = eat(vector<Keyword> {Keyword::TRUE, Keyword::FALSE, Keyword::kNULL, Keyword::THIS});
            switch(k) {
                case Keyword::TRUE:
                    vm.writePush(Segment::CONST, 1);
                    vm.writeArithmetic(Command::NEG);
                    break;
                case Keyword::FALSE:
                case Keyword::kNULL:
                    vm.writePush(Segment::CONST, 0);
                    break;
                case Keyword::THIS:
                    vm.writePush(Segment::POINTER, 0);
                    break;
                default:
                    break;
            }
            break;
        }
    }

    eatEnd("term");
}

size_t CompilationEngine::compileExpressionList() {
    eatBegin("expressionList");

    // handle empty expression list
    if (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() != '(' && !isUnaryOp(tokenizer.symbol())) {
        eatEnd("expressionList");
        return 0;
    }

    size_t count = 1;
    compileExpression();
    while (tokenizer.tokenType() == Token::SYMBOL && tokenizer.symbol() == ',') {
        count++;
        eat(',');
        compileExpression();
    }

    eatEnd("expressionList");
    return count;
}

/* End Public Methods */
