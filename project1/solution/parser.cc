#include "parser.h"
#include <climits>
#include <memory>
#include <string>
#include <sys/_types/_size_t.h>
#include <utility>


const int MIN_BOUND = -1e6;
const int MAX_BOUND = 1e6;
const int DEF_EXT = MAX_BOUND - MIN_BOUND;

Expr
Parser:: findIdx(string id){
    for(auto val : index_list){
        if( ( (val.second.as<Index>())->name ).compare(id) == 0){
            return val.second;
        }
    }
    return Expr();
}

void
Parser:: updateIdxRange(std::vector<size_t> &clst, std::vector<Expr> &alst){
    if(clst.size() != alst.size()){
        std::cout << "Clist.size != Alist.size" << std::endl;
        return;
    }
    int i = 0;

    for(auto aexpr : alst){
        size_t sz = clst.at(i ++);

            // std::cout << "sz : " << sz << std::endl;

        int st = 0, ed = sz, ext = sz;

        if( aexpr.node_type() == IRNodeType::Index ){       //i,j,k
            std::shared_ptr<const Dom> domp =  ((aexpr.as<Index>())->dom).as<Dom>();
            int beginv = 0, extendv = 0, edv;
            beginv = ((domp->begin).as<IntImm>())->value();
            extendv = ((domp->extent).as<IntImm>())->value();
            edv = beginv + extendv;

            // std::cout << st << ext << std::endl;

            if(st < beginv)
                st = beginv;
            if(ed > edv)
                ed = edv;
            ext = ed - st;

            // std::cout << st << ext << std::endl;
            
            // save changed Index to vector changed
            if(st != beginv || ext != extendv){

                Expr newdom = Dom::make(index_type, st, ext);
                Expr newIdx = Index::make(index_type, aexpr.as<Index>()->name, newdom, aexpr.as<Index>()->index_type);

                index_list.erase(index_list.find(aexpr.as<Index>()->name));
                index_list.insert(std::make_pair(aexpr.as<Index>()->name, newIdx));
            }

        }
        else{       // i + 2, j + 3
            // TO DO
        }

        //modify vector alist
        {
            //TO DO
        }
        
    }
}


string
Parser:: parse_Id(string str){
    return str;
}

int
Parser:: parse_IntV(string str){
    int retVal;
    ssm.clear();
    ssm <<  str;
    ssm >> retVal;


    return retVal;
}

float
Parser:: parse_FloatV(string str){
    float retVal;
    ssm.clear();
    ssm <<  str;
    ssm >> retVal;
    return retVal;
}

Expr 
Parser::parse_IdExpr(string str)
{
    // dprintf("IdExpr: %s\n", str.c_str());

    int idx = 0, las = 0, len = str.length();
    int bkt = 0;

    // + 
    for (idx = 0; idx < len; ++idx)
    {
        if (str[idx] == '(')
            bkt++;
        else if (str[idx] == ')')
            bkt--;
        else if (bkt == 0)
        {
            if (str[idx] == '+')
                break;
        }
    }

    if (idx != len)
    {
        Expr exprL = std::move(parse_IdExpr(str.substr(0, idx)));
        
        if (str[idx+1] < '0' || str[idx+1] > '9')
        {
            // IdExpr -> IdExpr + IdExpr
            Expr exprR = std::move(parse_IdExpr(str.substr(idx+1)));
            return Binary::make(index_type, BinaryOpType::Add, exprL, exprR);
        }
        else
        {
            // IdExpr -> IdExpr + IntV
            Expr exprR = std::move(parse_IntV(str.substr(idx+1)));
            return Binary::make(index_type, BinaryOpType::Add, exprL, exprR);
        }
        
    }

    // *   /   %   //($)
    for (idx = 0; idx < len; ++idx)
    {
        if (str[idx] == '(')
            bkt++;
        else if (str[idx] == ')')
            bkt--;
        else if (bkt == 0)
        {
            if (str[idx] == '*' || str[idx] == '/' 
                || str[idx] == '%' || str[idx] == '$')
                break;
        }
    }

    if (idx != len)
    {
        Expr exprL = std::move( parse_IdExpr(str.substr(0, idx)) ), exprR = parse_IntV(str.substr(idx+1));


        BinaryOpType optp;
        switch (str.c_str()[idx]){
            case '*':
                optp = BinaryOpType::Mul;
                break;
            case '/':
                optp = BinaryOpType::Div;
                break;
            case '%':
                optp = BinaryOpType::Mod;
                break;
            case '$':
                optp = BinaryOpType::Div;
                break;
        }
        return Binary::make(index_type, optp, exprL, exprR);
    }


    // bracket
    if (str[0] == '(' && str[len-1] == ')')
    {
        return parse_IdExpr(str.substr(1, len-2));
    }

    // Id
    Expr exprIdx = findIdx( parse_Id(str));
    if(exprIdx.defined())
        return exprIdx;
    else{
        Expr newdom = Dom::make(index_type, MIN_BOUND, DEF_EXT);
        Expr newIdx = Index::make(index_type, parse_Id(str), newdom, IndexType::Spatial);
        index_list.insert(std::make_pair(parse_Id(str), newIdx));
        return newIdx;
    }
    return exprIdx;
}

void 
Parser::parse_Const(string str)
{
    // dprintf("Const: %s\n", str.c_str());

    int idx = 0;

    if((idx = str.find('.', idx)) != string::npos)
    {
        parse_FloatV(str);
    }
    else
    {
        parse_IntV(str);
    }
}

std::vector<Expr>
Parser:: parse_AList(string str)
{
    std::vector<Expr> retVec;

    // dprintf("AList: %s\n", str.c_str());
    int idx = 0, las = 0;

    while((idx = str.find(',', idx)) != string::npos)
    {
        retVec.push_back( parse_IdExpr(str.substr(las, idx-las)) );
        las = ++idx;
    }
    retVec.push_back( parse_IdExpr(str.substr(las)) );
    return retVec;
}

std::vector<size_t>
Parser:: parse_CList(string str)
{
    // dprintf("CList: %s\n", str.c_str());
    int idx = 0, las = 0;
    std::vector<size_t> retVec;

    while((idx = str.find(',', idx)) != string::npos)
    {
        retVec.push_back( parse_IntV(str.substr(las, idx-las)) );
        las = ++idx;
    }
    retVec.push_back( parse_IntV(str.substr(las)) );

    return retVec;
}

Expr
Parser:: parse_TRef(string str)
{
    // dprintf("TRef: %s\n", str.c_str());

    int idx = 0, las = 0;
    // id part
    if((idx = str.find('<', idx)) != string::npos)
    {
        string name = parse_Id(str.substr(0, idx));
        las = idx + 1;

        if((idx = str.find('[', idx)) != string::npos)
        {
            std::vector<size_t> clst = parse_CList(str.substr(las, idx-las-1));
            std::vector<Expr> alst = parse_AList(str.substr(idx+1, str.length()-idx-2));

            updateIdxRange(clst, alst);
            return Var::make(data_type, name, alst, clst);
        }
        else
        {
            std::cout << "invalid statement. (without [)" << std::endl;
        }
    }
    else
    {
        std::cout << "invalid statement. (without <)" << std::endl;
    }

}