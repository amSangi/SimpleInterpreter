#include "visitor/interpreter.h"
#include "ast.h"


using std::vector;
using std::string;
using std::shared_ptr;

double Interpreter::Evaluate(CheckedProgram checked_program) {
    evaluation_table_.EnterScope();
    VisitorValue main_value = checked_program.GetProgram()->Accept(this);
    evaluation_table_.LeaveScope();
    return main_value.double_value;
}

VisitorValue Interpreter::Visit(Program* n) {
    for (auto fun : n->GetFunctions()) {
        function_table_.insert({fun->GetId()->GetName(), fun});
    }

    return n->GetMain()->Accept(this);
}

/*********** Functions ***********/
VisitorValue Interpreter::Visit(FunctionDecl* n) {
    is_return_ = false;
    for (const auto& stm : n->GetStatements()) {
        VisitorValue value = stm->Accept(this);
        if (stm->IsReturn()) return value;
    }

    return VisitorValue(nullptr);
}

VisitorValue Interpreter::Visit(FunctionParam* n) {
    return VisitorValue(nullptr); // return not needed
}


/*********** Statements ***********/
VisitorValue Interpreter::Visit(Assignment* n) {
    string name = n->GetLValue()->GetName();
    VisitorValue value = n->GetRValue()->Accept(this);
    evaluation_table_.Put(name, value);

    return VisitorValue(nullptr); // return not needed
}

VisitorValue Interpreter::Visit(Block* n) {
    for (const auto& stm : n->GetStatements()) {
        VisitorValue value = stm->Accept(this);
        if (stm->IsReturn()) return value;
    }

    return VisitorValue(nullptr);
}

VisitorValue Interpreter::Visit(IfThenElse* n) {
    VisitorValue predicate = n->GetPredicate()->Accept(this);

    return predicate.bool_value ? n->GetThenStatement()->Accept(this) : n->GetElseStatement()->Accept(this);
}

VisitorValue Interpreter::Visit(While* n) {
    while (n->GetPredicate()->Accept(this).bool_value) {
        VisitorValue value = n->GetBlock()->Accept(this);
        if (is_return_) return value;
    }

    return VisitorValue(nullptr); // return not needed
}

VisitorValue Interpreter::Visit(VarDecl* n) {
    evaluation_table_.Put(n->GetId()->GetName(), VisitorValue(0.0));
    return VisitorValue(nullptr); // return not needed
}

VisitorValue Interpreter::Visit(ReturnStm* n) {
    return n->GetExpression()->Accept(this);
}

/*********** Expressions ***********/
VisitorValue Interpreter::Visit(Identifier* n) {
    return evaluation_table_.Get(n->GetName());
}

VisitorValue Interpreter::Visit(BinaryOp* n) {
    VisitorValue left = n->GetLeft()->Accept(this);
    VisitorValue right = n->GetRight()->Accept(this);
    switch (n->GetOperator()) {
        case PLUS:
            return VisitorValue(left.double_value + right.double_value);
        case MINUS:
            return VisitorValue(left.double_value - right.double_value);
        case MULTIPLY:
            return VisitorValue(left.double_value * right.double_value);
        case DIVIDE:
            return VisitorValue(left.double_value / right.double_value);
        case MODULO:
            return VisitorValue((double) ((long)left.double_value % (long)right.double_value));
        case AND:
            return VisitorValue(left.bool_value && right.bool_value);
        case OR:
            return VisitorValue(left.bool_value || right.bool_value);
        case GT:
            return VisitorValue(left.double_value > right.double_value);
        case GTE:
            return VisitorValue(left.double_value >= right.double_value);
        case LT:
            return VisitorValue(left.double_value < right.double_value);
        case LTE:
            return VisitorValue(left.double_value <= right.double_value);
        case EQ:
            return VisitorValue(left.double_value == right.double_value);
    }
    return VisitorValue(0.0);
}

VisitorValue Interpreter::Visit(UnaryOp* n) {
    VisitorValue value = n->GetExpression()->Accept(this);
    switch (n->GetOperator()) {
        case NOT:
            return VisitorValue(!(value.bool_value));
    }
    return VisitorValue(false);
}

VisitorValue Interpreter::Visit(FunctionCall* n) {
    FunDeclPtr fun = function_table_[n->GetId()->GetName()];

    evaluation_table_.EnterScope();

    // Map Arguments to Parameters
    vector<shared_ptr<Expression>> arguments = n->GetArguments();
    vector<shared_ptr<FunctionParam>> parameters = fun->GetFormals();

    for (int i = 0; i < arguments.size(); ++i) {
        auto arg = arguments[i];
        auto param = parameters[i];
        evaluation_table_.Put(param->GetId()->GetName(), arg->Accept(this));
    }

    // Execute function
    VisitorValue call_value = fun->Accept(this);

    evaluation_table_.LeaveScope();

    return call_value;
}

VisitorValue Interpreter::Visit(Conditional* n) {
    VisitorValue predicate = n->GetPredicate()->Accept(this);
    return predicate.bool_value ? n->GetTrueValue()->Accept(this) : n->GetFalseValue()->Accept(this);
}

VisitorValue Interpreter::Visit(NumLiteral* n) {
    return VisitorValue(n->GetValue());
}

VisitorValue Interpreter::Visit(BooleanLiteral* n) {
    return VisitorValue(n->GetValue());
}

/*********** Types ***********/
VisitorValue Interpreter::Visit(NumType* n) {
    return VisitorValue(nullptr); // return not needed
}

VisitorValue Interpreter::Visit(BoolType* n) {
    return VisitorValue(nullptr); // return not needed
}

VisitorValue Interpreter::Visit(VoidType* n) {
    return VisitorValue(nullptr); // return not needed
}

VisitorValue Interpreter::Visit(FunctionType* n) {
    return VisitorValue(nullptr); // return not needed
}