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
	#include <optional>
	#include "ast.hpp"

	/* Handy aliases */
	using std::move;
	using std::make_unique;
	using std::string;
	using std::optional;
	template<class T> using up  = std::unique_ptr<T>;
	template<class T> using vec = std::vector<T>;

	/* elif pair type (cond, block) for storing elifs */
	using ElifPair = std::pair<up<Cond>, up<Block>>;

	/* yylex signature (TODO: connect this with lexer later) */
	namespace dana { class parser; } 
	extern int yylex(dana::parser::semantic_type& yylval,
					 dana::parser::location_type& yylloc);

	// Convert Bison location -> SourceLoc (use begin point)
  	static inline SourceLoc mkLoc(const dana::parser::location_type& L) {
    	return SourceLoc{ (int)L.begin.line, (int)L.begin.column };
  	}
}

%code {
	#include <iostream>

	/* TODO: Check parse error logging function */
	void dana::parser::error (const location_type& loc, const std::string& msg) {
		SourceLoc here{ (int)loc.begin.line, (int)loc.begin.column };
		std::cerr << here.line << ':' << here.column << ": " << msg << '\n';

		driver.last_error_loc = here;
	}
}

/* KEYWORDS */
%token T_AND T_AS T_BEGIN T_BREAK T_BYTE T_CONT T_DECL
%token T_DEF T_ELIF T_ELSE T_END T_EXIT T_FALSE T_IF
%token T_IS T_INT T_LOOP T_NOT T_OR T_REF T_RET
%token T_SKIP T_TRUE T_VAR

%token <int> T_INT_CONST
%token <char> T_CHAR_CONST
%token <string> T_STRING_CONST T_ID

/* Multi-character operators */
%token T_ASSIGN 		// :=
%token T_NE 			// <>
%token T_LE				// <=
%token T_GE				// >=

/* Special layout token */
%token T_AUTO_END

/* Single character operator tokens are using their ASCII CODE as token code */

/* Associate semantic types with non terminal symbols */
%type <up<Program>>							program
%type <up<FuncDef>>							func_def
%type <up<VarDef>>							var_def
%type <up<Def>>								local_def
%type <vec<up<Def>>>						localdef_list
%type <up<FuncDecl>>						func_decl
%type <up<Header>>  						header
%type <up<FParDef>>    						fpar_def
%type <vec<up<FParDef>>>					fpar_list
%type <up<FParType>>    					fpar_type
%type <up<Type>>      						type
%type <up<Stmt>>      						stmt if_stmt loop_stmt break_stmt continue_stmt return_stmt assign_stmt proc_call
%type <vec<up<Stmt>>>						stmt_list
%type <vec<std::pair<up<Cond>, up<Block>>>> elif_list
%type <up<Block>>     						block
%type <up<Lval>>      						l_value	
%type <up<Expr>>      						expr func_call
%type <vec<up<Expr>>>						expr_list
%type <up<Cond>>							cond	
%type <vec<string>>							id_list
/* these non terminals are optionals (Kleene's star in the grammar) */
%type <optional<DataType>>  				opt_ret_type // optional function return type
%type <vec<up<FParDef>>>    				opt_params // optional function parameters
%type <vec<optional<int>>> 					fpar_dims	// optional array dimensions list as function parameter
%type <vec<optional<int>>					type_dims	// optional array dimensions list in type definition
%type <optional<int>> 						dim_opt // a single, optional array dimension as parameter
%type <optional<up<Block>>>					opt_else // optional Else block
%type <optional<string>> 					opt_id	// optional identifier

/* Define operator precedence */
%left '|' T_OR
%left '&' T_AND
%nonassoc '=' T_NE '<' '>' T_LE T_GE
%left '+' '-'
%left '*' '/' '%'
%right UPLUS UMINUS '!' T_NOT

%%

program
	: func_def									{ $$ = make_unique<Program>(mkLoc(@$), move($1)); }			
	;

func_def
	: T_DEF header localdef_list block			{ $$ = make_unique<FuncDef>(mkLoc(@$), move($2), move($3), move($4)); }
	;

localdef_list
	: 											{ $$ = vec<up<Def>>{}; }
	| localdef_list local_def					{ $1.emplace_back(move($2)); $$ = move($1); }
	;

local_def
	: func_def									{ $$ = move($1); } 
	| func_decl 			                    { $$ = move($1); } 			
	| var_def									{ $$ = move($1); } 
	;

func_decl
	: T_DECL header								{ $$ = make_unique<FuncDecl>(mkLoc(@$), move($2)); }	
	;

var_def
	: T_VAR id_list T_IS type					{ $$ = make_unique<VarDef>(mkLoc(@$), move($2), move($4)); }
	;

type
	: T_INT  type_dims 							{ $$ = make_unique<Type>(mkLoc(@$), DataType::Int,  move($2)); }
	| T_BYTE type_dims 							{ $$ = make_unique<Type>(mkLoc(@$), DataType::Byte, move($2)); }
	;

type_dims
	:                             				{ $$ = vec<std::optional<int>>{}; }
	| type_dims '[' T_INT_CONST ']' 			{ $1.emplace_back($3); $$ = move($1); }
	;

header
	: T_ID opt_ret_type opt_params				{ $$ = make_unique<Header>(mkLoc(@$), move($1), $2, move($3)); }
	;

opt_ret_type
	:                							{ $$ = optional<DataType>{}; }
	| T_IS T_INT     							{ $$ = DataType::Int; }
	| T_IS T_BYTE    							{ $$ = DataType::Byte; }
	;

opt_params
	:											{ $$ = vec<up<FParDef>>{}; }
	| ':' fpar_list								{ $$ = move($2); }
	;

fpar_list
	: fpar_def               					{ vec<up<FParDef>> v; v.emplace_back(move($1)); $$ = move(v); }
	| fpar_list ',' fpar_def 					{ $1.emplace_back(move($3)); $$ = move($1); }
	;

fpar_def
	: id_list T_AS fpar_type 					{ $$ = make_unique<FParDef>(mkLoc(@$), move($1), move($3)); }
	;

id_list
	: T_ID         								{ vec<string> v; v.emplace_back(move($1)); $$ = move(v); }
	| id_list T_ID 								{ $1.emplace_back(move($2)); $$ = move($1); }
	;

fpar_type
	: T_INT {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */false, DataType::Int);
	  }
	| T_BYTE {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */false, DataType::Byte);
	  }
	| T_INT fpar_dims {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::Int, move($2)); 
	  }
	| T_BYTE fpar_dims {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::Byte, move($2));
	  }
	| T_REF T_INT {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::Int); 
	  }
	| T_REF T_BYTE {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::Byte); 
	  }
	;

fpar_dims
	: type_dims									{ $$ = move($1); }
	| '[' ']' type_dims							{ auto v = move($3); v.insert(v.begin(), optional<int>{}); $$ = move(v); }
	;

block
	: T_BEGIN stmt_list T_END   				{ $$ = make_unique<Block>(mkLoc(@$), move($2) ); }
	| stmt_list T_AUTO_END      				{ $$ = make_unique<Block>(mkLoc(@$), move($1) ); }
	;

stmt_list
	: stmt                   					{ vec<up<Stmt>> v; v.emplace_back(move($1)); $$ = move(v); }
	| stmt_list stmt            				{ $1.emplace_back(move($2)); $$ = move($1); }
	;

stmt
	: T_SKIP                    				{ $$ = make_unique<SkipStmt>(mkLoc(@$)); }
	| assign_stmt               				{ $$ = move($1); }
	| proc_call                 				{ $$ = move($1); }
	| T_EXIT                    				{ $$ = make_unique<ExitStmt>(mkLoc(@$)); }
	| return_stmt               				{ $$ = move($1); }
	| if_stmt                   				{ $$ = move($1); }
	| loop_stmt                 				{ $$ = move($1); }
	| break_stmt                				{ $$ = move($1); }
	| continue_stmt             				{ $$ = move($1); }
	;

assign_stmt
	: l_value T_ASSIGN expr 					{ $$ = make_unique<AssignStmt>(mkLoc(@$), move($1), move($3)); }
	;

return_stmt
	: T_RET ':' expr 							{ $$ = make_unique<ReturnStmt>(mkLoc(@$), move($3)); }
	;

proc_call
	: T_ID 										{ $$ = make_unique<ProcCall>(mkLoc(@$), move($1), vec<up<Expr>>{}); }
	| T_ID ':' expr_list 						{ $$ = make_unique<ProcCall>(mkLoc(@$), move($1), move($3)); }
	;

break_stmt
	: T_BREAK              						{ $$ = make_unique<BreakStmt>(mkLoc(@$), optional<string>{}); }
	| T_BREAK ':' T_ID     						{ $$ = make_unique<BreakStmt>(mkLoc(@$), optional<string>{move($3)}); }
	;

continue_stmt
	: T_CONT               						{ $$ = make_unique<ContinueStmt>(mkLoc(@$), optional<string>{}); }
	| T_CONT ':' T_ID      						{ $$ = make_unique<ContinueStmt>(mkLoc(@$), optional<string>{move($3)}); }
	;

if_stmt
	: T_IF cond ':' block elif_list opt_else 	{ $$ = make_unique<IfStmt>(mkLoc(@$), move($2), move($4), move($5), move($6)); }
	;

elif_list
	:                         					{ $$ = vec<std::pair<up<Cond>, up<Block>>>{}; }
	| elif_list T_ELIF cond ':' block 			{ $1.emplace_back(std::make_pair(move($3), move($5))); $$ = move($1); }
	;

opt_else
	:                         					{ $$ = optional<up<Block>>{}; }
	| T_ELSE ':' block        					{ $$ = optional<up<Block>>{ move($3) }; }
	;

loop_stmt
	: T_LOOP opt_id ':' block 					{ $$ = make_unique<LoopStmt>(mkLoc(@$), move($2), move($4)); }
	;

opt_id
	:           								{ $$ = optional<string>{}; }
	| T_ID      								{ $$ = optional<string>{ move($1) }; }
	;

expr_list
	: expr										{ vec<up<Expr>> v; v.emplace_back(move($1)); $$ = move(v); }				
	| expr_list ',' expr						{ $1.emplace_back(move($3)); $$ = move($1); }
	;

func_call
	: T_ID '(' ')'								{ $$ = make_unique<FuncCall>(mkLoc(@$), move($1), vec<up<Expr>>{}); }
	| T_ID '(' expr_list ')'					{ $$ = make_unique<FuncCall>(mkLoc(@$), move($1), move($3)); }
	;

l_value
	: T_ID                  					{ $$ = make_unique<IdLVal>(mkLoc(@$), move($1) ); }
	| T_STRING_CONST        					{ $$ = make_unique<StringLiteralLVal>(mkLoc(@$), move($1) ); }
	| l_value '[' expr ']'  					{ $$ = make_unique<IndexLVal>(mkLoc(@$), move($1), move($3) ); }
	;

expr
	: T_INT_CONST           					{ $$ = make_unique<IntConst>(mkLoc(@$), $1); }
	| T_CHAR_CONST          					{ $$ = make_unique<CharConst>(mkLoc(@$), (unsigned char)$1); }
	| T_TRUE                					{ $$ = make_unique<TrueConst>(mkLoc(@$)); }
	| T_FALSE               					{ $$ = make_unique<FalseConst>(mkLoc(@$)); }
	| l_value               					{ $$ = make_unique<LValueExpr>(mkLoc(@$), move($1)); }
	| '(' expr ')'          					{ $$ = make_unique<ParenExpr>(mkLoc(@$), move($2)); }
	| func_call             					{ $$ = move($1); }
	| '+' expr %prec UPLUS  					{ $$ = make_unique<UnaryExpr>(mkLoc(@$), UnOp::Plus,  move($2)); }
	| '-' expr %prec UMINUS 					{ $$ = make_unique<UnaryExpr>(mkLoc(@$), UnOp::Minus, move($2)); }
	| '!' expr              					{ $$ = make_unique<UnaryExpr>(mkLoc(@$), UnOp::Not,  move($2)); }
	| expr '+' expr         					{ $$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Add, move($1), move($3)); }
	| expr '-' expr         					{ $$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Sub, move($1), move($3)); }
	| expr '*' expr         					{ $$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Mul, move($1), move($3)); }
	| expr '/' expr         					{ $$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Div, move($1), move($3)); }
	| expr '%' expr         					{ $$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Mod, move($1), move($3)); }
	| expr '&' expr         					{ $$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::AndBits, move($1), move($3)); }
	| expr '|' expr         					{ $$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::OrBits,  move($1), move($3)); }
	;

cond
	: expr                  { $$ = make_unique<ExprCond>(mkLoc(@$), move($1)); }
	| '(' cond ')'          { $$ = make_unique<ParenCond>(mkLoc(@$), move($2)); }
	| T_NOT cond            { $$ = make_unique<NotCond>(mkLoc(@$), move($2)); }
	| cond T_AND cond       { $$ = make_unique<BinaryCond>(mkLoc(@$), LogicOp::And, move($1), move($3)); }
	| cond T_OR  cond       { $$ = make_unique<BinaryCond>(mkLoc(@$), LogicOp::Or,  move($1), move($3)); }
	| expr '='  expr        { $$ = make_unique<RelCond>(mkLoc(@$), RelOp::Eq, move($1), move($3)); }
	| expr T_NE expr        { $$ = make_unique<RelCond>(mkLoc(@$), RelOp::Ne, move($1), move($3)); }
	| expr T_LE expr        { $$ = make_unique<RelCond>(mkLoc(@$), RelOp::Le, move($1), move($3)); }
	| expr T_GE expr        { $$ = make_unique<RelCond>(mkLoc(@$), RelOp::Ge, move($1), move($3)); }
	| expr '<' expr         { $$ = make_unique<RelCond>(mkLoc(@$), RelOp::Lt, move($1), move($3)); }
	| expr '>' expr         { $$ = make_unique<RelCond>(mkLoc(@$), RelOp::Gt, move($1), move($3)); }
	;
