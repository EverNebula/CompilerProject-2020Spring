#include "parser.h"
#include <climits>
#include <memory>
#include <ostream>
#include <string>
#include <sys/_types/_size_t.h>
#include <utility>
#include <vector>


const int MIN_BOUND = -1e6;
const int MAX_BOUND = 1e6;
const int DEF_EXT = MAX_BOUND - MIN_BOUND;

bool now_inlhs = false;

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
        std::cout << "Clist.size != Alist.size before updateIdxRange" << std::endl;
        return;
    }
    std::vector<Expr> newalst = {};
    int i = 0;

    for(auto aexpr : alst){
        size_t sz = clst.at(i ++);

            // std::cout << "sz : " << sz << std::endl;

        int st = 0, ed = sz, ext = sz;
        bool change_flag = false;

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
            
            // save changed Index to vector newalst & index_list
            if(st != beginv || ext != extendv){

                Expr newdom = Dom::make(index_type, st, ext);
                Expr newIdx = Index::make(index_type, aexpr.as<Index>()->name, newdom, aexpr.as<Index>()->index_type);

                index_list.erase(index_list.find(aexpr.as<Index>()->name));
                index_list.insert(std::make_pair(aexpr.as<Index>()->name, newIdx));
                
                newalst.push_back(newIdx);
                change_flag = true;
            }

        }
        else if(aexpr.node_type() == IRNodeType :: Binary){       // expr op expr
            std::shared_ptr<const Binary> bnp = aexpr.as<Binary>();

            if(bnp->op_type  == BinaryOpType :: Add){       // expr + expr
                if(bnp->a.node_type() == IRNodeType::Index && bnp->b.node_type() == IRNodeType::IntImm){
                    // i + 1, j + 2 etc.
                    int immval = (bnp->b).as<IntImm>()->value();

                    std::shared_ptr<const Dom> domp =  ((bnp->a.as<Index>())->dom).as<Dom>();
                    int beginv = 0, extendv = 0, edv;
                    beginv = ((domp->begin).as<IntImm>())->value();
                    extendv = ((domp->extent).as<IntImm>())->value();
                    edv = beginv + extendv;

                    if(st - immval < beginv)
                        st = beginv;
                    else 
                        st = st - immval;

                    if(ed - immval > edv)
                        ed = edv;
                    else 
                        ed = ed - immval;

                    ext = ed - st;

                    // save changed Index to vector newalst & index_list
                    if(st != beginv || ed != edv){
                        Expr newdom = Dom::make(index_type, st, ext);
                        Expr newIdx = Index::make(index_type, bnp->a.as<Index>()->name, newdom, bnp->a.as<Index>()->index_type);


                        index_list.erase(index_list.find(bnp->a.as<Index>()->name));
                        index_list.insert(std::make_pair(bnp->a.as<Index>()->name, newIdx));

                        newalst.push_back(Binary::make(index_type, bnp->op_type, newIdx, bnp->b));
                        change_flag = true;
                    }
                }
                else if (bnp->a.node_type() == IRNodeType::Index && bnp->b.node_type() == IRNodeType::Index){
                    //i + j, q + s etc.
                    // FIXME:
                }
            }
        }

        if(change_flag == false)
            newalst.push_back(aexpr);
        
    }

    //modify vector alist
    alst.clear();
    for(auto v : newalst){
        alst.push_back(v);
    }

    if(clst.size() != alst.size()){
        std::cout << "Clist.size != Alist.size after updateIdxRange" << std::endl;
        return;
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
            Expr exprR = Expr(parse_IntV(str.substr(idx+1)));
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

    if (now_inlhs)
        index_inlhs.insert(parse_Id(str));
    else
        index_inrhs.insert(parse_Id(str));

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

Expr 
Parser::parse_Const(string str)
{
    // dprintf("Const: %s\n", str.c_str());

    int idx = 0;

    if((idx = str.find('.', idx)) != string::npos)
    {
        return parse_FloatV(str);
    }
    else
    {
        return parse_IntV(str);
    }
    return Expr();
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
    return Expr();

}

Expr
Parser:: parse_LHS(string str){
    // dprintf("LHS: %s\n", str.c_str());
    return parse_TRef(str);
}


Expr
Parser:: parse_SRef(string str)
{
    // dprintf("SRef: %s\n", str.c_str());

    int idx = 0;
    // id part
    if((idx = str.find('<', idx)) != string::npos)
    {
        std::vector<Expr> args = {};
        return Var::make(data_type, parse_Id(str.substr(0, idx)), args, 
                        parse_CList(str.substr(idx+1, str.length()-idx-2)));
    }
    else
    {
        std::cout << "invalid statement. (without <)" << std::endl;
    }
    return Expr();
}


Expr
Parser:: parse_RHS(string str)
{
    // dprintf("RHS: %s\n", str.c_str());
    // std::cout << "!! : " << str << std::endl;


    int idx = 0, las = 0, len = str.length();
    int bkt = 0;

    // +   -
    for (idx = 0; idx < len; ++idx)
    {
        if (str[idx] == '(' || str[idx] == '[')
            bkt++;
        else if (str[idx] == ')' || str[idx] == ']')
            bkt--;
        else if (bkt == 0)
        {
            if (str[idx] == '+' || str[idx] == '-')
                break;
        }
    }

    if (idx != len)
    {
        Expr exprR = parse_RHS(str.substr(idx+1));
        Expr exprL = parse_RHS(str.substr(0, idx));
        BinaryOpType optp = str[idx] == '+' ? BinaryOpType::Add : BinaryOpType::Sub;
        return Binary::make(data_type, optp, exprL, exprR);
    }

    // *   /   %   //($)
    for (idx = 0; idx < len; ++idx)
    {
        if (str[idx] == '(' || str[idx] == '[')
            bkt++;
        else if (str[idx] == ')' || str[idx] == ']')
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
        Expr exprR = parse_RHS(str.substr(idx+1));
        Expr exprL = parse_RHS(str.substr(0, idx));
        BinaryOpType optp = BinaryOpType::Mul;
        switch (str[idx]){
            case '/':
                optp = BinaryOpType::Div;
                break;
            case '%':
                optp = BinaryOpType::Mod;
                break;
            case '$':
                optp = BinaryOpType::Div;
                break;
            default:;
        }
        return Binary::make(data_type, optp, exprL, exprR);
    }


    // bracket
    if (str[0] == '(' && str[len-1] == ')')
    {
        return parse_RHS(str.substr(1, len-2));
    }


    // TRef
    if ((idx = str.find('[', 0)) != string::npos)
    {
    
        return parse_TRef(str);
    }

    // SRef
    if ((idx = str.find('<', 0)) != string::npos)
    {
        return parse_SRef(str);
    }

    // Const
    return parse_Const(str);
}

std::vector<Stmt>
Parser:: parse_S(string str, std::vector<Expr> &vars)
{
    std::vector<Stmt> retVec;
    std::vector<Expr> idx_lst;
    // dprintf("S: %s\n", str.c_str());
    int idx = 0, las = 0;
    int len = str.length();
    char lasop = '+';

    if((idx = str.find('=', idx)) != string::npos)
    {
        now_inlhs = true;
        Expr lhs = parse_LHS(str.substr(0, idx));
        now_inlhs = false;

        // vars[0] -> lhs ;  vars[1] -> temp
        vars.push_back(lhs);
        vars.push_back(Var::make(data_type, "temp", (lhs.as<Var>())->args, (lhs.as<Var>())->shape));

        bool flag = true;
        do
        {
            int bkt = 0;

            las = ++idx;
            // +   -
            for ( ; idx < len; ++idx)
            {
                if (str[idx] == '(' || str[idx] == '[')
                    bkt++;
                else if (str[idx] == ')' || str[idx] == ']')
                    bkt--;
                else if (bkt == 0)
                {
                    if (str[idx] == '+' || str[idx] == '-')
                        break;
                }
            }

            Expr expr = parse_RHS(str.substr(las, idx-las));
            // Expr exprL = parse_RHS(str.substr(0, idx));
            BinaryOpType optp = lasop == '+' ? BinaryOpType::Add : BinaryOpType::Sub;
            Expr binary = Binary::make(data_type, optp, vars[1], expr);
            Stmt mov = Move::make(vars[1], binary, MoveType::MemToMem);

            idx_lst.clear();
            for(auto val : index_inrhs)
                if (index_inlhs.count(val) == 0) // not in LHS
                {
                    idx_lst.push_back( index_list[val] );
                }
            
            if (idx_lst.size() > 0)
            {
                retVec.push_back( LoopNest::make(idx_lst, {mov}) );
            }
            else // like 'temp[i][j] = A[i][j]', loopnest is needless.
            {
                retVec.push_back( mov );
            }

            if (idx == len)
            {
                flag = false;
            }
            else
            {
                lasop = str[idx];
            }

        }while(flag);

        return retVec;
    }
    else
    {
        std::cout << "invalid statement. (without =)" << std::endl;
    }
}

std::vector<Stmt>
Parser:: parse_P(string str)
{
    std::vector<Stmt> res;
    std::vector<Expr> idx_lst, vars;
 
    int idx = 0, las = 0;

    // erase all space
    while((idx = str.find(' ', idx)) != string::npos)
    {
        str.erase(idx,1);
    }
    idx = 0;
    while((idx = str.find("//", idx)) != string::npos)
    {
        str.replace(idx, 2, "$");
    }
    // dprintf("P: %s\n", str.c_str());

    idx = 0;
    while((idx = str.find(';', idx)) != string::npos)
    {
        // new statement
        index_inlhs.clear();
        vars.clear();

        std::vector<Stmt> S = parse_S(str.substr(las, idx-las), vars);
        las = ++idx;

        idx_lst.clear();
        for(auto val : index_inlhs)
        {
            idx_lst.push_back( index_list[val] );
        }

        // temp initialization
        Stmt mov1 = Move::make(vars[1], 0, MoveType::MemToMem);
        res.push_back( LoopNest::make(idx_lst, {mov1}) );
        res.push_back( LoopNest::make(idx_lst, S) );
        Stmt mov2 = Move::make(vars[0], vars[1], MoveType::MemToMem);
        res.push_back( LoopNest::make(idx_lst, {mov2}) );
    }
    return res;
}


void 
Parser:: build_Kernel(){
    std::vector<Stmt> stmts = parse_P(kernel);
    // std::vector<Expr> idx_lst = {};
    // for(auto val : index_list)
    //     idx_lst.push_back(val.second);
    
    
    // Stmt loop_nest = LoopNest::make(idx_lst, stmts);
    std::vector<Expr> blank = {};
    Group knoderef = Kernel::make(name, blank, blank, stmts, KernelType::CPU);

    IRPrinter printer;
    std::string code = printer.print(knoderef);
    std::cout << code << std::endl;

}
