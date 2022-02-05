#ifndef _VAR_HPP_
#define _VAR_HPP_

#include <string>

using namespace std;


enum class Kind {
    STATIC, FIELD, ARG, VAR
};

typedef struct var_struct {
    string type;
    Kind kind;
    size_t index;
} Var;

#endif
