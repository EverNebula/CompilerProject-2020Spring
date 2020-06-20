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
        if(!curTargetVar.defined())
            curTargetVar = op;
        if(varcompare(curTargetVar, op) != 0){
            if(firstTargetVar)
                this->allTargetVars.push_back(op);
            return Expr(0);
        }
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
        firstTargetVar = true;
        new_body_list.push_back(mutate(body));
        firstTargetVar = false;
        while(!allTargetVars.empty()){
            curTargetVar = allTargetVars[allTargetVars.size() - 1];
            allTargetVars.pop_back();
            new_body_list.push_back(mutate(body));
        }
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


bool 
DerivMutator:: exprcompare(Expr a, Expr b){
    if(a.node_type() == IRNodeType::Var && b.node_type() == IRNodeType::Var){
        return varcompare(a.as<Var>(), b.as<Var>());
    }
    if(a.node_type() == IRNodeType::Binary && b.node_type() == IRNodeType::Binary){
        return binarycompare(a.as<Binary>(), b.as<Binary>());
    }
    if(a.node_type() == IRNodeType::IntImm && b.node_type() == IRNodeType::IntImm){
        return intimmcompare(a.as<IntImm>(), b.as<IntImm>());
    }
    if(a.node_type() == IRNodeType::StringImm && b.node_type() == IRNodeType::StringImm){
        return strimmcompare(a.as<StringImm>(), b.as<StringImm>());
    }
    if(a.node_type() == IRNodeType::Index && b.node_type() == IRNodeType::Index){
        return indexcompare(a.as<Index>(), b.as<Index>());
    }

    std::cout << "!!!exprcompare node type not supported!!!" << std::endl;
    return true; //true or false by default?
}


bool
DerivMutator:: varcompare(Ref<const Var> a, Ref<const Var> b){
    bool cmpresult = a->name == b->name ? 0 : 1;
    cmpresult |= a->args.size() == b->args.size() ? 0 : 1;
    if(cmpresult != 0)  //not same
        return cmpresult;
    for(int i = 0; i < a->args.size(); i++){
        cmpresult |= exprcompare(a->args[i], b->args[i]);
    }
    return cmpresult;
}

bool 
DerivMutator::binarycompare(Ref<const Binary> a, Ref<const Binary> b){
    bool cmpresult = a->op_type == b->op_type ? 0 : 1;
    if(cmpresult !=0)
        return cmpresult;
    cmpresult |= exprcompare(a->a, b->a);
    cmpresult |= exprcompare(a->b, b->b);
    return cmpresult;
}

bool 
DerivMutator::strimmcompare(Ref<const StringImm> a, Ref<const StringImm> b){
    bool cmpresult = (a->value()).compare(b->value()) == 0 ? 0 : 1;
    return cmpresult;
}

bool 
DerivMutator::intimmcompare(Ref<const IntImm> a, Ref<const IntImm> b){
    bool cmpresult = a->value() == b->value() ? 0 : 1;
    return cmpresult;
}

bool 
DerivMutator::indexcompare(Ref<const Index> a, Ref<const Index> b){
    bool cmpresult = (a->name).compare(b->name) == 0 ? 0 : 1;
    return cmpresult;
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