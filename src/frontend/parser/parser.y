%skeleton "lalr1.cc"
%require "3.8"
%define api.namespace {dana}
%define api.value.type {variant}
%define parse.error detailed
%locations
%start program

%code requires {
	#include <memory>
	#include <string>
	#include <vector>
	#include <utility>

	/* Forward decls for AST */ 
	struct Program; 

	struct Header;

	struct Decl; struct FuncDecl; struct FuncDef; struct VarDef;

	struct Stmt; struct Block; struct AssignStmt; struct CallStmt;
	struct IfStmt; struct LoopStmt; struct BreakStmt; struct ContinueStmt; struct ReturnStmt; struct SkipStmt;

	struct Expr; 
	struct IntConst; struct CharConst; struct BoolConst; struct UnaryExpr; struct BinaryExpr; 
	struct FuncCallExpr; struct LValueExpr;

	struct LValue; struct IdLValue; struct IndexLValue; struct StringLiteralLValue;

	struct Cond; struct NotCond; struct AndCond; struct OrCond; struct CompareCond; struct ExprCond;

	struct TypeNode; struct DataTypeNode; struct ArrayTypeNode; struct FParamType; struct FParamDefNode;

	/* Handy aliases */
	template<class T> using up  = std::unique_ptr<T>;
	template<class T> using vec = std::vector<std::unique_ptr<T>>;

	/* elif pair type (cond, block) for storing elifs */
	using ElifPair = std::pair<up<Cond>, up<Block>>;

	/* yylex signature (TODO: connect this with lexer later) */
	namespace dana { class parser; } 
	int yylex(dana::parser::semantic_type* yylval,
			dana::parser::location_type* yylloc);
}

%code {
	#include "ast.hpp"
	#include <iostream>

	void dana::parser::error (const location_type& loc, const std::string& msg) {
		std::cerr << loc.begin.line << ':' << loc.begin.column << ": " << msg << '\n';
	}
}

/* Deprecated: older C union approach
%union {
	// token types
	char* str; 	// strings
	int   num; 	// integers
	char  ch; 	// characters

// General AST node object
	ASTNode*		node;
	// AST types (presented using a bottom-up logic)
	Stmt*			stmt;
	Expr*			expr;
	Cond*			cond;
	Block*			blk;
	LocalDef*		localdef;
	FParamDef*		fparamdef;
	LVal*			lval;
	Type*			type;
	FParamType*		fparamtype;
	DataType*		datatype;
	Header*			header;
	Program*		Program;
}
*/

/* KEYWORDS */
%token T_AND T_AS T_BEGIN T_BREAK T_BYTE T_CONT T_DECL
%token T_DEF T_ELIF T_ELSE T_END T_EXIT T_FALSE T_IF
%token T_IS T_INT T_LOOP T_NOT T_OR T_REF T_RET
%token T_SKIP T_TRUE T_VAR

/*
%token <str>	T_ID				// general identifier: var names, function names etc.
%token <num>	T_INT_CONST			// number (eg. 42). Treated as a constant integer
%token <ch>		T_CHAR_CONST		// character constant
%token <str>	T_STRING_CONST		// String (eg. "abc"). Treated as a constant string
*/

%token <int> T_INT_CONST
%token <char> T_CHAR_CONST
%token <std::string> T_STRING_CONST T_ID

/* Multi-character operators */
%token T_ASSIGN 		// :=
%token T_NE 			// <>
%token T_LE				// <=
%token T_GE				// >=

/* Special layout token */
%token T_AUTO_END

/* Single character operator tokens are using their ASCII CODE as token code */

/* Associate semantic types with non terminal symbols */
%type <program>   		program
%type <localdef>		func_def func_decl var_def local_def
%type <header>    		header
%type <fparamdef>    	fpar_def
%type <fparamtype>    	fpar_type
%type <datatype>     	data_type
%type <type>      		type
%type <stmt>      		stmt if_stmt loop_stmt break_stmt continue_stmt return_stmt assign_stmt proc_call
%type <blk>     		block
%type <lval>      		l_value
%type <expr>      		expr func_call
%type <cond>			cond	

/* Associate list union types with non terminal symbols (to express * and + grammar symbols) */
%type <stmt_list>		stmt_list elif_list
%type <expr_list>		expr_list arg_list
%type <fparamdef_list>	fpar_list
%type <localdef_list>	localdef_list
%type <id_list>			id_list

/* Define operator precedence */
%left T_OR
%left T_AND
%right T_NOT
%nonassoc '=' T_NE '<' '>' T_LE T_GE
%left '|'
%left '&'
%left '+' '-'
%left '*' '/' '%'
%right UMINUS UPLUS

%%

program
	: func_def								
	;

func_def
	: T_DEF header localdef_list block		
	;

localdef_list
	: 
	| local_def localdef_list
	;

header
	: T_ID opt_ret_type opt_params
	;

opt_ret_type
	: 									
	| T_IS data_type						
	;

opt_params
	:										
	| ':' fpar_list							
	;

fpar_list
	: fpar_def									
	| fpar_def ',' fpar_list				
	;

fpar_def
	: id_list T_AS fpar_type
	;

id_list
	: T_ID
	| T_ID id_list
	;

fpar_type
	: type
	| T_REF data_type 
	| data_type '[' ']' idx
	;

type
	: data_type idx
	;

idx
	: 
	| '[' T_INT_CONST ']' idx
	;

data_type
	: T_INT
	| T_BYTE
	;

local_def
	: func_def 
	| func_decl 
	| var_def
	;

func_decl
	: T_DECL header
	;

var_def
	: T_VAR id_list T_IS type
	;

stmt
	: T_SKIP
	| assign_stmt
	| proc_call
	| T_EXIT
	| return_stmt
	| if_stmt
	| loop_stmt
	| break_stmt
	| continue_stmt
	;

assign_stmt
	: l_value T_ASSIGN expr
	;

return_stmt
	: T_RET ':' expr
	;

if_stmt
	: T_IF cond ':' block elif_list opt_else
	;

elif_list
	: 
	| T_ELIF cond ':' block elif_list
	;

opt_else
	: 
	| T_ELSE ':' block
	;

loop_stmt
	: T_LOOP opt_id ':' block
	;

opt_id
	: 
	| T_ID
	;

break_stmt
	: T_BREAK
	| T_BREAK ':' T_ID
	;

continue_stmt
	: T_CONTINUE
	| T_CONTINUE ':' T_ID
	;

block
	: T_BEGIN stmt_list T_END
	| stmt_list T_AUTO_END
	;

stmt_list
	: stmt
	| stmt stmt_list
	;

proc_call
	: T_ID
	| T_ID ':' expr_list
	;

expr_list
	: expr
	| expr ',' expr_list
	;

func_call
	: T_ID '(' ')'
	| T_ID '(' expr_list ')'
	;

l_value
	: T_ID
	| T_STRING_CONST
	| l_value '[' expr ']'
	;

expr
	: T_INT_CONST
	| T_CHAR_CONST
	| l_value
	| '(' expr ')'
	| func_call
	| UPLUS expr
	| UMINUS expr
	| expr '+' expr
	| expr '-' expr
	| expr '*' expr
	| expr '/' expr
	| expr '%' expr
	| T_TRUE 
	| T_FALSE
	| '!' expr
	| expr '&' expr
	| expr '|' expr
	;

cond
	: expr
	| '(' cond ')'
	| T_NOT cond
	| cond T_AND cond
	| cond T_OR cond
	| expr '=' expr
	| expr T_NE expr
	| expr T_LE expr
	| expr T_GE expr
	| expr '<' expr
	| expr '>' expr
	;
