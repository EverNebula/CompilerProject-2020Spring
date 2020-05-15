#ifndef PARSER_H
#define PARSER_H

#include "type.h"
#include "IR.h"
#include <sstream>
#include <map>
#include <string>
#include "IRVisitor.h"

using std::string;

using namespace Boost::Internal;

class Parser{
public :
    Parser(string _data_type) : index_type(Type::int_scalar(32)), 
        data_type(_data_type.compare("float") == 0 ? Type::float_scalar(32) : Type::int_scalar(32)){

    }
    Type index_type;
    Type data_type;

    Group kernel;

    // index map
    std::map<string, Expr> index_list;
    Expr findIdx(string id);
    void updateIdxRange(std::vector<size_t> &clst, std::vector<Expr> &alst);

    std::stringstream ssm;
    

    string parse_Id(string str);
    int parse_IntV(string str);
    float parse_FloatV(string str);

    Expr parse_IdExpr(string str);

    void parse_Const(string str);

    std::vector<Expr> parse_AList(string str);
    std::vector<size_t> parse_CList(string str);

    Expr parse_TRef(string str);


    Stmt  parse_P(string str);
};

#endif