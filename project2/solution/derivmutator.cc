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
        {
            this->allTargetVars.push_back(op);
            curTargetVar = op;
        }
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
        if (body.node_type() == IRNodeType::Move) // split the move stmt
        {
            firstTargetVar = true;
            mutate(body);
            firstTargetVar = false;
            while(!allTargetVars.empty()){
                curTargetVar = allTargetVars[allTargetVars.size() - 1];
                allTargetVars.pop_back();
                new_body_list.push_back(mutate(body));
            }
        }
        else
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


/*
Begin of IndexMutator
*/
bool in_lhs = false;
bool in_arg = false;
bool find_vic = false; // find a victim index
std::string vic_name = "";
bool no_op = false; // no calculation in index
bool has_mod = false; // mod calc exists
Ref<const Index> now_idx;
std::stack<StackItem> op_stack;
std::vector<std::string> cst_idx; // store constraints of old index
std::vector<Ref<const Index>> extendVar;

Expr 
IndexMutator::visit(Ref<const IntImm> op) {
    return op;
}

Expr 
IndexMutator::visit(Ref<const UIntImm> op) {
    return op;
}

Expr 
IndexMutator::visit(Ref<const FloatImm> op) {
    return op;
}

Expr
IndexMutator::visit(Ref<const Dom> op) {
    Expr new_begin = mutate(op->begin);
    Expr new_extent = mutate(op->extent);
    return Dom::make(op->type(), new_begin, new_extent);
}


Expr
IndexMutator::visit(Ref<const Index> op) {
    if (!in_lhs)
    {
        if (victimVar.count(op->name) != 0)
        {
            // replace it!
            return replaceExpr[op->name];
        }
        return op;
    }

    if (no_op && victimVar.count(op->name) == 0)
        return op;
    if (in_arg && !find_vic)
    {
        std::cout << "idx: " << op->name << std::endl;
        find_vic = true;
        vic_name = op->name;
        cst_idx.push_back(op->name);
    }
    Expr new_dom = mutate(op->dom);
    return Index::make(op->type(), op->name, new_dom, op->index_type);
}

Expr 
IndexMutator::visit(Ref<const Var> op) {
    // std::vector<Expr> new_args;
    // for (auto arg : op->args) {
    //     new_args.push_back(mutate(arg));
    // }
    // return Var::make(op->type(), op->name, new_args, op->shape);

    // choose victim var in every dim
    if (in_lhs)
    {
        std::cout << op->name << std::endl;
        in_arg = true;
        std::vector<Expr> new_args;

        int dim_num = 0;
        for (auto arg : op->args)
        {
            find_vic = false;
            no_op = true;
            
            std::cout << op->shape[dim_num] << std::endl;
            Expr new_arg = mutate(arg);

            // mod in index
            if (find_vic && has_mod && victimVar.count(vic_name) != 0)
            {
                has_mod = false;

                victimVar.insert(vic_name);
                Expr innerExpr = Index::make(Type::int_scalar(32), op->name+std::to_string(dim_num),
                    Dom::make(Type::int_scalar(32), 0, Expr((int)op->shape[dim_num])), IndexType::Spatial);
                extendVar.push_back(replaceVar[vic_name]);
                replaceVar[vic_name] = innerExpr.as<Index>();
                new_args.push_back(replaceVar[vic_name]);

                while (!op_stack.empty())
                {
                    StackItem u = op_stack.top();

                    switch (u.old_op->op_type)
                    {
                        case BinaryOpType::Add:
                            innerExpr =
                                Binary::make(u.old_op->type(), BinaryOpType::Sub, innerExpr, u.sub);
                            break;
                        case BinaryOpType::Sub:
                            if (u.pos == 2)
                            {
                                innerExpr =
                                    Binary::make(u.old_op->type(), BinaryOpType::Sub, u.sub, innerExpr);
                            }
                            else
                            {
                                innerExpr =
                                    Binary::make(u.old_op->type(), BinaryOpType::Add, innerExpr, u.sub);    
                            }
                            break;
                        case BinaryOpType::Mul:
                            innerExpr =
                                Binary::make(u.old_op->type(), BinaryOpType::Div, innerExpr, u.sub);
                            break;
                        case BinaryOpType::Div:
                            if (u.pos == 2)
                            {
                                innerExpr =
                                    Binary::make(u.old_op->type(), BinaryOpType::Div, u.sub, innerExpr);
                            }
                            else
                            {
                                innerExpr =
                                    Binary::make(u.old_op->type(), BinaryOpType::Mul, innerExpr, u.sub);    
                            }
                            break;
                        case BinaryOpType::Mod:
                            // no action
                            break;
                        default:
                            ;
                    }

                    op_stack.pop();
                }

                replaceExpr[vic_name] = Binary::make(Type::int_scalar(32), BinaryOpType::Add,
                    replaceExpr[vic_name], innerExpr);
            }
            else if (find_vic) // find a victim var, regist it
            {
                victimVar.insert(vic_name);
                replaceExpr[vic_name] = Index::make(Type::int_scalar(32), op->name+std::to_string(dim_num),
                    Dom::make(Type::int_scalar(32), 0, Expr((int)op->shape[dim_num])), IndexType::Spatial);
                replaceVar[vic_name] = replaceExpr[vic_name].as<Index>();
                new_args.push_back(replaceVar[vic_name]);

                while (!op_stack.empty())
                {
                    StackItem u = op_stack.top();

                    switch (u.old_op->op_type)
                    {
                        case BinaryOpType::Add:
                            replaceExpr[vic_name] =
                                Binary::make(u.old_op->type(), BinaryOpType::Sub, replaceExpr[vic_name], u.sub);
                            break;
                        case BinaryOpType::Sub:
                            if (u.pos == 2)
                            {
                                replaceExpr[vic_name] =
                                    Binary::make(u.old_op->type(), BinaryOpType::Sub, u.sub, replaceExpr[vic_name]);
                            }
                            else
                            {
                                replaceExpr[vic_name] =
                                    Binary::make(u.old_op->type(), BinaryOpType::Add, replaceExpr[vic_name], u.sub);    
                            }
                            break;
                        case BinaryOpType::Mul:
                            replaceExpr[vic_name] =
                                Binary::make(u.old_op->type(), BinaryOpType::Div, replaceExpr[vic_name], u.sub);
                            break;
                        case BinaryOpType::Div:
                            if (u.pos == 2)
                            {
                                replaceExpr[vic_name] =
                                    Binary::make(u.old_op->type(), BinaryOpType::Div, u.sub, replaceExpr[vic_name]);
                            }
                            else
                            {
                                replaceExpr[vic_name] =
                                    Binary::make(u.old_op->type(), BinaryOpType::Mul, replaceExpr[vic_name], u.sub);    
                            }
                            break;
                        case BinaryOpType::Mod:
                            // no action
                            break;
                        default:
                            ;
                    }

                    op_stack.pop();
                }
            }
            else
                new_args.push_back(new_arg);

            dim_num++;
        }
        in_arg = false;
        return Var::make(op->type(), op->name, new_args, op->shape);
    }

    else
    {
        std::vector<Expr> new_args;
        for (auto arg : op->args) {
            new_args.push_back(mutate(arg));
        }
        return Var::make(op->type(), op->name, new_args, op->shape);
    }
    
    return Expr(0);
}

Expr 
IndexMutator::visit(Ref<const Binary> op) {

    no_op = false;

    bool aImm = false, bImm = false, vic_found = find_vic;
    int main_pos = 0; // illustrate where main branch is.  0-null  1-left  2-right


    // right first
    StackItem new_b = StackItem(op, mutate(op->b), 1);
    if (find_vic) main_pos = 2;
    StackItem new_a = StackItem(op, mutate(op->a), 2);
    if (find_vic) main_pos = std::max(main_pos, 1);
    if (vic_found) main_pos = 0;

    if (!in_arg)
        return Binary::make(op->type(), op->op_type, new_a.sub, new_b.sub);

    // In argument of a tensor
    if(new_a.sub.node_type() == IRNodeType::IntImm && (new_a.sub.as<IntImm>())->value() == 0){
        aImm = true;
    }
    if(new_b.sub.node_type() == IRNodeType::IntImm && (new_b.sub.as<IntImm>())->value() == 0){
        bImm = true;
    }

    if (main_pos == 1) // main in left
    {
        op_stack.push(new_b);
    }
    else if (main_pos == 2) // main in right
    {
        op_stack.push(new_a);
    }

    switch(op->op_type){
        case Boost::Internal::BinaryOpType::Add : 
        case Boost::Internal::BinaryOpType::Sub :
        case Boost::Internal::BinaryOpType::Mul :
        case Boost::Internal::BinaryOpType::Div :
            if(main_pos == 1)
                return new_b.sub;
            if(main_pos == 2)
                return new_a.sub;
            return Binary::make(op->type(), op->op_type, new_a.sub, new_b.sub);
            break;
        case Boost::Internal::BinaryOpType::Mod :
            has_mod = true;
            return Binary::make(op->type(), op->op_type, new_a.sub, new_b.sub);
            break;
        default:
            std::cout << "IndexMutator Binary op type error!" << std::endl;
            return Expr(0);
    }
}

Stmt 
IndexMutator::visit(Ref<const Move> op) {
    now_move = op;
    cst_idx.clear(); // clear constraints
    in_lhs = true;
    Expr new_dst = mutate(op->dst);
    in_lhs = false;
    Expr new_src = mutate(op->src);

    Stmt mov = Move::make(new_dst, new_src, op->move_type);

    if (cst_idx.size())
    {
        Ref<const Index> idx = originVar[cst_idx[0]];
        Ref<const Dom> now_dom = idx->dom.as<Dom>();
        Expr comp1 = Compare::make(Type::int_scalar(32), CompareOpType::GE,
            replaceExpr[idx->name], idx->dom.as<Dom>()->begin);
        Expr comp2 = Compare::make(Type::int_scalar(32), CompareOpType::LT,
            replaceExpr[idx->name], idx->dom.as<Dom>()->extent);
        Expr cond = Binary::make(Type::int_scalar(32), BinaryOpType::And,
            comp1, comp2);

        for (int i = 1; i < cst_idx.size(); ++i)
        {
            idx = originVar[cst_idx[i]];
            now_dom = idx->dom.as<Dom>();

            comp1 = Compare::make(Type::int_scalar(32), CompareOpType::GE,
                replaceExpr[idx->name], idx->dom.as<Dom>()->begin);
            comp2 = Compare::make(Type::int_scalar(32), CompareOpType::LT,
                replaceExpr[idx->name], idx->dom.as<Dom>()->extent);
            cond = Binary::make(Type::int_scalar(32), BinaryOpType::And,
                cond, Binary::make(Type::int_scalar(32), BinaryOpType::And,
                                    comp1, comp2));
        }

        return IfThenElse::make(cond, mov, Stmt());
    }

    return mov;
}

Stmt
IndexMutator::visit(Ref<const LoopNest> op) {
    std::vector<Expr> new_index_list;
    std::vector<Stmt> new_body_list;

    extendVar.clear();

    for (auto index : op->index_list) {
        originVar[index.as<Index>()->name] = index.as<Index>();
    }


    for (auto body : op->body_list) {
        new_body_list.push_back(mutate(body));
    }

    in_index = true;
    for (auto index : op->index_list) {
        std::string index_name = index.as<Index>()->name;
        if (victimVar.count(index_name) == 0)
            new_index_list.push_back(mutate(index));
        else // change old index to new temp index
        {

            Ref<const Index> tmp_idx = replaceVar[index_name];
            new_index_list.push_back(
                Index::make(tmp_idx->type(), tmp_idx->name, tmp_idx->dom, tmp_idx->index_type));
        }
    }
    for (auto index : extendVar) {
        new_index_list.push_back(
            Index::make(index->type(), index->name, index->dom, index->index_type));
    }
    in_index = false;

    return LoopNest::make(new_index_list, new_body_list);
}


void 
MyVisitor::visit(Ref<const Var> op) {
    // std::cout << op->name << ", ";
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