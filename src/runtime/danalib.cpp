#include "../frontend/semantic/sema_context.hpp"
#include "../backend/codegen/codegen_context.hpp"
#include "danalib.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

static void declareBuiltins(SemContext& ctx) {
	ctx.openScope();
	SemContext::HeaderInfo info;
	SemContext::ParamInfo param;
	info.loc = SourceLoc::builtin();
	param.loc = SourceLoc::builtin();

	for (int i = 0; i < sizeof(builtinTable) / sizeof(DanaBuiltin); ++i) {
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
		
		FuncSymbol* funcSym = ctx.makeFunctionSymbol(info);
		ctx.declareSymbol(std::unique_ptr<Symbol>(funcSym), /*reportDuplicates=*/false);

	}
}
