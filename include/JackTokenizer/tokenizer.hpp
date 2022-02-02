#ifndef _TOKENIZER_HPP_
#define _TOKENIZER_HPP_

#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <boost/bimap.hpp>

using namespace std;


enum class Token {
    KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST
};

enum class Keyword {
    CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN,
    CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE,
    WHILE, RETURN, TRUE, FALSE, kNULL, THIS
};

class Tokenizer {
    vector<string> tokens;
    size_t tokenCounter = 0;

    Tokenizer() = delete;
    void splitIntoTokens(string &code);
    void addToTokens(string &token);
    void addToTokens(char c);
public:
    static const vector<string> KEYWORDLIST;
    static const string SYMBOLLIST;
    static const regex IDENTIFIERREGEX;
    //static const map<string, Keyword> KEYWORDMAPPING;
    static const boost::bimap<string, Keyword> KEYWORDMAPPING;

    Tokenizer(istream &input);
    
    bool hasMoreTokens();
    void advance();
    Token tokenType();
    Keyword keyWord();
    char symbol();
    string identifier();
    int intVal();
    string stringVal();

    void writeOutput(ostream &output); /* write to an xml file */
};

#endif
