#include "semantic_pass.hpp"

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

// -----------------------------------------------------------------------------
// SemanticPass Constructor
// -----------------------------------------------------------------------------

SemanticPass::SemanticPass(SemContext& context) : context_(context) {}

// -----------------------------------------------------------------------------
// Type Utility Helpers (private static methods)
// -----------------------------------------------------------------------------

bool SemanticPass::isIntType(const SemaTypePtr& t) {
	return t && t->getKind() == SemaType::TypeKind::INT;
}

bool SemanticPass::isByteType(const SemaTypePtr& t) {
	return t && t->getKind() == SemaType::TypeKind::BYTE;
}

bool SemanticPass::isArrayType(const SemaTypePtr& t) {
	return t && t->getKind() == SemaType::TypeKind::ARRAY;
}

bool SemanticPass::typesEqual(const SemaTypePtr& a, const SemaTypePtr& b) {
	if (a == b) {
		return true;
	}
	if (!a || !b) {
		return false;
	}
	return a->equals(*b);
}

bool SemanticPass::arrayTypesCompatible(const ArrayType* actualArr, const ArrayType* expectedArr) {
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

bool SemanticPass::typesCompatible(const SemaTypePtr& actual, const SemaTypePtr& expected) {
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

SemaTypePtr SemanticPass::scalarType(DataType dt) {
	return dt == DataType::INT ? makeIntType() : makeByteType();
}

bool SemanticPass::validateDimension(const std::optional<int>& dim, bool allowUnsized, const SourceLoc& loc) {
	if (!dim.has_value()) {
		if (allowUnsized) {
			return true;
		}
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
						  "array dimension must be specified");
		return false;
	}
	if (*dim <= 0) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
						  "array dimension must be greater than zero");
		return false;
	}
	return true;
}

SemaTypePtr SemanticPass::buildArrayType(const SourceLoc& loc, SemaTypePtr base, 
										 const vec<std::optional<int>>& dims, bool allowUnsizedFirst) {
	SemaTypePtr result = std::move(base);
	for (std::size_t i = dims.size(); i-- > 0;) {
		const bool allowUnsized = allowUnsizedFirst && i == 0;
		if (!validateDimension(dims[i], allowUnsized, loc)) {
			return nullptr;
		}
		std::optional<std::size_t> extent;
		if (dims[i]) {
			extent = static_cast<std::size_t>(*dims[i]);
		}
		result = makeArrayType(result, extent);
	}
	return result;
}

SemaTypePtr SemanticPass::resolveType(const Type& node, bool allowUnsizedFirst) {
	auto base = scalarType(node.data_type());
	return buildArrayType(node.loc, base, node.dimensions(), allowUnsizedFirst);
}

SemaTypePtr SemanticPass::resolveParamType(const FParType& node, Symbol::ParamPass& pass) {
	const bool isArray = !node.dimensions().empty();
	auto resolved = resolveType(node, true);
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

std::string SemanticPass::typeToString(const SemaTypePtr& type) {
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

// -----------------------------------------------------------------------------
// Semantic Analysis Helpers (private methods)
// -----------------------------------------------------------------------------

bool SemanticPass::collectParams(const Header& header, std::vector<ParamInfo>& params) {
	std::unordered_set<std::string> seen;
	const std::string& headerName = header.identifier();

	for (const auto& param : header.parameters()) {
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
		auto paramType = resolveParamType(*typeNode, pass);
		if (!paramType) {
			continue;
		}

		for (const auto& id : param->names()) {
			if (!seen.insert(id).second) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
							param->loc,
							"duplicate parameter name '" + id + "' in header '" + headerName + "'");
				continue;
			}

			ParamInfo info;
			info.name = id;
			info.loc = param->loc;
			info.type = paramType;
			info.passMode = pass;
			params.push_back(std::move(info));
		}
	}
	return true;
}

bool SemanticPass::signaturesMatch(bool isProcedure, const SemaTypePtr& returnType,
								   const std::vector<ParamInfo>& params, const Symbol* symbol) {
	if (!symbol || symbol->getKind() != Symbol::SymKind::FUNC) {
		return false;
	}
	const auto* func = static_cast<const FuncSymbol*>(symbol);
	if (func->isProcedure() != isProcedure) {
		return false;
	}

	// Build expected signature from params
	std::vector<SemaTypePtr> paramTypes;
	paramTypes.reserve(params.size());
	for (const auto& param : params) {
		paramTypes.push_back(param.type);
	}
	auto expectedSig = makeFuncType(returnType, std::move(paramTypes));
	if (!typesEqual(expectedSig, func->getType())) {
		return false;
	}

	// Check parameter count and pass modes
	const auto& funcParams = func->getParams();
	if (funcParams.size() != params.size()) {
		return false;
	}
	for (std::size_t i = 0; i < funcParams.size(); ++i) {
		if (!typesEqual(funcParams[i]->getType(), params[i].type)) {
			return false;
		}
		if (funcParams[i]->getPass() != params[i].passMode) {
			return false;
		}
	}
	return true;
}

bool SemanticPass::checkArguments(const vec<up<Expr>>& args,
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
		if (!typesCompatible(actualType, params[i]->getType())) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						   arg ? arg->loc : loc,
						   "in call to '" + callee + "', argument " + std::to_string(i + 1) +
						   " has type '" + typeToString(actualType) + "', expected '" +
						   typeToString(params[i]->getType()) + "'");
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

// -----------------------------------------------------------------------------
// SemanticPass Visitor Implementations
// -----------------------------------------------------------------------------

void SemanticPass::visit(Type& n) { resolveType(n); }

void SemanticPass::visit(FParType& n) {
	Symbol::ParamPass pass = Symbol::ParamPass::BY_VAL;
	resolveParamType(n, pass);
}

void SemanticPass::visit(Program& n) {
	auto* top = n.definition();
	if (!top) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  SourceLoc::builtin(), "missing top level function definition");
		return;
	}

	top->setEntrypoint(true);
	top->accept(*this);

	auto* header = top->funcHeader();
	if (!header) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  top->loc, "missing top level function header");
		return;
	}

	FuncSymbol* main_sym = header->symbol();
	if (!main_sym) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  header->loc, "failed to resolve top level function symbol");
		return;
	}
	if (!main_sym->isProcedure()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, header->loc,
						  "Top level function must be a procedure");
	}
	if (!main_sym->getParams().empty()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, header->loc,
						  "Top level function cannot have parameters");
	}
}

void SemanticPass::visit(FParDef& n) {
	if (auto* t = n.parameterType()) {
		t->accept(*this);
	}
}

void SemanticPass::visit(Header&) {}

void SemanticPass::visit(VarDef& n) {
	n.symbols().clear();
	auto* typeNode = n.declaredType();
	if (!typeNode) {
		return;
	}
	auto resolved = resolveType(*typeNode);
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

void SemanticPass::visit(FuncDecl& n) {
	auto* header = n.funcHeader();
	if (!header) {
		return;
	}

	// Collect header info directly from AST
	const std::string& name = header->identifier();
	const auto returnTypeOpt = header->returnType();
	const bool isProcedure = !returnTypeOpt.has_value();
	const SemaTypePtr returnType = isProcedure ? makeVoidType() : scalarType(*returnTypeOpt);

	// Collect parameters
	std::vector<ParamInfo> params;
	if (!collectParams(*header, params)) {
		return; // errors already reported
	}

	// Check for existing declaration
	auto existing = context_.lookupLocalSymbol(name);
	Symbol* symbol = existing.symbol;
	if (symbol) {
		if (!signaturesMatch(isProcedure, returnType, params, symbol)) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						   n.loc,
						   "forward declaration of '" + name + "' conflicts with previous declaration");
			return;
		}
		if (symbol->isDefined()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						   n.loc, "symbol '" + name + "' already defined");
			return;
		}
		context_.diags().report(Diagnostics::Severity::Note, Diagnostics::Phase::Semantic,
						  symbol->getLocation(), "previous declaration here");
		header->setSymbol(static_cast<FuncSymbol*>(symbol));
		return;
	}

	// Create function signature and symbol
	std::vector<SemaTypePtr> paramTypes;
	paramTypes.reserve(params.size());
	for (const auto& p : params) {
		paramTypes.push_back(p.type);
	}
	auto sig = makeFuncType(returnType, std::move(paramTypes));
	auto func = std::make_unique<FuncSymbol>(name, std::move(sig), isProcedure, header->loc);

	// Create temporary scope for parameters (orphaned after FuncDecl)
	context_.openScope();
	for (const auto& paramInfo : params) {
		auto p = std::make_unique<ParamSymbol>(paramInfo.name, paramInfo.type, paramInfo.passMode, paramInfo.loc);
		p->setDefiningFunc(func.get());
		auto result = context_.declareSymbol(std::move(p));
		if (result.symbol) {
			func->addParam(static_cast<ParamSymbol*>(result.symbol));
		}
	}
	context_.closeScope();

	FuncSymbol* raw = func.get();
	if (auto* frame = context_.currentFunction()) {
		raw->setDefiningFunc(frame->symbol);
	}
	context_.declareSymbol(std::move(func));
	raw->markForwardDeclaration();
	header->setSymbol(raw);
}

void SemanticPass::visit(FuncDef& n) {
	auto* header = n.funcHeader();
	if (!header) {
		return;
	}

	// Collect header info directly from AST
	const std::string& name = header->identifier();
	const auto returnTypeOpt = header->returnType();
	const bool isProcedure = !returnTypeOpt.has_value();
	const SemaTypePtr returnType = isProcedure ? makeVoidType() : scalarType(*returnTypeOpt);

	// Collect parameters
	std::vector<ParamInfo> params;
	if (!collectParams(*header, params)) {
		return; // errors already reported
	}

	// Check for existing declaration
	auto existing = context_.lookupLocalSymbol(name);
	Symbol* symbol = existing.symbol;
	bool wasForwardDeclared = false;

	if (symbol) {
		if (!signaturesMatch(isProcedure, returnType, params, symbol)) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						   n.loc, "definition of '" + name + "' does not match prior declaration");
			return;
		}
		if (symbol->isDefined()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						   n.loc, "redefinition of '" + name + "'");
			return;
		}
		wasForwardDeclared = true;
	}

	FuncSymbol* fsym = nullptr;

	if (wasForwardDeclared) {
		// Use existing symbol from forward declaration
		fsym = static_cast<FuncSymbol*>(symbol);
		// Clear old params - they were orphaned when the FuncDecl's scope closed
		fsym->clearParams();
	} else {
		// Create new function symbol
		std::vector<SemaTypePtr> paramTypes;
		paramTypes.reserve(params.size());
		for (const auto& p : params) {
			paramTypes.push_back(p.type);
		}
		auto sig = makeFuncType(returnType, std::move(paramTypes));
		auto func = std::make_unique<FuncSymbol>(name, std::move(sig), isProcedure, header->loc);
		fsym = func.get();
		context_.declareSymbol(std::move(func));
	}

	fsym->setDefiningFunc(context_.currentFunction()
					   ? context_.currentFunction()->symbol
					   : nullptr);
	header->setSymbol(fsym);

	context_.openScope();

	// Create function frame
	SemContext::FunctionFrame frame;
	frame.symbol = fsym;
	frame.isProcedure = isProcedure;
	frame.returnType = returnType;
	context_.enterFunction(frame);

	// Declare parameters in the new scope
	for (const auto& paramInfo : params) {
		auto sym = std::make_unique<ParamSymbol>(paramInfo.name, paramInfo.type, paramInfo.passMode, paramInfo.loc);
		sym->setDefiningFunc(fsym);
		auto result = context_.declareSymbol(std::move(sym));
		if (result.symbol) {
			fsym->addParam(static_cast<ParamSymbol*>(result.symbol));
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
	fsym->markDefined();
}

void SemanticPass::visit(Block& n) {
	for (auto& stmt : n.statementsList()) {
		if (stmt) {
			stmt->accept(*this);
		}
	}
}

void SemanticPass::visit(SkipStmt&) {}
void SemanticPass::visit(ExitStmt&) {}

void SemanticPass::visit(AssignStmt& n) {
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
	if (isArrayType(leftType)) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "cannot assign to an array value");
		return;
	}
	if (lhs && !lhs->isAssignable()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "left-hand side of assignment is not assignable");
		return;
	}
	if (!typesEqual(leftType, rightType)) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, n.loc,
						  "assignment type mismatch: left is '" + typeToString(leftType) +
						  "', right is '" + typeToString(rightType) + "'");
	}
}

void SemanticPass::visit(ReturnStmt& n) {
	auto* value = n.returnValue();
	if (value) {
		value->accept(*this);
	}

	auto* frame = context_.currentFunction();
	if (!frame || frame->isProcedure || !value) {
		return;
	}
	if (!typesEqual(frame->returnType, value->type())) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, n.loc,
						  "return type mismatch: expected '" + typeToString(frame->returnType) +
						  "' but got '" + typeToString(value->type()) + "'");
	}
}

void SemanticPass::visit(ProcCall& n) {
	auto lookup = context_.lookupSymbol(n.identifier());
	Symbol* symbol = lookup.symbol;
	auto* funcSym = (symbol && symbol->getKind() == Symbol::SymKind::FUNC)
		? static_cast<FuncSymbol*>(symbol)
		: nullptr;
	if (!funcSym || !funcSym->isProcedure()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "unknown procedure '" + n.identifier() + "'");
		n.setFuncSymbol(nullptr);
		return;
	}
	n.setFuncSymbol(funcSym);
	const auto& params = funcSym->getParams();
	checkArguments(n.arguments(), params, n.identifier(), n.loc);
}

void SemanticPass::visit(BreakStmt&) {}
void SemanticPass::visit(ContinueStmt&) {}

void SemanticPass::visit(IfStmt& n) {
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

void SemanticPass::visit(LoopStmt& n) {
	if (auto* body = n.loopBody()) {
		body->accept(*this);
	}
}

void SemanticPass::visit(IdLVal& n) {
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

void SemanticPass::visit(StringLiteralLVal& n) {
	auto len = static_cast<std::size_t>(n.literal().size() + 1);
	n.setType(makeArrayType(makeByteType(), len));
	n.setAssignable(false);
}

void SemanticPass::visit(IndexLVal& n) {
	auto* base = n.baseExpr();
	auto* index = n.indexExpr();
	if (base) {
		base->accept(*this);
	}
	if (index) {
		index->accept(*this);
	}
	auto baseType = base ? base->type() : SemaTypePtr{};
	if (!isArrayType(baseType)) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "cannot index non-array value");
		n.setType(nullptr);
		n.setAssignable(false);
		return;
	}
	if (!index || !isIntType(index->type())) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "array index must be of type int");
	}
	n.setType(static_cast<const ArrayType*>(baseType.get())->elementType());
	n.setAssignable(base ? base->isAssignable() : true);
}

void SemanticPass::visit(IntConst& n) {
	n.setType(makeIntType());
	n.setConstExpr(true);
}

void SemanticPass::visit(TrueConst& n) {
	n.setType(makeByteType());
	n.setConstExpr(true);
}

void SemanticPass::visit(FalseConst& n) {
	n.setType(makeByteType());
	n.setConstExpr(true);
}

void SemanticPass::visit(CharConst& n) {
	n.setType(makeByteType());
	n.setConstExpr(true);
}

void SemanticPass::visit(LValueExpr& n) {
	auto* value = n.lvalue();
	if (value) {
		value->accept(*this);
	}
	n.setType(value ? value->type() : SemaTypePtr{});
	n.setLValue(true);
	n.setAssignable(value ? value->isAssignable() : false);
}

void SemanticPass::visit(ParenExpr& n) {
	auto* inner = n.innerExpr();
	if (inner) {
		inner->accept(*this);
	}
	n.setType(inner ? inner->type() : SemaTypePtr{});
	n.setLValue(inner && inner->isLValue());
	n.setAssignable(inner && inner->isAssignable());
}

void SemanticPass::visit(FuncCall& n) {
	auto lookup = context_.lookupSymbol(n.identifier());
	Symbol* symbol = lookup.symbol;
	auto* funcSym = (symbol && symbol->getKind() == Symbol::SymKind::FUNC)
		? static_cast<FuncSymbol*>(symbol)
		: nullptr;
	if (!funcSym || funcSym->isProcedure()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "unknown function '" + n.identifier() + "'");
		n.setFuncSymbol(nullptr);
		n.setType(nullptr);
		return;
	}
	n.setFuncSymbol(funcSym);
	const auto& params = funcSym->getParams();
	checkArguments(n.arguments(), params, n.identifier(), n.loc);
	const auto* sig = static_cast<const FuncType*>(funcSym->getType().get());
	n.setType(sig ? sig->returnType() : SemaTypePtr{});
	n.setLValue(false);
	n.setAssignable(false);
}

void SemanticPass::visit(UnaryExpr& n) {
	auto* operand = n.operandExpr();
	if (operand) {
		operand->accept(*this);
	}
	auto operandType = operand ? operand->type() : SemaTypePtr{};
	switch (n.opKind()) {
		case UnOp::Plus:
		case UnOp::Minus:
			if (!isIntType(operandType)) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
							n.loc, "unary '+' and '-' require int operand");
			}
			n.setType(makeIntType());
			break;
		case UnOp::Not:
			if (!isByteType(operandType)) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
							n.loc, "'!' requires byte operand");
			}
			n.setType(makeByteType());
			break;
	}
	n.setLValue(false);
	n.setAssignable(false);
}

void SemanticPass::visit(BinaryExpr& n) {
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
			if (!typesEqual(leftType, rightType) ||
				!(isIntType(leftType) || isByteType(leftType))) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
							n.loc, "arithmetic operands must both be int or byte");
			}
			n.setType(leftType);
			break;
		case BinOp::AndBits:
		case BinOp::OrBits:
			if (!isByteType(leftType) || !typesEqual(leftType, rightType)) {
				context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
							n.loc, "'&' and '|' require byte operands");
			}
			n.setType(makeByteType());
			break;
	}
	n.setLValue(false);
	n.setAssignable(false);
}

void SemanticPass::visit(ExprCond& n) {
	if (auto* expr = n.expression()) {
		expr->accept(*this);
	}
	// Dana does not allow bare expressions as conditions; 
	// conditions must be relational (=, <>, <, >, <=, >=) or logical (and, or, not)
	context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
					  n.loc, "bare expression cannot be used as condition");
	n.setType(makeByteType());
}

void SemanticPass::visit(ParenCond& n) {
	if (auto* inner = n.conditionExpr()) {
		inner->accept(*this);
	}
	n.setType(makeByteType());
}

void SemanticPass::visit(NotCond& n) {
	if (auto* inner = n.conditionExpr()) {
		inner->accept(*this);
	}
	n.setType(makeByteType());
}

void SemanticPass::visit(BinaryCond& n) {
	if (auto* left = n.leftCond()) {
		left->accept(*this);
	}
	if (auto* right = n.rightCond()) {
		right->accept(*this);
	}
	n.setType(makeByteType());
}

void SemanticPass::visit(RelCond& n) {
	if (auto* left = n.leftExpr()) {
		left->accept(*this);
	}
	if (auto* right = n.rightExpr()) {
		right->accept(*this);
	}
	auto lt = n.leftExpr() ? n.leftExpr()->type() : SemaTypePtr{};
	auto rt = n.rightExpr() ? n.rightExpr()->type() : SemaTypePtr{};
	if (!typesEqual(lt, rt) || !(isIntType(lt) || isByteType(lt))) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "comparison requires operands of the same numeric type");
	}
	n.setType(makeByteType());
}

// -----------------------------------------------------------------------------
// Entry Point
// -----------------------------------------------------------------------------

void runSemanticPass(Program& program, SemContext& context) {
	SemanticPass pass(context);
	program.accept(pass);
}
