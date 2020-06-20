#include "derivmutator.h"
#include <iostream>

bool in_index = false;
Ref<const Move> now_move;
Ref<const Var> grad_var;

// All Expr(0) should be IntImm with value 0
Expr 
DerivMutator::visit(Ref<const IntImm> op) {
    if (in_index) return op;
    return Expr(0);
}


Expr 
DerivMutator::visit(Ref<const UIntImm> op) {
    if (in_index) return op;
    return Expr(0);
}

Expr 
DerivMutator::visit(Ref<const FloatImm> op) {
    if (in_index) return op;
    return Expr(0);
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
        grad_var = Var::make(op->type(), "d"+op->name, op->args, op->shape).as<Var>();
        return Var::make(now_move->dst.as<Var>()->type(), "d"+now_move->dst.as<Var>()->name, now_move->dst.as<Var>()->args, now_move->dst.as<Var>()->shape);
    }
    else if(op->name.compare(outMtx) == 0){
        return Var::make(grad_var->type(), grad_var->name, grad_var->args, grad_var->shape);;
        // return Var::make(op->type(), "d"+op->name, op->args, op->shape);
    }
    // else
    // {
    //     usedVar.insert(op->name);
    // }
    
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

    if(new_a.node_type() == IRNodeType::IntImm && (new_a.as<IntImm>())->value() == 0){
        aImm = true;
    }
    if(new_b.node_type() == IRNodeType::IntImm && (new_b.as<IntImm>())->value() == 0){
        bImm = true;
    }
    // if(op->a.node_type() == Boost::Internal::IRNodeType::IntImm ||
    //     op->a.node_type() == Boost::Internal::IRNodeType::UIntImm ||
    //     op->a.node_type() == Boost::Internal::IRNodeType::FloatImm ){
    //         aImm = true;
    //     }
    // if(op->b.node_type() == Boost::Internal::IRNodeType::IntImm ||
    //     op->b.node_type() == Boost::Internal::IRNodeType::UIntImm ||
    //     op->b.node_type() == Boost::Internal::IRNodeType::FloatImm){
    //         bImm = true;
    //     }

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
    now_move = op;
    Expr new_src = mutate(op->src);
    Expr new_dst = mutate(op->dst);
    return Move::make(new_dst, new_src, op->move_type);
}

Stmt
DerivMutator::visit(Ref<const LoopNest> op) {
    std::vector<Expr> new_index_list;
    std::vector<Stmt> new_body_list;
    in_index = true;
    for (auto index : op->index_list) {
        new_index_list.push_back(mutate(index));
    }
    in_index = false;
    for (auto body : op->body_list) {
        new_body_list.push_back(mutate(body));
    }
    return LoopNest::make(new_index_list, new_body_list);
}

Group 
DerivMutator::visit(Ref<const Kernel> op) {
    std::vector<Expr> new_inputs;
    for (auto expr : op->inputs) {
        new_inputs.push_back(mutate(expr));
    }
    std::vector<Expr> new_outputs;
    for (auto expr : op->outputs) {
        new_outputs.push_back(mutate(expr));
    }
    std::vector<Stmt> new_stmt_list;
    for (auto stmt : op->stmt_list) {
        new_stmt_list.push_back(mutate(stmt));
    }
    Group newknode = Kernel::make(op->name, new_inputs, new_outputs, new_stmt_list, op->kernel_type);
    MyVisitor myvis;
    newknode.visit_group(&myvis);
    usedVar.clear();

    for(auto uv : myvis.usedVar){
        usedVar.insert(uv);
    }
    std::cout << std::endl;
    return newknode;
}

void 
MyVisitor::visit(Ref<const Var> op) {
    std::cout << op->name << ", ";
    if(usedVar.find(op->name) == usedVar.end())
        usedVar.insert(op->name);

    for (auto arg : op->args) {
        arg.visit_expr(this);
    }
    return;
}

void 
MyVisitor::visit(Ref<const Kernel> op) {
    for (auto stmt : op->stmt_list) {
        stmt.visit_stmt(this);
    }
    return;
}