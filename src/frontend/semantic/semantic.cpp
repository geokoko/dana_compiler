#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <cassert>

#include "../ast/ast.hpp"
#include "../symbol/sematype.hpp"
#include "../symbol/symbol.hpp"
#include "sema_context.hpp"

namespace {

// Forward declaration for recursive compatibility checks
bool typesCompatible(const SemaTypePtr& actual, const SemaTypePtr& expected);

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

/* Converts the frontend DataType enum to the corresponding semantic type */
SemaTypePtr scalarType(DataType dt) {
	return dt == DataType::INT ? makeIntType() : makeByteType();
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



/* Validates an array dimension */
bool validateDimension(const std::optional<int>& dim, bool allowUnsized, const SourceLoc& loc, SemContext& context) {
	if (!dim.has_value()) {
		if (allowUnsized) {
			return true;
		}
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "array dimension must be specified");
		return false;
	}
	if (*dim <= 0) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "array dimension must be greater than zero");
		return false;
	}
	return true;
}

/* Builds any semantic type from a AST captured info into an array type (or a base type, if no dims are present) */
/* If allowUnsizedFirst is true, the first dimension can be unsized (for parameters) */
SemaTypePtr buildArrayType(const SourceLoc& loc, SemaTypePtr base, const vec<std::optional<int>>& dims, 
						   bool allowUnsizedFirst, SemContext& context) 
{
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

/* Sets function/procedure parameters from header info into the given symbol */
void setSymbolParamsFromHeader(Symbol* symbol, const SemContext::HeaderInfo& info) {
	if (!symbol || symbol->getKind() != Symbol::SymKind::FUNC) {
		return;
	}
	auto* func = static_cast<FuncSymbol*>(symbol);
	func->clearParams();
	for (const auto& param : info.params) {
		auto p = std::make_shared<ParamSymbol>(param.name, param.type, param.passMode, param.loc);
		p->setDefiningFunc(func);
		func->addParam(std::move(p));
	}
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

/* Declares function/procedure parameters in the current scope */
void declareParamsInScope(const SemContext::HeaderInfo& info, SemContext& context) {
	for (const auto& param : info.params) {
		auto sym = std::make_unique<ParamSymbol>(param.name, param.type, param.passMode, param.loc);
		if (auto* frame = context.currentFunction()) {
			sym->setDefiningFunc(static_cast<FuncSymbol*>(frame->symbol));
		}
		context.declareSymbol(std::move(sym));
	}
}

/* Ensures that a loop label is not already in use */
bool ensureLoopLabelAvailable(SemContext& context, const std::optional<std::string>& label, const SourceLoc& loc) {
	if (!label) {
		return true;
	}
	if (context.hasLoopLabel(*label)) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "loop label '" + *label + "' already in use");
		return false;
	}
	return true;
}

/* Checks function/procedure call arguments against parameters */
bool checkArguments(vec<up<Expr>>& args,
					const std::vector<std::shared_ptr<ParamSymbol>>& params,
					SemContext& context,
					const std::string& callee,
					const SourceLoc& loc) {
	if (args.size() != params.size()) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
						"call to '" + callee + "' expects " + std::to_string(params.size()) +
						" argument(s) but got " + std::to_string(args.size()));
	}
	std::size_t count = std::min(args.size(), params.size());
	bool ok = args.size() == params.size();
	for (std::size_t i = 0; i < count; ++i) {
		auto& arg = args[i];
		if (arg) {
			arg->sem(context);
		}
		auto actualType = arg ? arg->type() : SemaTypePtr{};
		if (!typesCompatible(actualType, params[i]->getType())) {
			context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
			                       arg ? arg->loc : loc,
			                       "in call to '" + callee + "', argument " + std::to_string(i + 1) +
			                       " has type '" + typeToString(actualType) + "', expected '" +
			                       typeToString(params[i]->getType()) + "'");
			ok = false;
		}
		if (params[i]->getPass() == Symbol::ParamPass::BY_REF) {
			if (!arg || !arg->isLValue()) {
				context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
				                       arg ? arg->loc : loc,
				                       "in call to '" + callee + "', argument " + std::to_string(i + 1) +
				                       " must be an l-value for by-ref parameter");
				ok = false;
			}
		}
	}
	for (std::size_t i = count; i < args.size(); ++i) {
		if (args[i]) {
			args[i]->sem(context);
		}
	}
	return ok;
}

} // helper namespace 

// Semantic analysis functions for AST nodes
// Each sem() function performs semantic checks and type resolution

void Program::sem(SemContext& context) {
	if (top) {
		top->sem(context);
	}
}

void Block::sem(SemContext& context) {
	for (auto& stmt : statements) stmt->sem(context);
}

void FParType::sem(SemContext& context) {
	Symbol::ParamPass pass = Symbol::ParamPass::BY_VAL;
	resolveParamType(*this, pass, context);
}

void FParDef::sem(SemContext& context) {
	if (type) {
		type->sem(context);
	}
}

void Header::sem(SemContext& context) {
	std::unordered_set<std::string> seen;
	SemContext::HeaderInfo info;
	info.name = name;
	info.loc = loc;
	info.isProcedure = !return_type.has_value();
	info.returnType = info.isProcedure ? makeVoidType() : scalarType(*return_type);

	for (const auto& param : parameters()) {
		if (!param) {
			continue;
		}
		const auto* typeNode = param->parameterType();
		if (!typeNode) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, param->loc, "missing parameter type");
			continue;
		}

		Symbol::ParamPass pass = Symbol::ParamPass::BY_VAL;
		auto paramType = resolveParamType(*typeNode, pass, context);
		if (!paramType) {
			continue;
		}

		for (const auto& id : param->names()) {
			if (!seen.insert(id).second) {
				context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						   param->loc, "duplicate parameter name '" + id + "' in header '" + name + "'");
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

	context.setHeaderInfo(std::move(info));
}

void VarDef::sem(SemContext& context) {
	symbols_.clear();
	auto resolved = resolveType(*declared_type, context);
	if (!resolved) {
		return;
	}
	for (const auto& id : names) {
		auto sym = std::make_unique<VarSymbol>(id, resolved, loc);
		VarSymbol* raw = sym.get();
		if (auto* frame = context.currentFunction()) {
			raw->setDefiningFunc(static_cast<FuncSymbol*>(frame->symbol));
		}
		context.declareSymbol(std::move(sym));
		symbols_.push_back(raw);
	}
}

void FuncDecl::sem(SemContext& context) {
	header->sem(context);
	auto info = context.takeHeaderInfo();
	std::cout << "FuncDecl::sem: processing declaration of " << (info ? info->name : "<invalid>") << "\n";
	if (!info) {
		return;
	}

	auto existing = context.lookupLocalSymbol(info->name);
	Symbol* symbol = existing.symbol;
	if (symbol) {
		if (!signaturesMatch(*info, symbol)) {
			context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						  loc, "forward declaration of '" + info->name + "' conflicts with previous declaration");
			return;
		}
		if (symbol->isDefined()) {
			context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						  loc, "symbol '" + info->name + "' already defined");
			return;
		}
		context.diags().report(Diagnostics::Severity::Note, Diagnostics::Phase::Semantic,
		                      symbol->getLocation(), "previous declaration here");
		header->setSymbol(static_cast<FuncSymbol*>(symbol));
		return;
	}

	auto sym = context.makeFunctionSymbol(*info);
	Symbol* raw = sym.get();
	if (auto* frame = context.currentFunction()) {
		raw->setDefiningFunc(static_cast<FuncSymbol*>(frame->symbol));
	}
	context.declareSymbol(std::move(sym));
	raw->markForwardDeclaration();
	header->setSymbol(static_cast<FuncSymbol*>(raw));
}

void FuncDef::sem(SemContext& context) {
	header->sem(context);
	auto info = context.takeHeaderInfo();
	if (!info) {
		return;
	}

	auto existing = context.lookupLocalSymbol(info->name);
	Symbol* symbol = existing.symbol;
	if (symbol) {
		if (!signaturesMatch(*info, symbol)) {
			context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						  loc, "definition of '" + info->name + "' does not match prior declaration");
			return;
		}
		if (symbol->isDefined()) {
			context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						  loc, "redefinition of '" + info->name + "'");
			return;
		}
	} else {
		auto sym = context.makeFunctionSymbol(*info);
		symbol = sym.get();
		context.declareSymbol(std::move(sym));
	}

	if (auto* frame = context.currentFunction()) {
		symbol->setDefiningFunc(static_cast<FuncSymbol*>(frame->symbol));
	}
	header->setSymbol(static_cast<FuncSymbol*>(symbol));

	context.openScope();
	// create a function frame and store inside SemContext driver
	SemContext::FunctionFrame frame;
	frame.symbol = symbol;
	frame.isProcedure = info->isProcedure;
	frame.returnType = info->returnType;
	frame.sawReturn = false;
	context.enterFunction(frame);

	setSymbolParamsFromHeader(symbol, *info);
	declareParamsInScope(*info, context);

	for (auto& def : locals) {
		if (def) {
			def->sem(context);
		}
	}

	if (body) {
		body->sem(context);
	}

	bool needsReturn = false;
	if (auto* active = context.currentFunction()) {
		needsReturn = !active->isProcedure && !active->sawReturn;
	}
	context.leaveFunction();
	context.closeScope();
	symbol->markDefined();

	if (needsReturn) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "no path in function '" + info->name + "' returns a value");
	}
}

void SkipStmt::sem(SemContext&) {}

void ExitStmt::sem(SemContext& context) {
	auto* frame = context.currentFunction();
	if (!frame) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "'exit' outside of procedure");
		return;
	}
	if (!frame->isProcedure) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "'exit' allowed only inside procedures");
	}
}

void AssignStmt::sem(SemContext& context) {
	if (lhs) {
		lhs->sem(context);
	}
	if (rhs) {
		rhs->sem(context);
	}
	auto leftType = lhs ? lhs->type() : SemaTypePtr{};
	auto rightType = rhs ? rhs->type() : SemaTypePtr{};
	if (!leftType) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "invalid assignment target");
		return;
	}
	if (isArrayType(leftType)) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "cannot assign to an array value");
		return;
	}
	if (lhs && !lhs->isAssignable()) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "left-hand side of assignment is not assignable");
		return;
	}
	if (!typesEqual(leftType, rightType)) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
			"assignment type mismatch: left is '" + typeToString(leftType) + "', right is '" + typeToString(rightType) + "'");
	}
}

void LoopStmt::sem(SemContext& context) {
	if (!ensureLoopLabelAvailable(context, label, loc)) {
		return;
	}
	context.pushLoop(label);
	if (body) {
		body->sem(context);
	}
	context.popLoop();
}

void BreakStmt::sem(SemContext& context) {
	if (!context.inLoop()) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "'break' used outside of loop");
		return;
	}
	if (label && !context.hasLoopLabel(*label)) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "unknown loop label '" + *label + "'");
	}
}

void ContinueStmt::sem(SemContext& context) {
	if (!context.inLoop()) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "'continue' used outside of loop");
		return;
	}
	if (label && !context.hasLoopLabel(*label)) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "unknown loop label '" + *label + "'");
	}
}

void ReturnStmt::sem(SemContext& context) {
	auto* frame = context.currentFunction();
	if (!frame) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "'return' outside of function");
		return;
	}
	if (value) {
		value->sem(context);
	}
	if (frame->isProcedure) {
		if (value) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "procedures cannot return a value");
		}
		frame->sawReturn = true;
		return;
	}
	if (!value) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "functions must return a value");
		return;
	}
	if (!typesEqual(frame->returnType, value->type())) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc,
						"return type mismatch: expected '" + typeToString(frame->returnType) +
						"' but got '" + typeToString(value->type()) + "'");
	}
	frame->sawReturn = true;
}

void ProcCall::sem(SemContext& context) {
	auto lookup = context.lookupSymbol(name);
	Symbol* symbol = lookup.symbol;
	auto* funcSym = (symbol && symbol->getKind() == Symbol::SymKind::FUNC)
		? static_cast<FuncSymbol*>(symbol)
		: nullptr;
	if (!funcSym || !funcSym->isProcedure()) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "unknown procedure '" + name + "'");
		setSymbol(nullptr);
		return;
	}
	setSymbol(funcSym);
	const auto& params = funcSym->getParams();
	checkArguments(args, params, context, name, loc);
}

void IfStmt::sem(SemContext& context) {
	if (condition) {
		condition->sem(context);
	}
	if (then_branch) {
		then_branch->sem(context);
	}
	for (auto& elif : elif_branches) {
		if (elif.first) {
			elif.first->sem(context);
		}
		if (elif.second) {
			elif.second->sem(context);
		}
	}
	if (else_branch && *else_branch) {
		(*else_branch)->sem(context);
	}
}

void IdLVal::sem(SemContext& context) {
	auto lookup = context.lookupSymbol(name);
	Symbol* symbol = lookup.symbol;
	if (!symbol || !(symbol->isVariable() || symbol->isParameter())) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "unknown variable '" + name + "'");
		setType(nullptr);
		setAssignable(false);
		setSymbol(nullptr);
		return;
	}
	setSymbol(symbol);
	setType(symbol->getType());
	setAssignable(true);
}

void StringLiteralLVal::sem(SemContext& context) {
	(void)context;
	auto len = static_cast<std::size_t>(value.size() + 1);
	setType(makeArrayType(makeByteType(), len));
	setAssignable(false);
}

void IndexLVal::sem(SemContext& context) {
	if (base) {
		base->sem(context);
	}
	if (index) {
		index->sem(context);
	}
	auto baseType = base ? base->type() : SemaTypePtr{};
	if (!isArrayType(baseType)) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "cannot index non-array value");
		setType(nullptr);
		setAssignable(false);
		return;
	}
	if (!index || !isIntType(index->type())) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "array index must be of type int");
	}
	setType(static_cast<const ArrayType*>(baseType.get())->elementType());
	setAssignable(base ? base->isAssignable() : true);
}

void LValueExpr::sem(SemContext& context) {
	if (value) {
		value->sem(context);
	}
	setType(value ? value->type() : SemaTypePtr{});
	setLValue(true);
	setAssignable(value ? value->isAssignable() : false);
}

void IntConst::sem(SemContext& context) {
	(void)context;
	setType(makeIntType());
	setConstExpr(true);
}

void TrueConst::sem(SemContext& context) {
	(void)context;
	setType(makeByteType());
	setConstExpr(true);
}

void FalseConst::sem(SemContext& context) {
	(void)context;
	setType(makeByteType());
	setConstExpr(true);
}

void CharConst::sem(SemContext& context) {
	(void)context;
	setType(makeByteType());
	setConstExpr(true);
}

void ParenExpr::sem(SemContext& context) {
	if (inner) {
		inner->sem(context);
	}
	setType(inner ? inner->type() : SemaTypePtr{});
	setLValue(inner && inner->isLValue());
	setAssignable(inner && inner->isAssignable());
}

void FuncCall::sem(SemContext& context) {
	auto lookup = context.lookupSymbol(name);
	Symbol* symbol = lookup.symbol;
	auto* funcSym = (symbol && symbol->getKind() == Symbol::SymKind::FUNC)
		? static_cast<FuncSymbol*>(symbol)
		: nullptr;
	if (!funcSym || funcSym->isProcedure()) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "unknown function '" + name + "'");
		setSymbol(nullptr);
		setType(nullptr);
		return;
	}
	setSymbol(funcSym);
	const auto& params = funcSym->getParams();
	checkArguments(args, params, context, name, loc);
	const auto* sig = static_cast<const FuncType*>(funcSym->getType().get());
	setType(sig ? sig->returnType() : SemaTypePtr{});
	setLValue(false);
	setAssignable(false);
}

void UnaryExpr::sem(SemContext& context) {
	if (operand) {
		operand->sem(context);
	}
	auto operandType = operand ? operand->type() : SemaTypePtr{};
	switch (op) {
		case UnOp::Plus:
		case UnOp::Minus:
			if (!isIntType(operandType)) {
				context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						   loc, "unary '+' and '-' require int operand");
			}
			setType(makeIntType());
			break;
		case UnOp::Not:
			if (!isByteType(operandType)) {
				context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						   loc, "'!' requires byte operand");
			}
			setType(makeByteType());
			break;
	}
	setLValue(false);
	setAssignable(false);
}

void BinaryExpr::sem(SemContext& context) {
	if (lhs) {
		lhs->sem(context);
	}
	if (rhs) {
		rhs->sem(context);
	}
	auto leftType = lhs ? lhs->type() : SemaTypePtr{};
	auto rightType = rhs ? rhs->type() : SemaTypePtr{};
	switch (op) {
		case BinOp::Add:
		case BinOp::Sub:
		case BinOp::Mul:
		case BinOp::Div:
		case BinOp::Mod:
			if (!typesEqual(leftType, rightType) ||
				!(isIntType(leftType) || isByteType(leftType))) {
				context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						   loc, "arithmetic operands must both be int or byte");
			}
			setType(leftType);
			break;
		case BinOp::AndBits:
		case BinOp::OrBits:
			if (!isByteType(leftType) || !typesEqual(leftType, rightType)) {
				context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						   loc, "'&' and '|' require byte operands");
			}
			setType(makeByteType());
			break;
	}
	setLValue(false);
	setAssignable(false);
}

void Cond::sem(SemContext&) {}

void ExprCond::sem(SemContext& context) {
	if (expr) {
		expr->sem(context);
	}
	if (!isByteType(expr ? expr->type() : SemaTypePtr{})) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, loc, "condition expression must have type byte");
	}
	setType(makeByteType());
}

void ParenCond::sem(SemContext& context) {
	if (condition) {
		condition->sem(context);
	}
	setType(makeByteType());
}

void NotCond::sem(SemContext& context) {
	if (condition) {
		condition->sem(context);
	}
	setType(makeByteType());
}

void BinaryCond::sem(SemContext& context) {
	if (lhs) {
		lhs->sem(context);
	}
	if (rhs) {
		rhs->sem(context);
	}
	setType(makeByteType());
}

void RelCond::sem(SemContext& context) {
	if (lhs) {
		lhs->sem(context);
	}
	if (rhs) {
		rhs->sem(context);
	}
	auto lt = lhs ? lhs->type() : SemaTypePtr{};
	auto rt = rhs ? rhs->type() : SemaTypePtr{};
	if (!typesEqual(lt, rt) || !(isIntType(lt) || isByteType(lt))) {
		context.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic, 
						 loc, "comparison requires operands of the same numeric type");
	}
	setType(makeByteType());
}
