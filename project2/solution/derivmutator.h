#ifndef DERIVMUTATOR_H
#define DERIVMUTATOR_H

#include "IR.h"
#include "IRMutator.h"
#include "IRVisitor.h"
#include <set>

using namespace Boost::Internal;
// function mutate & visit takes the derivative of expr
class DerivMutator : public IRMutator{
    public:
    DerivMutator(std::string _targetMtx, std::string _outMtx)
    : IRMutator(), targetMtx(_targetMtx), outMtx(_outMtx) {};
    Expr visit(Ref<const IntImm>) override;
    Expr visit(Ref<const UIntImm>) override;
    Expr visit(Ref<const FloatImm>) override;
    Expr visit(Ref<const Var>) override;
    Expr visit(Ref<const Binary>) override; 
    Stmt visit(Ref<const Move>) override;
    Stmt visit(Ref<const LoopNest>) override;
    Group visit(Ref<const Kernel>) override;
        
    std::string targetMtx;
    std::string outMtx;
    std::set<std::string> usedVar;
};

//MyVisitor collects used Var nodes to imply input arguments
class MyVisitor : public IRVisitor{
    public:
        MyVisitor() : IRVisitor(), usedVar({}){};
        std::set<std::string> usedVar;
        void visit(Ref<const Var> op) override;
        void visit(Ref<const Kernel> op) override;
};

#endif