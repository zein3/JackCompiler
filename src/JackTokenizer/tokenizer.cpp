#include <JackTokenizer/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#include <boost/assign.hpp>
#include <iostream>
#include <string>
#include <regex>

using namespace std;


const vector<string> Tokenizer::KEYWORDLIST {"class", 
    "constructor", "function", "method", "field", "static",
    "var", "int", "char", "boolean", "void", "true", "false",
    "null", "this", "let", "do", "if", "else", "while", "return"
};

const string Tokenizer::SYMBOLLIST = "{}()[].,;+-*/&|<>=~";

const regex Tokenizer::IDENTIFIERREGEX ( R"(^[^0-9][0-9a-zA-Z_]*$)" );

const boost::bimap<string, Keyword> Tokenizer::KEYWORDMAPPING = boost::assign::list_of<boost::bimap<string, Keyword>::relation>
    ("class", Keyword::CLASS)
    ("constructor", Keyword::CONSTRUCTOR)
    ("function", Keyword::FUNCTION)
    ("method", Keyword::METHOD)
    ("field", Keyword::FIELD)
    ("static", Keyword::STATIC)
    ("var", Keyword::VAR)
    ("int", Keyword::INT)
    ("char", Keyword::CHAR)
    ("boolean", Keyword::BOOLEAN)
    ("void", Keyword::VOID)
    ("true", Keyword::TRUE)
    ("false", Keyword::FALSE)
    ("null", Keyword::kNULL)
    ("this", Keyword::THIS)
    ("let", Keyword::LET)
    ("do", Keyword::DO)
    ("if", Keyword::IF)
    ("else", Keyword::ELSE)
    ("while", Keyword::WHILE)
    ("return", Keyword::RETURN);

// side effects: token will be emptied
void Tokenizer::addToTokens(string &token) {
    if (token.size() > 0) {
        tokens.push_back(token);
    }

    token = "";
}

// no side effects
void Tokenizer::addToTokens(char c) {
    tokens.push_back(string {c});
}

void Tokenizer::splitIntoTokens(string &code) {
    size_t pos;
    string token = "";

    for (pos = 0; pos < code.size(); pos++) {
        char c = code[pos];

        // found symbol
        if (SYMBOLLIST.find(c) != string::npos) {
            addToTokens(token);
            addToTokens(c);
        }
        // found whitespace
        else if (isspace(c)) {
            addToTokens(token);
        }
        // special case for stringConstant
        else if (c == '"') {
            token = "";
            token.push_back('"');
            char s;
            try {
                while ((s = code[++pos]) != '"') {
                    token += s;
                }
            } catch(exception &e) {
                cout << "Error: non-terminated string \"" << endl;
                throw e;
            }

            token.push_back('"');
            addToTokens(token);
        }
        else {
            token.push_back(c);
        }
    }

    // if token is not empty, add to tokens
    addToTokens(token);
}

Tokenizer::Tokenizer(istream &input) {
    string fullCode = "";
    string line;
    regex multilineComments ( R"(\/\*.*\*\/)" );
    regex inlineComment ( R"(\/\/.*)" );

    while (getline(input, line)) {
        // strip line
        boost::algorithm::trim(line);

         // ignore line that is whitespace or comment
        if (line.empty() || line.rfind("//", 0) == 0 || line.find_first_not_of(" ") == string::npos) {
            continue;
        }

        // remove inline comment
        line = regex_replace(line, inlineComment, "");

        fullCode += line;
    }

    // remove multiline comments
    fullCode = regex_replace(fullCode, multilineComments, "");
    
    // split the code into tokens
    splitIntoTokens(fullCode);
}

bool Tokenizer::hasMoreTokens() {
    return (tokenCounter < (tokens.size() - 1));
}

void Tokenizer::advance() {
    if (hasMoreTokens()) {
        tokenCounter++;
    }
}

Token Tokenizer::tokenType() {
    string token = tokens[tokenCounter];

    if (token.empty()) {
        throw "Error: Empty token";
    }

    // check if token is a keyword
    if (find(KEYWORDLIST.begin(), KEYWORDLIST.end(), token) != KEYWORDLIST.end()) {
        return Token::KEYWORD;
    }
    // check if token is a symbol, if size != 1, doesn't need to check, it isn't a symbol
    else if (token.size() == 1 && SYMBOLLIST.find(token) != string::npos) {
        return Token::SYMBOL;
    }
    // check if token is an integer constant
    else if (find_if(token.begin(), token.end(), 
                [](unsigned char c) { return !isdigit(c); } ) == token.end()) {
        return Token::INT_CONST;
    }
    // check if token is a string constant
    else if (token[0] == '"' && token[token.size() - 1] == '"') {
        return Token::STRING_CONST;
    }
    // check if token is a valid identifier
    else if (regex_search(token, IDENTIFIERREGEX)) {
        return Token::IDENTIFIER;
    } else {
        string err = "Error: Invalid Constant " + token;
        throw err;
    }
}

Keyword Tokenizer::keyWord() {
    string token = tokens[tokenCounter];
    auto result = Tokenizer::KEYWORDMAPPING.left.find(token);
    if (result == Tokenizer::KEYWORDMAPPING.left.end()) {
        // if not found, throw an error
        throw "Error: Invalid keyword " + token;
    } else {
        return result->second;
    }
}

char Tokenizer::symbol() {
    if (tokenType() != Token::SYMBOL) {
        throw "Error: can't get symbol from non-symbol";
    }

    return tokens[tokenCounter][0];
}

string Tokenizer::identifier() {
    if (tokenType() != Token::IDENTIFIER) {
        throw "Error: can't get identifier from non-identifier";
    }

    return tokens[tokenCounter];
}

int Tokenizer::intVal() {
    if (tokenType() != Token::INT_CONST) {
        throw "Error: can't get integer constant from non-integer";
    }

    return stoi(tokens[tokenCounter]);
}

string Tokenizer::stringVal() {
    if (tokenType() != Token::STRING_CONST) {
        throw "Error: can't get string constant from non-string constant";
    }

    string token = tokens[tokenCounter];
    return token.substr(1, token.size() - 2);
}

void Tokenizer::writeOutput(ostream &output) {
    tokenCounter = 0;

    output << "<tokens>" << endl;

    while (true) {
        Token type = tokenType();

        switch(type) {
            case Token::IDENTIFIER:
                output << "<identifier> " << identifier() << " </identifier>" << endl;
                break;
            case Token::INT_CONST:
                output << "<integerConstant> " << intVal() << " </integerConstant" << endl;
                break;
            case Token::KEYWORD:
                {
                    auto result = Tokenizer::KEYWORDMAPPING.right.find(keyWord());
                    output << "<keyword> " << result->second << " </keyword>" << endl;
                    break;
                }
            case Token::SYMBOL:
                output << "<symbol> " << symbol() << " </symbol>" << endl;
                break;
            case Token::STRING_CONST:
                output << "<stringConstant> " << stringVal() << " </stringConstant>" << endl;
                break;
            default:
                output << "unknown" << endl;
        }

        if (hasMoreTokens()) {
            advance();
        } else {
            break;
        }
    }

    output << "</tokens>" << endl;
}
