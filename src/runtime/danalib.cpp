#include "../frontend/semantic/sema_context.hpp"
#include "../backend/codegen/codegen_context.hpp"
#include "danalib.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

namespace {

// Local helper structs for builtin declaration
struct ParamInfo {
	std::string name;
	SourceLoc loc = SourceLoc::builtin();
	SemaTypePtr type;
	Symbol::ParamPass passMode = Symbol::ParamPass::BY_VAL;
};

struct HeaderInfo {
	std::string name;
	SourceLoc loc = SourceLoc::builtin();
	bool isProcedure = false;
	SemaTypePtr returnType;
	std::vector<ParamInfo> params;
};

} // namespace

void declareBuiltins(SemContext& ctx) {
	HeaderInfo info;
	ParamInfo param;

	for (std::size_t i = 0; i < sizeof(builtinTable) / sizeof(DanaBuiltin); ++i) {
		switch (i) {
			case 0: // writeInteger
				{
					info.params.clear();
					param.type = makeIntType();
					param.name = "^@num";
					param.passMode = Symbol::ParamPass::BY_VAL;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = true;
					info.returnType = makeVoidType();
					break;
				}
			case 1: // writeByte
				{
					info.params.clear();
					param.type = makeByteType();
					param.name = "^@byte";
					param.passMode = Symbol::ParamPass::BY_VAL;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = true;
					info.returnType = makeVoidType();
					break;
				}
			case 2: // writeChar
				{
					info.params.clear();
					param.type = makeByteType();
					param.name = "^@char";
					param.passMode = Symbol::ParamPass::BY_VAL;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = true;
					info.returnType = makeVoidType();
					break;
				}
			case 3: // writeString
				{	
					info.params.clear();
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.name = "^@str";
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = true;
					info.returnType = makeVoidType();
					break;
				}
			case 4: // readInteger
				{
					info.params.clear();
					info.name = builtinTable[i].danaName;
					info.isProcedure = false;
					info.returnType = makeIntType();
					break;
				}
			case 5: // readByte
				{
					info.params.clear();
					info.name = builtinTable[i].danaName;
					info.isProcedure = false;
					info.returnType = makeByteType();
					break;
				}
			case 6: // readChar
				{
					info.params.clear();
					info.name = builtinTable[i].danaName;
					info.isProcedure = false;
					info.returnType = makeByteType();
					break;
				}
			case 7: // readString
				{
					info.params.clear();
					param.name = "^@maxLen";
					param.type = makeIntType();
					param.passMode = Symbol::ParamPass::BY_VAL;
					info.params.push_back(param);
					param.passMode = Symbol::ParamPass::BY_REF;
					param.name = "^@buf";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = true;
					info.returnType = makeVoidType();
					break;
				}
			case 8: // extend
				{
					info.params.clear();
					param.name = "^@b";
					param.passMode = Symbol::ParamPass::BY_VAL;
					param.type = makeByteType();
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = false;
					info.returnType = makeIntType();
					break;
				}
			case 9: // shrink
				{
					info.params.clear();
					param.name = "^@i";
					param.type = makeIntType();
					param.passMode = Symbol::ParamPass::BY_VAL;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = false;
					info.returnType = makeByteType();
					break;
				}
			case 10: // strlen
				{
					info.params.clear();
					param.name = "^@str";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = false;
					info.returnType = makeIntType();
					break;
				}	
			case 11: // strcpy
				{
					info.params.clear();
					param.name = "^@dest";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					param.name = "^@src";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = true;
					break;
				}
			case 12: // strcat
				{
					info.params.clear();
					param.name = "^@dest";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					param.name = "^@src";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = true;
					break;
				}
			case 13: // strcmp
				{
					info.params.clear();
					param.name = "^@str1";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					param.name = "^@str2";
					param.type = makeArrayType(makeByteType(), std::nullopt);
					param.passMode = Symbol::ParamPass::BY_REF;
					info.params.push_back(param);
					info.name = builtinTable[i].danaName;
					info.isProcedure = false;
					info.returnType = makeIntType();
					break;
				}
			default:
				break;
		}
		
		std::vector<SemaTypePtr> paramTypes;
		paramTypes.reserve(info.params.size());
		for (const auto& p : info.params) {
			paramTypes.push_back(p.type);
		}
		auto sig = makeFuncType(info.returnType, std::move(paramTypes));
		auto func = std::make_unique<FuncSymbol>(info.name, std::move(sig), info.isProcedure, info.loc);

		ctx.openScope();
		for (const auto& pInfo : info.params) {
			auto p = std::make_unique<ParamSymbol>(pInfo.name, pInfo.type, pInfo.passMode, pInfo.loc);
			p->setDefiningFunc(func.get());
			auto res = ctx.declareSymbol(std::move(p));
			if (res.symbol) {
				func->addParam(static_cast<ParamSymbol*>(res.symbol));
			}
		}
		ctx.closeScope();

		ctx.declareSymbol(std::move(func), /*reportDuplicates=*/false);
	}
}

void genBuiltins(SemContext& semCtx, CodegenContext& ctx) {
	auto& mod   = ctx.llvmModule();
	auto& llctx = ctx.llvmContext();

	auto* i32Ty = llvm::Type::getInt32Ty(llctx);
	auto* i8Ty  = llvm::Type::getInt8Ty(llctx);
	auto* ptrTy = llvm::PointerType::get(llctx, 0);

	auto bind = [&](const char* danaName, llvm::Function* fn) {
		if (!fn) return;
		auto res = semCtx.lookupSymbol(danaName);
		if (res.symbol && res.symbol->getKind() == Symbol::SymKind::FUNC) {
			ctx.bindFunction(static_cast<FuncSymbol*>(res.symbol), fn);
		}
	};

	llvm::FunctionType* writeIntTy  = llvm::FunctionType::get(llvm::Type::getVoidTy(llctx), {i32Ty}, false);
	llvm::FunctionType* writeByteTy = llvm::FunctionType::get(llvm::Type::getVoidTy(llctx), {i8Ty}, false);
	llvm::FunctionType* writeCharTy = llvm::FunctionType::get(llvm::Type::getVoidTy(llctx), {i8Ty}, false);
	llvm::FunctionType* writeStrTy  = llvm::FunctionType::get(llvm::Type::getVoidTy(llctx), {ptrTy}, false);

	llvm::FunctionType* readIntTy   = llvm::FunctionType::get(i32Ty, {}, false);
	llvm::FunctionType* readByteTy  = llvm::FunctionType::get(i8Ty, {}, false);
	llvm::FunctionType* readCharTy  = llvm::FunctionType::get(i8Ty, {}, false);

	bind("writeInteger", llvm::Function::Create(writeIntTy,  llvm::Function::ExternalLinkage, builtinTable[0].runtimeName, &mod));
	bind("writeByte",    llvm::Function::Create(writeByteTy, llvm::Function::ExternalLinkage, builtinTable[1].runtimeName, &mod));
	bind("writeChar",    llvm::Function::Create(writeCharTy, llvm::Function::ExternalLinkage, builtinTable[2].runtimeName, &mod));
	bind("writeString",  llvm::Function::Create(writeStrTy,  llvm::Function::ExternalLinkage, builtinTable[3].runtimeName, &mod));

	bind("readInteger", llvm::Function::Create(readIntTy,   llvm::Function::ExternalLinkage, builtinTable[4].runtimeName, &mod));
	bind("readByte",    llvm::Function::Create(readByteTy,  llvm::Function::ExternalLinkage, builtinTable[5].runtimeName, &mod));
	bind("readChar",    llvm::Function::Create(readCharTy,  llvm::Function::ExternalLinkage, builtinTable[6].runtimeName, &mod));

	llvm::FunctionType* readStrTy = llvm::FunctionType::get(llvm::Type::getVoidTy(llctx), {i32Ty, ptrTy}, false);
	bind("readString", llvm::Function::Create(readStrTy, llvm::Function::ExternalLinkage, builtinTable[7].runtimeName, &mod));

	llvm::FunctionType* extendTy = llvm::FunctionType::get(i32Ty, {i8Ty}, false);
	bind("extend", llvm::Function::Create(extendTy, llvm::Function::ExternalLinkage, builtinTable[8].runtimeName, &mod));

	llvm::FunctionType* shrinkTy = llvm::FunctionType::get(i8Ty, {i32Ty}, false);
	bind("shrink", llvm::Function::Create(shrinkTy, llvm::Function::ExternalLinkage, builtinTable[9].runtimeName, &mod));

	llvm::FunctionType* strlenTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
	bind("strlen", llvm::Function::Create(strlenTy, llvm::Function::ExternalLinkage, builtinTable[10].runtimeName, &mod));

	llvm::FunctionType* strcpyTy = llvm::FunctionType::get(llvm::Type::getVoidTy(llctx), {ptrTy, ptrTy}, false);
	bind("strcpy", llvm::Function::Create(strcpyTy, llvm::Function::ExternalLinkage, builtinTable[11].runtimeName, &mod));

	llvm::FunctionType* strcatTy = llvm::FunctionType::get(llvm::Type::getVoidTy(llctx), {ptrTy, ptrTy}, false);
	bind("strcat", llvm::Function::Create(strcatTy, llvm::Function::ExternalLinkage, builtinTable[12].runtimeName, &mod));

	llvm::FunctionType* strcmpTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
	bind("strcmp", llvm::Function::Create(strcmpTy, llvm::Function::ExternalLinkage, builtinTable[13].runtimeName, &mod));
}
