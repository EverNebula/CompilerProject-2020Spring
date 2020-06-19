#include "derivmutator.h"
#include <iostream>


Expr 
DerivMutator::visit(Ref<const IntImm> op) {
    return Expr(0);
}


Expr 
DerivMutator::visit(Ref<const UIntImm> op) {
    return Expr(0);
}

Expr 
DerivMutator::visit(Ref<const FloatImm> op) {
    return Expr(0.0);
}

Expr 
DerivMutator::visit(Ref<const Var> op) {
    // std::vector<Expr> new_args;
    // for (auto arg : op->args) {
    //     new_args.push_back(mutate(arg));
    // }
    // return Var::make(op->type(), op->name, new_args, op->shape);

    // take derivative of itself
    if(op->name.compare(targetMtx) == 0){
        return Expr(1);
    }
    
    return Expr(0);
}

// Expr 
// DerivMutator::visit(Ref<const Unary> op) {
//     Expr new_a = mutate(op->a);
//     return Unary::make(op->type(), op->op_type, new_a);
// }

Expr 
DerivMutator::visit(Ref<const Binary> op) {
    bool aImm = false, bImm = false;
    Expr new_a = mutate(op->a);
    Expr new_b = mutate(op->b);
    if(op->a.node_type() == Boost::Internal::IRNodeType::IntImm ||
        op->a.node_type() == Boost::Internal::IRNodeType::UIntImm ||
        op->a.node_type() == Boost::Internal::IRNodeType::FloatImm){
            aImm = true;
        }
    if(op->b.node_type() == Boost::Internal::IRNodeType::IntImm ||
        op->b.node_type() == Boost::Internal::IRNodeType::UIntImm ||
        op->b.node_type() == Boost::Internal::IRNodeType::FloatImm){
            bImm = true;
        }

    switch(op->op_type){
        case Boost::Internal::BinaryOpType::Add : 
        case Boost::Internal::BinaryOpType::Sub :
            if(aImm && bImm)
                return Expr(0);
            if(aImm)
                return new_b;
            if(bImm)
                return new_a;
            return Binary::make(op->type(), op->op_type, new_a, new_b);
            break;

        case Boost::Internal::BinaryOpType::Mul:
            if(aImm && bImm)
                return Expr(0);
            if(aImm)
                return Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, op->a, new_b);
            if(bImm)
                return Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, new_a, op->b);
            return Binary::make(op->type(), Boost::Internal::BinaryOpType::Add, 
                        Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, new_a, op->b),
                        Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, op->a, new_b));
            break;
        case Boost::Internal::BinaryOpType::Div:{
            if(aImm && bImm)
                return Expr(0);
            Expr numerator = Binary::make(op->type(), Boost::Internal::BinaryOpType::Sub, 
                                Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, new_a, op->b),
                                Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, op->a, new_b));
            Expr denominator = Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, op->b, op->b);

            if(bImm)
                return Binary::make(op->type(), Boost::Internal::BinaryOpType::Div, 
                                Binary::make(op->type(), Boost::Internal::BinaryOpType::Mul, new_a, op->b), 
                                denominator);
            return Binary::make(op->type(), Boost::Internal::BinaryOpType::Div, numerator, denominator);
            break;
        }
        default:
            std::cout << "DerivMutator Binary op type error!" << std::endl;
            return Expr(0);
    }
    // return Binary::make(op->type(), op->op_type, new_a, new_b);
}

Stmt 
DerivMutator::visit(Ref<const Move> op) {
    // Expr new_dst = mutate(op->dst);
    Expr new_src = mutate(op->src);
    return Move::make(op->dst, new_src, op->move_type);
}