#include "ast.hpp"
#include <iostream>

// helper functions for printing lists of strings
void print_string_list(std::ostream& out, const vec<string>& items) {
    out << '[';
    for (std::size_t i = 0; i < items.size(); ++i) {
        if (i) out << ", ";
        out << items[i];
    }
    out << ']';
}

// helper function for printing lists of AST nodes
template <class T>
void print_node_list(std::ostream& out, const vec<up<T>>& nodes) {
    out << '[';
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (i) out << ", ";
        if (nodes[i]) {
            out << *nodes[i];
        } else {
            out << "null";
        }
    }
    out << ']';
}

void Type::print(std::ostream& out) const {
    out << "Type[" << loc.line << ':' << loc.col << "](";
    printTypeDetails(out);
    out << ')';
}

void Type::printTypeDetails(std::ostream& out) const {
    out << "base=" << dataTypeName(base) << ", dims=[";
    for (std::size_t i = 0; i < dims.size(); ++i) {
        if (i) out << ", ";
        if (dims[i]) {
            out << *dims[i];
        } else {
            out << '?';
        }
    }
    out << ']';
}

void Block::print(std::ostream& out) const {
    out << "Block[" << loc.line << ':' << loc.col << "](stmts=";
    print_node_list(out, statements);
    out << ')';
}

void Def::print(std::ostream& out) const {
    (void)out;
}

void Program::print(std::ostream& out) const {
    out << "Program[" << loc.line << ':' << loc.col << "](top=";
    if (top) {
        out << *top;
    } else {
        out << "null";
    }
    out << ')';
}

void FParType::print(std::ostream& out) const {
    out << "FParType[" << loc.line << ':' << loc.col << "](by_ref=" << (by_ref ? "true" : "false") << ", ";
    printTypeDetails(out);
    out << ')';
}

void FParDef::print(std::ostream& out) const {
    out << "FParDef[" << loc.line << ':' << loc.col << "](ids=";
    print_string_list(out, identifiers);
    out << ", type=";
    if (type) {
        out << *type;
    } else {
        out << "null";
    }
    out << ')';
}

void Header::print(std::ostream& out) const {
    out << "Header[" << loc.line << ':' << loc.col << "](name=" << name << ", return=";
    if (return_type) {
        out << dataTypeName(*return_type);
    } else {
        out << "null";
    }
    out << ", params=";
    print_node_list(out, params);
    out << ')';
}

void VarDef::print(std::ostream& out) const {
    out << "VarDef[" << loc.line << ':' << loc.col << "](names=";
    print_string_list(out, names);
    out << ", type=";
    if (declared_type) {
        out << *declared_type;
    } else {
        out << "null";
    }
    out << ')';
}

void FuncDecl::print(std::ostream& out) const {
    out << "FuncDecl[" << loc.line << ':' << loc.col << "](header=";
    if (header) {
        out << *header;
    } else {
        out << "null";
    }
    out << ')';
}

void FuncDef::print(std::ostream& out) const {
    out << "FuncDef[" << loc.line << ':' << loc.col << "](header=";
    if (header) {
        out << *header;
    } else {
        out << "null";
    }
    out << ", locals=";
    print_node_list(out, locals);
    out << ", body=";
    if (body) {
        out << *body;
    } else {
        out << "null";
    }
    out << ')';
}

void SkipStmt::print(std::ostream& out) const {
    out << "SkipStmt[" << loc.line << ':' << loc.col << ']';
}

void ExitStmt::print(std::ostream& out) const {
    out << "ExitStmt[" << loc.line << ':' << loc.col << ']';
}

void AssignStmt::print(std::ostream& out) const {
    out << "AssignStmt[" << loc.line << ':' << loc.col << "](lhs=";
    if (lhs) {
        out << *lhs;
    } else {
        out << "null";
    }
    out << ", rhs=";
    if (rhs) {
        out << *rhs;
    } else {
        out << "null";
    }
    out << ')';
}

void ReturnStmt::print(std::ostream& out) const {
    out << "ReturnStmt[" << loc.line << ':' << loc.col << "](expr=";
    if (value) {
        out << *value;
    } else {
        out << "null";
    }
    out << ')';
}

void ProcCall::print(std::ostream& out) const {
    out << "ProcCall[" << loc.line << ':' << loc.col << "](name=" << name << ", args=";
    print_node_list(out, args);
    out << ')';
}

void BreakStmt::print(std::ostream& out) const {
    out << "BreakStmt[" << loc.line << ':' << loc.col << "](label=";
    if (label) {
        out << *label;
    } else {
        out << "null";
    }
    out << ')';
}

void ContinueStmt::print(std::ostream& out) const {
    out << "ContinueStmt[" << loc.line << ':' << loc.col << "](label=";
    if (label) {
        out << *label;
    } else {
        out << "null";
    }
    out << ')';
}

void IfStmt::print(std::ostream& out) const {
    out << "IfStmt[" << loc.line << ':' << loc.col << "](cond=";
    if (condition) {
        out << *condition;
    } else {
        out << "null";
    }
    out << ", then=";
    if (then_branch) {
        out << *then_branch;
    } else {
        out << "null";
    }
    out << ", elifs=[";
    for (std::size_t i = 0; i < elif_branches.size(); ++i) {
        if (i) out << ", ";
        out << '(';
        if (elif_branches[i].first) {
            out << *elif_branches[i].first;
        } else {
            out << "null";
        }
        out << ", ";
        if (elif_branches[i].second) {
            out << *elif_branches[i].second;
        } else {
            out << "null";
        }
        out << ')';
    }
    out << "], else=";
    if (else_branch && *else_branch) {
        out << **else_branch;
    } else {
        out << "null";
    }
    out << ')';
}

void LoopStmt::print(std::ostream& out) const {
    out << "LoopStmt[" << loc.line << ':' << loc.col << "](label=";
    if (label) {
        out << *label;
    } else {
        out << "null";
    }
    out << ", body=";
    if (body) {
        out << *body;
    } else {
        out << "null";
    }
    out << ')';
}

void IdLVal::print(std::ostream& out) const {
    out << "IdLVal[" << loc.line << ':' << loc.col << "](name=" << name << ')';
}

void StringLiteralLVal::print(std::ostream& out) const {
    out << "StringLiteralLVal[" << loc.line << ':' << loc.col << "](value=\"" << value << "\")";
}

void IndexLVal::print(std::ostream& out) const {
    out << "IndexLVal[" << loc.line << ':' << loc.col << "](base=";
    if (base) {
        out << *base;
    } else {
        out << "null";
    }
    out << ", index=";
    if (index) {
        out << *index;
    } else {
        out << "null";
    }
    out << ')';
}

void IntConst::print(std::ostream& out) const {
    out << "IntConst[" << loc.line << ':' << loc.col << "](value=" << value << ')';
}

void CharConst::print(std::ostream& out) const {
    out << "CharConst[" << loc.line << ':' << loc.col << "](value=" << static_cast<int>(value) << ')';
}

void TrueConst::print(std::ostream& out) const {
    out << "TrueConst[" << loc.line << ':' << loc.col << ']';
}

void FalseConst::print(std::ostream& out) const {
    out << "FalseConst[" << loc.line << ':' << loc.col << ']';
}

void LValueExpr::print(std::ostream& out) const {
    out << "LValueExpr[" << loc.line << ':' << loc.col << "](value=";
    if (value) {
        out << *value;
    } else {
        out << "null";
    }
    out << ')';
}

void ParenExpr::print(std::ostream& out) const {
    out << "ParenExpr[" << loc.line << ':' << loc.col << "](expr=";
    if (inner) {
        out << *inner;
    } else {
        out << "null";
    }
    out << ')';
}

void FuncCall::print(std::ostream& out) const {
    out << "FuncCall[" << loc.line << ':' << loc.col << "](name=" << name << ", args=";
    print_node_list(out, args);
    out << ')';
}

void UnaryExpr::print(std::ostream& out) const {
    out << "UnaryExpr[" << loc.line << ':' << loc.col << "](op=" << unOpName(op) << ", expr=";
    if (operand) {
        out << *operand;
    } else {
        out << "null";
    }
    out << ')';
}

void BinaryExpr::print(std::ostream& out) const {
    out << "BinaryExpr[" << loc.line << ':' << loc.col << "](op=" << binOpName(op) << ", lhs=";
    if (lhs) {
        out << *lhs;
    } else {
        out << "null";
    }
    out << ", rhs=";
    if (rhs) {
        out << *rhs;
    } else {
        out << "null";
    }
    out << ')';
}

void ExprCond::print(std::ostream& out) const {
    out << "ExprCond[" << loc.line << ':' << loc.col << "](expr=";
    if (expr) {
        out << *expr;
    } else {
        out << "null";
    }
    out << ')';
}

void ParenCond::print(std::ostream& out) const {
    out << "ParenCond[" << loc.line << ':' << loc.col << "](cond=";
    if (condition) {
        out << *condition;
    } else {
        out << "null";
    }
    out << ')';
}

void NotCond::print(std::ostream& out) const {
    out << "NotCond[" << loc.line << ':' << loc.col << "](cond=";
    if (condition) {
        out << *condition;
    } else {
        out << "null";
    }
    out << ')';
}

void BinaryCond::print(std::ostream& out) const {
    out << "BinaryCond[" << loc.line << ':' << loc.col << "](op=" << logicOpName(op) << ", lhs=";
    if (lhs) {
        out << *lhs;
    } else {
        out << "null";
    }
    out << ", rhs=";
    if (rhs) {
        out << *rhs;
    } else {
        out << "null";
    }
    out << ')';
}

void RelCond::print(std::ostream& out) const {
    out << "RelCond[" << loc.line << ':' << loc.col << "](op=" << relOpName(op) << ", lhs=";
    if (lhs) {
        out << *lhs;
    } else {
        out << "null";
    }
    out << ", rhs=";
    if (rhs) {
        out << *rhs;
    } else {
        out << "null";
    }
    out << ')';
}


