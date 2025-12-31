#include "semantic.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>

#include "../ast/ast.hpp"
#include "../ast/ast_visitor.hpp"
#include "../symbol/sematype.hpp"
#include "../symbol/symbol.hpp"
#include "sema_context.hpp"

namespace {
namespace helper {

// Forward declaration for recursive compatibility checks
bool typesCompatible(const SemaTypePtr& actual, const SemaTypePtr& expected);

// -----------------------------------------------------------------------------
// Type Classification
// -----------------------------------------------------------------------------

/* Checks if a semantic type is of a specific kind */
bool isIntType(const SemaTypePtr& t) {
	return t && t->getKind() == SemaType::TypeKind::INT;
}

bool isByteType(const SemaTypePtr& t) {
	return t && t->getKind() == SemaType::TypeKind::BYTE;
}

bool isArrayType(const SemaTypePtr& t) {
	return t && t->getKind() == SemaType::TypeKind::ARRAY;
}

// -----------------------------------------------------------------------------
// Type Comparison
// -----------------------------------------------------------------------------

/* Compares two semantic types for equality */
bool typesEqual(const SemaTypePtr& a, const SemaTypePtr& b) {
	if (a == b) {
		return true;
	}
	if (!a || !b) {
		return false;
	}
	return a->equals(*b);
}

// Checks compatibility of array types, allowing sized actuals for unsized parameters
bool arrayTypesCompatible(const ArrayType* actualArr, const ArrayType* expectedArr) {
	if (!actualArr || !expectedArr) {
		return false;
	}

	const auto expectedSize = expectedArr->size();
	const auto actualSize = actualArr->size();

	// If the parameter is sized, require an exact match from the caller
	if (expectedSize) {
		if (!actualSize || *actualSize != *expectedSize) {
			return false;
		}
	}
	// If the parameter is unsized, any actual size is acceptable
	return typesCompatible(actualArr->elementType(), expectedArr->elementType());
}

// Checks whether an "actual" type can be used where "expected" is required
bool typesCompatible(const SemaTypePtr& actual, const SemaTypePtr& expected) {
	if (actual == expected) {
		return true;
	}
	if (!actual || !expected) {
		return false;
	}

	if (expected->getKind() == SemaType::TypeKind::ARRAY) {
		if (actual->getKind() != SemaType::TypeKind::ARRAY) {
			return false;
		}
		return arrayTypesCompatible(
			static_cast<const ArrayType*>(actual.get()),
			static_cast<const ArrayType*>(expected.get()));
	}

	// For non-array types, fall back to structural equality
	return typesEqual(actual, expected);
}

/* Checks if a function/procedure signature matches the provided header info */
bool signaturesMatch(const SemContext::HeaderInfo& info, const Symbol* symbol) {
	if (!symbol || symbol->getKind() != Symbol::SymKind::FUNC) {
		return false;
	}
	const auto* func = static_cast<const FuncSymbol*>(symbol);
	if (func->isProcedure() != info.isProcedure) {
		return false;
	}
	std::vector<SemaTypePtr> paramTypes;
	paramTypes.reserve(info.params.size());
	for (const auto& param : info.params) {
		paramTypes.push_back(param.type);
	}
	auto expectedSig = makeFuncType(info.returnType, std::move(paramTypes));
	if (!typesEqual(expectedSig, func->getType())) {
		return false;
	}
	const auto& params = func->getParams();
	if (params.size() != info.params.size()) {
		return false;
	}
	for (std::size_t i = 0; i < params.size(); ++i) {
		if (!typesEqual(params[i]->getType(), info.params[i].type)) {
			return false;
		}
		if (params[i]->getPass() != info.params[i].passMode) {
			return false;
		}
	}
	return true;
}

// -----------------------------------------------------------------------------
// Type Construction and Resolution
// -----------------------------------------------------------------------------

/* Converts the frontend DataType enum to the corresponding semantic type */
SemaTypePtr scalarType(DataType dt) {
	return dt == DataType::INT ? makeIntType() : makeByteType();
}

/* Validates an array dimension */
bool validateDimension(const std::optional<int>& dim, bool allowUnsized, const SourceLoc& loc, SemContext& context) {
	if (!dim.has_value()) {
		if (allowUnsized) {
			return true;
		}
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
		                      "array dimension must be specified");
		return false;
	}
	if (*dim <= 0) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
		                      "array dimension must be greater than zero");
		return false;
	}
	return true;
}

/* Builds any semantic type from a AST captured info into an array type (or a base type, if no dims are present) */
/* If allowUnsizedFirst is true, the first dimension can be unsized (for parameters) */
SemaTypePtr buildArrayType(const SourceLoc& loc, SemaTypePtr base, const vec<std::optional<int>>& dims,
                           bool allowUnsizedFirst, SemContext& context) {
	SemaTypePtr result = std::move(base); // first move the scalar type to result
	for (std::size_t i = dims.size(); i-- > 0;) {
		const bool allowUnsized = allowUnsizedFirst && i == 0;
		if (!validateDimension(dims[i], allowUnsized, loc, context)) {
			return nullptr;
		}
		std::optional<std::size_t> extent;
		if (dims[i]) {
			extent = static_cast<std::size_t>(*dims[i]);
		}
		result = makeArrayType(result, extent); // if we have dims, wrap the current result into an array type
	}
	/* Returns:
		 * - base type if no dimensions
		 * - array type(s) if dimensions are present
		 */
	return result;
}

/* Resolves a type node into a semantic base type (int or byte) */
SemaTypePtr resolveType(const Type& node, SemContext& context, bool allowUnsizedFirst = false) {
	auto base = scalarType(node.data_type());
	return buildArrayType(node.loc, base, node.dimensions(), allowUnsizedFirst, context);
}

/* Resolves a function/procedure parameter type, determining its pass mode */
SemaTypePtr resolveParamType(const FParType& node, Symbol::ParamPass& pass, SemContext& context) {
	const bool isArray = !node.dimensions().empty();
	auto resolved = resolveType(node, context, true);
	if (!resolved) {
		return nullptr;
	}
	if (isArray) {
		pass = Symbol::ParamPass::BY_REF;
	} else {
		pass = node.isByRef() ? Symbol::ParamPass::BY_REF : Symbol::ParamPass::BY_VAL;
	}
	return resolved;
}

// -----------------------------------------------------------------------------
// Diagnostics
// -----------------------------------------------------------------------------

/* Converts a semantic type to its string representation for diagnostics */
std::string typeToString(const SemaTypePtr& type) {
	if (!type) {
		return "<invalid>";
	}
	switch (type->getKind()) {
		case SemaType::TypeKind::INT:
			return "int";
		case SemaType::TypeKind::BYTE:
			return "byte";
		case SemaType::TypeKind::VOID:
			return "void";
		case SemaType::TypeKind::ARRAY: {
			const auto* arr = static_cast<const ArrayType*>(type.get());
			std::ostringstream oss;
			oss << typeToString(arr->elementType()) << '[';
			if (arr->size()) {
				oss << *arr->size();
			}
			oss << ']';
			return oss.str();
		}
		case SemaType::TypeKind::FUNC:
			return "fn";
	}
	return "<unknown>";
}

} // namespace helper
} // namespace

class SemanticPass : public AstVisitor {
public:
	explicit SemanticPass(SemContext& context) : context_(context) {}

	void visit(Type& n) override { helper::resolveType(n, context_); }
	void visit(FParType& n) override {
		Symbol::ParamPass pass = Symbol::ParamPass::BY_VAL;
		helper::resolveParamType(n, pass, context_);
	}

	void visit(Program& n) override {
		auto* top = n.definition();
		if (!top) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                       SourceLoc::builtin(), "missing top level function definition");
			return;
		}

		top->accept(*this);

		auto* header = top->funcHeader();
		if (!header) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                       top->loc, "missing top level function header");
			return;
		}

		// check for main function semantic correctness
		FuncSymbol* main_sym = header->symbol();
		if (!main_sym) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                       header->loc, "failed to resolve top level function symbol");
			return;
		}
		// main cannot have a return type
		if (!main_sym->isProcedure()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, header->loc,
			                       "Top level function must be a procedure");
		}
		// main cannot have parameters
		if (!main_sym->getParams().empty()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, header->loc,
			                       "Top level function cannot have parameters");
		}
	}

	void visit(FParDef& n) override {
		if (auto* t = n.parameterType()) {
			t->accept(*this);
		}
	}

	void visit(Header& n) override {
		std::unordered_set<std::string> seen;
		SemContext::HeaderInfo info;
		info.name = n.identifier();
		info.loc = n.loc;
		const auto returnType = n.returnType();
		info.isProcedure = !returnType.has_value();
		info.returnType = info.isProcedure ? makeVoidType() : helper::scalarType(*returnType);

		for (const auto& param : n.parameters()) {
			if (!param) {
				continue;
			}
			const auto* typeNode = param->parameterType();
			if (!typeNode) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      param->loc, "missing parameter type");
				continue;
			}

			Symbol::ParamPass pass = Symbol::ParamPass::BY_VAL;
			auto paramType = helper::resolveParamType(*typeNode, pass, context_);
			if (!paramType) {
				continue;
			}

			for (const auto& id : param->names()) {
				if (!seen.insert(id).second) {
					context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
					                      param->loc,
					                      "duplicate parameter name '" + id + "' in header '" + info.name + "'");
					continue;
				}

				SemContext::ParamInfo infoParam;
				infoParam.name = id;
				infoParam.loc = param->loc;
				infoParam.type = paramType;
				infoParam.passMode = pass;
				info.params.push_back(std::move(infoParam));
			}
		}

		context_.setHeaderInfo(std::move(info));
	}

	void visit(VarDef& n) override {
		n.symbols().clear();
		auto* typeNode = n.declaredType();
		if (!typeNode) {
			return;
		}
		auto resolved = helper::resolveType(*typeNode, context_);
		if (!resolved) {
			return;
		}
		for (const auto& id : n.identifiers()) {
			auto sym = std::make_unique<VarSymbol>(id, resolved, n.loc);
			VarSymbol* raw = sym.get();
			if (auto* frame = context_.currentFunction()) {
				raw->setDefiningFunc(static_cast<FuncSymbol*>(frame->symbol));
			}
			context_.declareSymbol(std::move(sym));
			n.symbols().push_back(raw);
		}
	}

	void visit(FuncDecl& n) override {
		auto* header = n.funcHeader();
		if (!header) {
			return;
		}
		header->accept(*this);
		auto info = context_.takeHeaderInfo();
		if (!info) {
			return;
		}

		auto existing = context_.lookupLocalSymbol(info->name);
		Symbol* symbol = existing.symbol;
		if (symbol) {
			if (!helper::signaturesMatch(*info, symbol)) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      n.loc,
				                      "forward declaration of '" + info->name + "' conflicts with previous declaration");
				return;
			}
			if (symbol->isDefined()) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      n.loc, "symbol '" + info->name + "' already defined");
				return;
			}
			context_.diags().report(Diagnostics::Severity::Note, Diagnostics::Phase::Semantic,
			                      symbol->getLocation(), "previous declaration here");
			header->setSymbol(static_cast<FuncSymbol*>(symbol));
			return;
		}

		std::vector<SemaTypePtr> paramTypes;
		paramTypes.reserve(info->params.size());
		for (const auto& param : info->params) {
			paramTypes.push_back(param.type);
		}
		auto sig = makeFuncType(info->returnType, std::move(paramTypes));
		auto func = std::make_unique<FuncSymbol>(info->name, std::move(sig), info->isProcedure, info->loc);

		context_.openScope();
		for (const auto& paramInfo : info->params) {
			auto p = std::make_unique<ParamSymbol>(paramInfo.name, paramInfo.type, paramInfo.passMode, paramInfo.loc);
			p->setDefiningFunc(func.get());
			auto result = context_.declareSymbol(std::move(p));
			if (result.symbol) {
				func->addParam(static_cast<ParamSymbol*>(result.symbol));
			}
		}
		context_.closeScope();

		Symbol* raw = func.get();
		if (auto* frame = context_.currentFunction()) {
			raw->setDefiningFunc(static_cast<FuncSymbol*>(frame->symbol));
		}
		context_.declareSymbol(std::move(func));
		raw->markForwardDeclaration();
		header->setSymbol(static_cast<FuncSymbol*>(raw));
	}

	void visit(FuncDef& n) override {
		auto* header = n.funcHeader();
		if (!header) {
			return;
		}
		header->accept(*this);
		auto info = context_.takeHeaderInfo();
		if (!info) {
			return;
		}

		auto existing = context_.lookupLocalSymbol(info->name);
		Symbol* symbol = existing.symbol;
		if (symbol) {
			if (!helper::signaturesMatch(*info, symbol)) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      n.loc, "definition of '" + info->name + "' does not match prior declaration");
				return;
			}
			if (symbol->isDefined()) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      n.loc, "redefinition of '" + info->name + "'");
				return;
			}
			// Reuse existing symbol, don't create a new one
		} else {
			// Create new function symbol if not already declared
			std::vector<SemaTypePtr> paramTypes;
			paramTypes.reserve(info->params.size());
			for (const auto& param : info->params) {
				paramTypes.push_back(param.type);
			}
			auto sig = makeFuncType(info->returnType, std::move(paramTypes));
			auto func = std::make_unique<FuncSymbol>(info->name, std::move(sig), info->isProcedure, info->loc);
			symbol = func.get();
			symbol->setDefiningFunc(context_.currentFunction()
				? static_cast<FuncSymbol*>(context_.currentFunction()->symbol)
				: nullptr);
			context_.declareSymbol(std::move(func));
		}

		auto* fsym = static_cast<FuncSymbol*>(symbol);
		header->setSymbol(fsym);

		context_.openScope();
		// create a function frame and store inside SemContext driver
		SemContext::FunctionFrame frame;
		frame.symbol = symbol;
		frame.isProcedure = info->isProcedure;
		frame.returnType = info->returnType;
		context_.enterFunction(frame);

		// After entering function, declare parameters in the new scope
		// Only if the symbol was newly created (not forward-declared)
		if (!existing.symbol) {
			for (const auto& paramInfo : info->params) {
				auto sym = std::make_unique<ParamSymbol>(paramInfo.name, paramInfo.type, paramInfo.passMode, paramInfo.loc);
				sym->setDefiningFunc(fsym);
				auto result = context_.declareSymbol(std::move(sym));
				if (result.symbol) {
					fsym->addParam(static_cast<ParamSymbol*>(result.symbol));
				}
			}
		}

		for (auto& def : n.localDefs()) {
			if (def) {
				def->accept(*this);
			}
		}

		if (auto* body = n.funcBody()) {
			body->accept(*this);
		}

		context_.leaveFunction();
		context_.closeScope();
		symbol->markDefined();
	}

	void visit(Block& n) override {
		for (auto& stmt : n.statementsList()) {
			if (stmt) {
				stmt->accept(*this);
			}
		}
	}

	void visit(SkipStmt&) override {}
	void visit(ExitStmt&) override {}

	void visit(AssignStmt& n) override {
		auto* lhs = n.left();
		auto* rhs = n.right();
		if (lhs) {
			lhs->accept(*this);
		}
		if (rhs) {
			rhs->accept(*this);
		}
		auto leftType = lhs ? lhs->type() : SemaTypePtr{};
		auto rightType = rhs ? rhs->type() : SemaTypePtr{};
		if (!leftType) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "invalid assignment target");
			return;
		}
		if (helper::isArrayType(leftType)) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "cannot assign to an array value");
			return;
		}
		if (lhs && !lhs->isAssignable()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "left-hand side of assignment is not assignable");
			return;
		}
		if (!helper::typesEqual(leftType, rightType)) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, n.loc,
			                      "assignment type mismatch: left is '" + helper::typeToString(leftType) +
			                      "', right is '" + helper::typeToString(rightType) + "'");
		}
	}

	void visit(ReturnStmt& n) override {
		auto* value = n.returnValue();
		if (value) {
			value->accept(*this);
		}

		auto* frame = context_.currentFunction();
		if (!frame || frame->isProcedure || !value) {
			return;
		}
		if (!helper::typesEqual(frame->returnType, value->type())) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, n.loc,
			                      "return type mismatch: expected '" + helper::typeToString(frame->returnType) +
			                      "' but got '" + helper::typeToString(value->type()) + "'");
		}
	}

	void visit(ProcCall& n) override {
		auto lookup = context_.lookupSymbol(n.identifier());
		Symbol* symbol = lookup.symbol;
		auto* funcSym = (symbol && symbol->getKind() == Symbol::SymKind::FUNC)
			? static_cast<FuncSymbol*>(symbol)
			: nullptr;
		if (!funcSym || !funcSym->isProcedure()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "unknown procedure '" + n.identifier() + "'");
			n.setSymbol(nullptr);
			return;
		}
		n.setSymbol(funcSym);
		const auto& params = funcSym->getParams();
		checkArguments(n.arguments(), params, n.identifier(), n.loc);
	}

	void visit(BreakStmt&) override {}
	void visit(ContinueStmt&) override {}

	void visit(IfStmt& n) override {
		if (auto* cond = n.conditionExpr()) {
			cond->accept(*this);
		}
		if (auto* thenBranch = n.thenBlock()) {
			thenBranch->accept(*this);
		}
		for (auto& elif : n.elifs()) {
			if (elif.first) {
				elif.first->accept(*this);
			}
			if (elif.second) {
				elif.second->accept(*this);
			}
		}
		if (auto* elseBranch = n.elseBlock()) {
			elseBranch->accept(*this);
		}
	}

	void visit(LoopStmt& n) override {
		if (auto* body = n.loopBody()) {
			body->accept(*this);
		}
	}

	void visit(IdLVal& n) override {
		auto lookup = context_.lookupSymbol(n.identifier());
		Symbol* symbol = lookup.symbol;
		if (!symbol || !(symbol->isVariable() || symbol->isParameter())) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "unknown variable '" + n.identifier() + "'");
			n.setType(nullptr);
			n.setAssignable(false);
			n.setSymbol(nullptr);
			return;
		}
		n.setSymbol(symbol);
		n.setType(symbol->getType());
		n.setAssignable(true);
	}

	void visit(StringLiteralLVal& n) override {
		auto len = static_cast<std::size_t>(n.literal().size() + 1);
		n.setType(makeArrayType(makeByteType(), len));
		n.setAssignable(false);
	}

	void visit(IndexLVal& n) override {
		auto* base = n.baseExpr();
		auto* index = n.indexExpr();
		if (base) {
			base->accept(*this);
		}
		if (index) {
			index->accept(*this);
		}
		auto baseType = base ? base->type() : SemaTypePtr{};
		if (!helper::isArrayType(baseType)) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "cannot index non-array value");
			n.setType(nullptr);
			n.setAssignable(false);
			return;
		}
		if (!index || !helper::isIntType(index->type())) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "array index must be of type int");
		}
		n.setType(static_cast<const ArrayType*>(baseType.get())->elementType());
		n.setAssignable(base ? base->isAssignable() : true);
	}

	void visit(IntConst& n) override {
		n.setType(makeIntType());
		n.setConstExpr(true);
	}

	void visit(TrueConst& n) override {
		n.setType(makeByteType());
		n.setConstExpr(true);
	}

	void visit(FalseConst& n) override {
		n.setType(makeByteType());
		n.setConstExpr(true);
	}

	void visit(CharConst& n) override {
		n.setType(makeByteType());
		n.setConstExpr(true);
	}

	void visit(LValueExpr& n) override {
		auto* value = n.lvalue();
		if (value) {
			value->accept(*this);
		}
		n.setType(value ? value->type() : SemaTypePtr{});
		n.setLValue(true);
		n.setAssignable(value ? value->isAssignable() : false);
	}

	void visit(ParenExpr& n) override {
		auto* inner = n.innerExpr();
		if (inner) {
			inner->accept(*this);
		}
		n.setType(inner ? inner->type() : SemaTypePtr{});
		n.setLValue(inner && inner->isLValue());
		n.setAssignable(inner && inner->isAssignable());
	}

	void visit(FuncCall& n) override {
		auto lookup = context_.lookupSymbol(n.identifier());
		Symbol* symbol = lookup.symbol;
		auto* funcSym = (symbol && symbol->getKind() == Symbol::SymKind::FUNC)
			? static_cast<FuncSymbol*>(symbol)
			: nullptr;
		if (!funcSym || funcSym->isProcedure()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "unknown function '" + n.identifier() + "'");
			n.setSymbol(nullptr);
			n.setType(nullptr);
			return;
		}
		n.setSymbol(funcSym);
		const auto& params = funcSym->getParams();
		checkArguments(n.arguments(), params, n.identifier(), n.loc);
		const auto* sig = static_cast<const FuncType*>(funcSym->getType().get());
		n.setType(sig ? sig->returnType() : SemaTypePtr{});
		n.setLValue(false);
		n.setAssignable(false);
	}

	void visit(UnaryExpr& n) override {
		auto* operand = n.operandExpr();
		if (operand) {
			operand->accept(*this);
		}
		auto operandType = operand ? operand->type() : SemaTypePtr{};
		switch (n.opKind()) {
			case UnOp::Plus:
			case UnOp::Minus:
				if (!helper::isIntType(operandType)) {
					context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
					                      n.loc, "unary '+' and '-' require int operand");
				}
				n.setType(makeIntType());
				break;
			case UnOp::Not:
				if (!helper::isByteType(operandType)) {
					context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
					                      n.loc, "'!' requires byte operand");
				}
				n.setType(makeByteType());
				break;
		}
		n.setLValue(false);
		n.setAssignable(false);
	}

	void visit(BinaryExpr& n) override {
		auto* lhs = n.leftExpr();
		auto* rhs = n.rightExpr();
		if (lhs) {
			lhs->accept(*this);
		}
		if (rhs) {
			rhs->accept(*this);
		}
		auto leftType = lhs ? lhs->type() : SemaTypePtr{};
		auto rightType = rhs ? rhs->type() : SemaTypePtr{};
		switch (n.opKind()) {
			case BinOp::Add:
			case BinOp::Sub:
			case BinOp::Mul:
			case BinOp::Div:
			case BinOp::Mod:
				if (!helper::typesEqual(leftType, rightType) ||
					!(helper::isIntType(leftType) || helper::isByteType(leftType))) {
					context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
					                      n.loc, "arithmetic operands must both be int or byte");
				}
				n.setType(leftType);
				break;
			case BinOp::AndBits:
			case BinOp::OrBits:
				if (!helper::isByteType(leftType) || !helper::typesEqual(leftType, rightType)) {
					context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
					                      n.loc, "'&' and '|' require byte operands");
				}
				n.setType(makeByteType());
				break;
		}
		n.setLValue(false);
		n.setAssignable(false);
	}

	void visit(ExprCond& n) override {
		if (auto* expr = n.expression()) {
			expr->accept(*this);
		}
		if (!helper::isByteType(n.expression() ? n.expression()->type() : SemaTypePtr{})) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "condition expression must have type byte");
		}
		n.setType(makeByteType());
	}

	void visit(ParenCond& n) override {
		if (auto* inner = n.conditionExpr()) {
			inner->accept(*this);
		}
		n.setType(makeByteType());
	}

	void visit(NotCond& n) override {
		if (auto* inner = n.conditionExpr()) {
			inner->accept(*this);
		}
		n.setType(makeByteType());
	}

	void visit(BinaryCond& n) override {
		if (auto* left = n.leftCond()) {
			left->accept(*this);
		}
		if (auto* right = n.rightCond()) {
			right->accept(*this);
		}
		n.setType(makeByteType());
	}

	void visit(RelCond& n) override {
		if (auto* left = n.leftExpr()) {
			left->accept(*this);
		}
		if (auto* right = n.rightExpr()) {
			right->accept(*this);
		}
		auto lt = n.leftExpr() ? n.leftExpr()->type() : SemaTypePtr{};
		auto rt = n.rightExpr() ? n.rightExpr()->type() : SemaTypePtr{};
		if (!helper::typesEqual(lt, rt) || !(helper::isIntType(lt) || helper::isByteType(lt))) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "comparison requires operands of the same numeric type");
		}
		n.setType(makeByteType());
	}

private:
	bool checkArguments(const vec<up<Expr>>& args,
		                 const std::vector<ParamSymbol*>& params,
		                 const std::string& callee,
		                 const SourceLoc& loc) {
		if (args.size() != params.size()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
			                      "call to '" + callee + "' expects " + std::to_string(params.size()) +
			                      " argument(s) but got " + std::to_string(args.size()));
		}
		std::size_t count = std::min(args.size(), params.size());
		bool ok = args.size() == params.size();
		for (std::size_t i = 0; i < count; ++i) {
			auto* arg = args[i].get();
			if (arg) {
				arg->accept(*this);
			}
			auto actualType = arg ? arg->type() : SemaTypePtr{};
			if (!helper::typesCompatible(actualType, params[i]->getType())) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      arg ? arg->loc : loc,
				                      "in call to '" + callee + "', argument " + std::to_string(i + 1) +
				                      " has type '" + helper::typeToString(actualType) + "', expected '" +
				                      helper::typeToString(params[i]->getType()) + "'");
				ok = false;
			}
			if (params[i]->getPass() == Symbol::ParamPass::BY_REF) {
				if (!arg || !arg->isLValue()) {
					context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
					                      arg ? arg->loc : loc,
					                      "in call to '" + callee + "', argument " + std::to_string(i + 1) +
					                      " must be an l-value for by-ref parameter");
					ok = false;
				}
			}
		}
		for (std::size_t i = count; i < args.size(); ++i) {
			if (auto* arg = args[i].get()) {
				arg->accept(*this);
			}
		}
		return ok;
	}

	SemContext& context_;
};

class ControlFlowPass : public AstVisitor {
public:
	explicit ControlFlowPass(SemContext& context) : context_(context) {}

	void visit(Type&) override {}
	void visit(FParType&) override {}

	void visit(Program& n) override {
		if (auto* top = n.definition()) {
			top->accept(*this);
		}
	}

	void visit(FParDef&) override {}
	void visit(Header&) override {}
	void visit(VarDef&) override {}
	void visit(FuncDecl&) override {}

	void visit(FuncDef& n) override {
		auto* header = n.funcHeader();
		if (!header) {
			return;
		}
		FunctionInfo info;
		info.name = header->identifier();
		info.isProcedure = !header->returnType().has_value();
		functionStack_.push_back(info);

		for (auto& def : n.localDefs()) {
			if (def) {
				def->accept(*this);
			}
		}
		if (auto* body = n.funcBody()) {
			body->accept(*this);
		}

		if (!info.isProcedure && blockCanFallThrough(n.funcBody())) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "no path in function '" + info.name + "' returns a value");
		}

		functionStack_.pop_back();
	}

	void visit(Block& n) override {
		for (auto& stmt : n.statementsList()) {
			if (stmt) {
				stmt->accept(*this);
			}
		}
	}

	void visit(SkipStmt&) override {}

	void visit(ExitStmt& n) override {
		if (functionStack_.empty()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "'exit' outside of procedure");
			return;
		}
		if (!functionStack_.back().isProcedure) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "'exit' allowed only inside procedures");
		}
	}

	void visit(AssignStmt&) override {}

	void visit(ReturnStmt& n) override {
		if (functionStack_.empty()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "'return' outside of function");
			return;
		}
		if (functionStack_.back().isProcedure) {
			if (n.returnValue()) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      n.loc, "procedures cannot return a value");
			}
			return;
		}
		if (!n.returnValue()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "functions must return a value");
		}
	}

	void visit(ProcCall&) override {}

	void visit(BreakStmt& n) override {
		if (loopStack_.empty()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "'break' used outside of loop");
			return;
		}
		if (n.loopLabel() && activeLoopLabels_.find(*n.loopLabel()) == activeLoopLabels_.end()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "unknown loop label '" + *n.loopLabel() + "'");
		}
	}

	void visit(ContinueStmt& n) override {
		if (loopStack_.empty()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "'continue' used outside of loop");
			return;
		}
		if (n.loopLabel() && activeLoopLabels_.find(*n.loopLabel()) == activeLoopLabels_.end()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                      n.loc, "unknown loop label '" + *n.loopLabel() + "'");
		}
	}

	void visit(IfStmt& n) override {
		if (auto* cond = n.conditionExpr()) {
			cond->accept(*this);
		}
		if (auto* thenBlock = n.thenBlock()) {
			thenBlock->accept(*this);
		}
		for (auto& elif : n.elifs()) {
			if (elif.first) {
				elif.first->accept(*this);
			}
			if (elif.second) {
				elif.second->accept(*this);
			}
		}
		if (auto* elseBlock = n.elseBlock()) {
			elseBlock->accept(*this);
		}
	}

	void visit(LoopStmt& n) override {
		const auto& label = n.loopLabel();
		bool insertedLabel = false;
		if (label) {
			auto result = activeLoopLabels_.insert(*label);
			insertedLabel = result.second;
			if (!insertedLabel) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                      n.loc, "loop label '" + *label + "' already in use");
			}
		}
		loopStack_.push_back(label);

		if (auto* body = n.loopBody()) {
			body->accept(*this);
		}

		if (label && insertedLabel) {
			activeLoopLabels_.erase(*label);
		}
		loopStack_.pop_back();
	}

	void visit(IdLVal&) override {}
	void visit(StringLiteralLVal&) override {}
	void visit(IndexLVal&) override {}
	void visit(IntConst&) override {}
	void visit(CharConst&) override {}
	void visit(TrueConst&) override {}
	void visit(FalseConst&) override {}
	void visit(LValueExpr&) override {}
	void visit(ParenExpr&) override {}
	void visit(FuncCall&) override {}
	void visit(UnaryExpr&) override {}
	void visit(BinaryExpr&) override {}
	void visit(ExprCond&) override {}
	void visit(ParenCond&) override {}
	void visit(NotCond&) override {}
	void visit(BinaryCond&) override {}
	void visit(RelCond&) override {}

private:
	struct FunctionInfo {
		std::string name;
		bool isProcedure = false;
	};

	bool blockCanFallThrough(const Block* block) {
		if (!block) {
			return true;
		}
		bool canFallThrough = true;
		for (const auto& stmt : block->statementsList()) {
			if (!canFallThrough) {
				break;
			}
			canFallThrough = stmtCanFallThrough(stmt.get());
		}
		return canFallThrough;
	}

	bool stmtCanFallThrough(const Stmt* stmt) {
		if (!stmt) {
			return true;
		}
		if (dynamic_cast<const ReturnStmt*>(stmt)) {
			return false;
		}
		if (dynamic_cast<const ExitStmt*>(stmt)) {
			return false;
		}
		if (dynamic_cast<const BreakStmt*>(stmt)) {
			return false;
		}
		if (dynamic_cast<const ContinueStmt*>(stmt)) {
			return false;
		}
		if (const auto* ifstmt = dynamic_cast<const IfStmt*>(stmt)) {
			if (!ifstmt->elseBlock()) {
				return true;
			}
			bool canFallThrough = blockCanFallThrough(ifstmt->thenBlock());
			for (const auto& elif : ifstmt->elifs()) {
				canFallThrough = canFallThrough || blockCanFallThrough(elif.second.get());
			}
			canFallThrough = canFallThrough || blockCanFallThrough(ifstmt->elseBlock());
			return canFallThrough;
		}
		if (const auto* loop = dynamic_cast<const LoopStmt*>(stmt)) {
			return loopCanFallThrough(loop);
		}
		return true;
	}

	bool blockHasBreakForLoop(const Block* block,
		                     const std::optional<std::string>& loopLabel,
		                     int depth) {
		if (!block) {
			return false;
		}
		for (const auto& stmt : block->statementsList()) {
			if (stmtHasBreakForLoop(stmt.get(), loopLabel, depth)) {
				return true;
			}
		}
		return false;
	}

	bool stmtHasBreakForLoop(const Stmt* stmt,
		                    const std::optional<std::string>& loopLabel,
		                    int depth) {
		if (!stmt) {
			return false;
		}
		if (const auto* brk = dynamic_cast<const BreakStmt*>(stmt)) {
			if (brk->loopLabel()) {
				return loopLabel && *brk->loopLabel() == *loopLabel;
			}
			return depth == 0;
		}
		if (const auto* ifstmt = dynamic_cast<const IfStmt*>(stmt)) {
			if (blockHasBreakForLoop(ifstmt->thenBlock(), loopLabel, depth)) {
				return true;
			}
			for (const auto& elif : ifstmt->elifs()) {
				if (blockHasBreakForLoop(elif.second.get(), loopLabel, depth)) {
					return true;
				}
			}
			if (ifstmt->elseBlock() && blockHasBreakForLoop(ifstmt->elseBlock(), loopLabel, depth)) {
				return true;
			}
			return false;
		}
		if (const auto* loop = dynamic_cast<const LoopStmt*>(stmt)) {
			return blockHasBreakForLoop(loop->loopBody(), loopLabel, depth + 1);
		}
		return false;
	}

	bool loopCanFallThrough(const LoopStmt* loop) {
		if (!loop) {
			return true;
		}
		return blockHasBreakForLoop(loop->loopBody(), loop->loopLabel(), 0);
	}

	SemContext& context_;
	std::vector<FunctionInfo> functionStack_;
	std::vector<std::optional<std::string>> loopStack_;
	std::unordered_set<std::string> activeLoopLabels_;
};

void runSemanticPass(Program& program, SemContext& context) {
	SemanticPass pass(context);
	program.accept(pass);
}

void runControlFlowPass(Program& program, SemContext& context) {
	ControlFlowPass pass(context);
	program.accept(pass);
}