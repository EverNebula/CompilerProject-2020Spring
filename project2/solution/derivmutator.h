#ifndef DERIVMUTATOR_H
#define DERIVMUTATOR_H

#include "IR.h"
#include "IRMutator.h"

using namespace Boost::Internal;
// function mutate & visit takes the derivative of expr
class DerivMutator : public IRMutator{
    public:
    DerivMutator(std::string _targetMtx) : IRMutator(), targetMtx(_targetMtx) {};
    Expr visit(Ref<const IntImm>) override;
    Expr visit(Ref<const UIntImm>) override;
    Expr visit(Ref<const FloatImm>) override;
    Expr visit(Ref<const Var>) override;
    Expr visit(Ref<const Binary>) override; 
    Stmt visit(Ref<const Move>) override;

        
    std::string targetMtx;
};
#endif