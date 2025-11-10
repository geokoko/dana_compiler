#include <string>
#include "../ast/ast.hpp"
#include "../symbol/symbol.hpp"
#include "sema_context.hpp"

// Semantic analysis functions for AST nodes
// Each sem() function performs semantic checks and type resolution

void Program::sem(SemContext& context) {
	top->sem(context);
}

void Block::sem(SemContext& context) {
	for (auto& stmt : statements) {
		stmt->sem(context);
	}
}

void FParType::sem(SemContext& context) {

}

void FParDef::sem(SemContext& context) {

}

void Header::sem(SemContext& context) {
	context.setCurrentHeader(this);
	std::
}

void VarDef::sem(SemContext& context) {

}

void FuncDecl::sem(SemContext& context) {

}

void FuncDef::sem(SemContext& context) {
	// check header semantics
	header->sem(context);
	// check local definitions semantics
	for (auto& def : locals) {
		def->sem(context);
	}
	// check body semantics
	body->sem(context);

	

}

void SkipStmt::sem(SemContext& context) {
// does nothing
	return;
}

void ExitStmt::sem(SemContext& context) {

}

void AssignStmt::sem(SemContext& context) {

}

void LoopStmt::sem(SemContext& context) {

}

void BreakStmt::sem(SemContext& context) {

} 

void ContinueStmt::sem(SemContext& context) {

}

void ReturnStmt::sem(SemContext& context) {

}

void ProcCall::sem(SemContext& context) {

}

void IfStmt::sem(SemContext& context) {

}

void IdLVal::sem(SemContext& context) {

}

void StringLiteralLVal::sem(SemContext& context) {

}

void IndexLVal::sem(SemContext& context) {
}

void LValueExpr::sem(SemContext& context) {
	value->sem(context);
	exprType = value->getType();
}

void IntConst::sem(SemContext& context) {
	(void)context; // to avoid unused parameter warning
	exprType = Type::Int();
}

void TrueConst::sem(SemContext& context) {
	(void)context;
	exprType = Type::Bool();
}

void FalseConst::sem(SemContext& context) {
	(void)context;
	exprType = Type::Bool();
}

void CharConst::sem(SemContext& context) {
	(void)context;
	exprType = Type::Byte();
}

void ParenExpr::sem(SemContext& context) {
	inner->sem(context);
	exprType = inner->getType();
}

void FuncCall::sem(SemContext& context) {
	Symbol* sym = context.symtab().lookup(name);
    if (!sym || !sym->isFunction()) {
        context.diags().error(loc,
            "call to unknown function '" + name + "'");
        exprType = Type::Error();
        return;
    }

    const FunctionType& ft = sym->functionType(); // adapt to your API

    if (args.size() != ft.paramCount()) {
        context.diags().error(loc,
            "wrong number of arguments in call to '" + name +
            "' (expected " + std::to_string(ft.paramCount()) +
            ", got " + std::to_string(args.size()) + ")");
        exprType = Type::Error();
    }

    // Check each argument.
    for (std::size_t i = 0; i < args.size(); ++i) {
        args[i]->sem(context);

        Type actual = args[i]->getType();
        const ParamInfo& formal = ft.param(i);

        if (actual.isError())
            continue;

        if (formal.passingMode == PassingMode::ByValue) {
            if (actual != formal.type) {
                context.diags().error(args[i]->loc,
                    "in call to '" + name + "', argument " +
                    std::to_string(i+1) + " has type '" +
                    actual.toString() + "', expected '" +
                    formal.type.toString() + "'");
            }
        } else { // by-reference: actual must be L-value of same type
            if (!args[i]->isLValue() || actual != formal.type) {
                context.diags().error(args[i]->loc,
                    "in call to '" + name + "', argument " +
                    std::to_string(i+1) +
                    " must be a variable (l-value) of type '" +
                    formal.type.toString() + "'");
            }
        }
    }

    // Result type = function return type.
    exprType = ft.returnType();
}

void UnaryExpr::sem(SemContext& context) {

}

void BinaryExpr::sem(SemContext& context) {
	eft->sem(context);
    right->sem(context);

    Type lt = left->getType();
    Type rt = right->getType();

    if (lt.isError() || rt.isError()) {
        exprType = Type::Error();
        return;
    }

    switch (op) {
    case BinaryOp::Plus:
    case BinaryOp::Minus:
    case BinaryOp::Mul:
    case BinaryOp::Div:
    case BinaryOp::Mod:
        // arithmetic: both int or both byte, result same type
        if ((lt.isInt() && rt.isInt()) ||
            (lt.isByte() && rt.isByte())) {
            exprType = lt; // same as rt
        } else {
            context.diags().error(loc,
                "arithmetic operator requires two int or two byte operands");
            exprType = Type::Error();
        }
        break;

    case BinaryOp::BitAnd:   // &
    case BinaryOp::BitOr:    // |
        if (lt.isByte() && rt.isByte()) {
            exprType = Type::Byte();
        } else {
            context.diags().error(loc,
                "operator '&' and '|' require byte (logical) operands");
            exprType = Type::Error();
        }
        break;

    case BinaryOp::Eq:
    case BinaryOp::Ne:
    case BinaryOp::Lt:
    case BinaryOp::Gt:
    case BinaryOp::Le:
    case BinaryOp::Ge:
        // comparison between numbers; result is condition
        if (!((lt.isInt() && rt.isInt()) ||
              (lt.isByte() && rt.isByte()))) {
            context.diags().error(loc,
                "relational operator requires operands of the same numeric type");
            exprType = Type::Error();
        } else {
            exprType = Type::Bool();  // or Type::Byte() if you don't distinguish
        }
        break;
    }
}

void Cond::sem(SemContext& context) {

}

void ExprCond::sem(SemContext& context) {

}

void ParenCond::sem(SemContext& context) {

}

void NotCond::sem(SemContext& context) {

}

void BinCond::sem(SemContext& context) {

}

void RelCond::sem(SemContext& context) {

}
