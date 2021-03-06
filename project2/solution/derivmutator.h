#ifndef DERIVMUTATOR_H
#define DERIVMUTATOR_H

#include "IR.h"
#include "IRMutator.h"
#include "IRVisitor.h"
#include <set>
#include <map>
#include <stack>

using namespace Boost::Internal;
// function mutate & visit takes the derivative of expr
class DerivMutator : public IRMutator{
    public:
    DerivMutator(std::string _targetMtx, std::string _outMtx)
    : IRMutator(), targetMtx(_targetMtx), outMtx(_outMtx), curTargetVar(),
    allTargetVars({}) ,firstTargetVar(true) {};
    Expr visit(Ref<const IntImm>) override;
    Expr visit(Ref<const UIntImm>) override;
    Expr visit(Ref<const FloatImm>) override;
    Expr visit(Ref<const Var>) override;
    Expr visit(Ref<const Binary>) override; 
    Stmt visit(Ref<const Move>) override;
    Stmt visit(Ref<const LoopNest>) override;
    Group visit(Ref<const Kernel>) override;
    
    bool exprcompare(Expr a, Expr b);
    bool varcompare(Ref<const Var> a, Ref<const Var> b);
    bool binarycompare(Ref<const Binary> a, Ref<const Binary> b);
    bool strimmcompare(Ref<const StringImm> a, Ref<const StringImm> b);
    bool intimmcompare(Ref<const IntImm> a, Ref<const IntImm> b);
    bool indexcompare(Ref<const Index> a, Ref<const Index> b);

    Ref<const Var> curTargetVar;      // deal with situation : B[i], B[i+1], B[i+2]
    std::vector<Ref<const Var>> allTargetVars;
    bool firstTargetVar;

    std::string targetMtx;
    std::string outMtx;
    std::set<std::string> usedVar;
};

// alter index in lhs
class IndexMutator : public IRMutator{
    public:
    IndexMutator(std::string _targetMtx, std::string _outMtx)
    : IRMutator(), targetMtx(_targetMtx), outMtx(_outMtx) {};
    Expr visit(Ref<const IntImm>) override;
    Expr visit(Ref<const UIntImm>) override;
    Expr visit(Ref<const FloatImm>) override;
    Expr visit(Ref<const Var>) override;
    Expr visit(Ref<const Binary>) override; 
    Stmt visit(Ref<const Move>) override;
    Stmt visit(Ref<const LoopNest>) override;
    Expr visit(Ref<const Dom>) override;
    Expr visit(Ref<const Index>) override;

    std::string targetMtx;
    std::string outMtx;
    std::set<std::string> victimVar;
    std::map<std::string, Ref<const Index>> replaceVar;
    std::map<std::string, Expr> replaceExpr;
    std::map<std::string, Ref<const Index>> originVar;
};

//MyVisitor collects used Var nodes to imply input arguments
class MyVisitor : public IRVisitor{
    public:
        MyVisitor() : IRVisitor(), usedVar({}){};
        std::set<std::string> usedVar;
        void visit(Ref<const Var> op) override;
        void visit(Ref<const Kernel> op) override;
};

class StackItem{
public:
    Ref<const Binary> old_op;
    Expr sub;
    int pos;
    StackItem(Ref<const Binary> old_op, Expr sub, int pos)
        :old_op(old_op), sub(sub), pos(pos) {};
};

#endif