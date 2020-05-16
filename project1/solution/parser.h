#ifndef PARSER_H
#define PARSER_H

#include "type.h"
#include "IR.h"
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "IRVisitor.h"
#include "IRPrinter.h"

using std::string;

using namespace Boost::Internal;

class Parser{
public :
    explicit Parser() = delete;
    explicit Parser(Parser &psr) = delete;

    Parser(string _name, std::vector<string> _insvec, std::vector<string> _outvec, string _data_type, string _kernel) : 
    index_type(Type::int_scalar(32)), name(_name), insvec(_insvec), outvec(_outvec),
    data_type(_data_type.compare("float") == 0 ? Type::float_scalar(32) : Type::int_scalar(32)),
    kernel(_kernel){
        ;
    }
    Type index_type;

    string name;
    std::vector<string> insvec;
    std::vector<string> outvec;
    Type data_type;
    string kernel;


    // index map
    std::map<string, Expr> index_list;
    std::set<string> index_inlhs;
    std::set<string> index_inrhs;

    Expr findIdx(string id);
    void updateIdxRange(std::vector<size_t> &clst, std::vector<Expr> &alst);

    std::stringstream ssm;
    

    string parse_Id(string str);
    int parse_IntV(string str);
    float parse_FloatV(string str);

    Expr parse_IdExpr(string str);

    Expr parse_Const(string str);

    std::vector<Expr> parse_AList(string str);
    std::vector<size_t> parse_CList(string str);

    Expr parse_TRef(string str);
    Expr parse_LHS(string str);

    Expr parse_SRef(string str);
    Expr parse_RHS(string str);

    std::vector<Stmt> parse_S(string str, std::vector<Expr> &vars);
    std::vector<Stmt> parse_P(string str);

    void build_Kernel();

};

#endif