#pragma once

struct DanaBuiltin {
    const char* runtimeName;
    const char* danaName;
};

inline constexpr DanaBuiltin builtinTable[] = {
    {"dana_writeInteger", "writeInteger"},
    {"dana_writeByte", "writeByte"},
    {"dana_writeChar", "writeChar"},
    {"dana_writeString", "writeString"},
    {"dana_readInteger", "readInteger"},
    {"dana_readByte", "readByte"},
    {"dana_readChar", "readChar"},
    {"dana_readString", "readString"},
    {"dana_extend", "extend"},
    {"dana_shrink", "shrink"},
	{"dana_strlen", "strlen"},
	{"dana_strcpy", "strcpy"},
	{"dana_strcat", "strcat"},
	{"dana_strcmp", "strcmp"}
};

class CodegenContext;
class SemContext;

static void declareBuiltins(SemContext& semCtx);
static void genBuiltins(CodegenContext& codegenCtx);
