#include <CompilationEngine/cengine.hpp>
#include <fstream>
#include <string>

using namespace std;


/* Begin Private Methods */

string CompilationEngine::keywordToStr(Keyword key) {
    auto result = Tokenizer::KEYWORDMAPPING.right.find(key);
    if (result == Tokenizer::KEYWORDMAPPING.right.end()) {
        throw "Error: Unknown keyword";
    } else {
        return result->second;
    }
}

// handle keyword
void CompilationEngine::eat(Keyword key) {
    writeIndent();

    string keyVal = keywordToStr(key);
    if (tokenizer.tokenType() != Token::KEYWORD || tokenizer.keyWord() != key) {
        throw "Error: Unexpected keyword " + keyVal;
    }

    output << "<keyword> " << keyVal << " </keyword>";
    output << endl;
    tokenizer.advance();
}

void CompilationEngine::eat(vector<Keyword> &possibleKeyword) {
    Keyword key = tokenizer.keyWord();

    if (find(possibleKeyword.begin(), possibleKeyword.end(), key) == possibleKeyword.end()) {
        throw "Error: Unexpected keyword " + keywordToStr(key);
    } else {
        eat(key);
    }
}

// handle symbol
void CompilationEngine::eat(char symbol) {
    writeIndent();

    if (tokenizer.tokenType() != Token::SYMBOL || tokenizer.symbol() != symbol) {
        throw "Error: Expected symbol " + string(1, symbol);
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
        throw "Error: Unexpected Token";
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
            throw "Error: expected identifier or constant";
            break;
    }

    output << endl;
    tokenizer.advance();
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
}

void CompilationEngine::compileClassVarDec() {
    eatBegin("classVarDec");

    // static or field

    eatEnd("classVarDec");
}

/* End Public Methods */
