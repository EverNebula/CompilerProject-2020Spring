#ifndef DERIVMUTATOR_H
#define DERIVMUTATOR_H

#include "IR.h"
#include "IRMutator.h"
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

        
    std::string targetMtx;
    std::string outMtx;
    std::set<std::string> usedVar;
};
#endif