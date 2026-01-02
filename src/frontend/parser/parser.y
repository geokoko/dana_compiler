%skeleton "lalr1.cc"
%require "3.8"
%define api.namespace {dana}
%define api.value.type variant
%define parse.error detailed
%locations
%start program

/* Pass the AST root back to the caller */
%parse-param { up<Program>& ast_root }

%code requires {
	#include <memory>
	#include <string>
	#include <vector>
	#include <utility>
	#include <optional>

	#include "../ast/ast.hpp"

	/* Handy aliases */
	using std::make_unique;
	using std::string;
	using std::optional;
	template<class T> using up  = std::unique_ptr<T>;
	template<class T> using vec = std::vector<T>;

	/* elif pair type (cond, block) for storing elifs */
	using ElifPair = std::pair<up<Cond>, up<Block>>;

	/* forward decl for the parser class and yylex */
	namespace dana { class parser; } 
}

%code {
	#include <iostream>

	extern int yylex(dana::parser::semantic_type* yylval, dana::parser::location_type* yylloc);

	// Convert Bison location -> SourceLoc (use begin point)
  	static inline SourceLoc mkLoc(const dana::parser::location_type& L) {
    	return SourceLoc{ (int)L.begin.line, (int)L.begin.column };
	}

	static void __attribute__((unused)) yyerror (const dana::parser::location_type& loc, const std::string& msg) {
		SourceLoc here{ (int)loc.begin.line, (int)loc.begin.column };
		std::cerr << here.line << ':' << here.col << ": " << msg << '\n';
	}

	// Bison C++ interface expects this method on the parser class
	void dana::parser::error(const dana::parser::location_type& loc, const std::string& msg) {
		SourceLoc here{ (int)loc.begin.line, (int)loc.begin.column };
		std::cerr << here.line << ':' << here.col << ": " << msg << '\n';
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
%type <vec<optional<int>>>					type_dims	// optional array dimensions list in type definition
%type <optional<up<Block>>>					opt_else // optional Else block
%type <optional<string>> 					opt_id	// optional identifier

/* Define operator precedence */
%left  T_OR          			/* || */
%left  T_AND         			/* && */

%left  '|'           			/* bitwise OR */
%left  '&'           			/* bitwise AND */

%nonassoc '=' T_NE   		 	/* ==, <> */
%nonassoc '<' '>' T_LE T_GE  	/* <, >, <=, >= */

%left  '+' '-'
%left  '*' '/' '%'

%precedence UPLUS UMINUS '!' T_NOT

%%

program
	: func_def									{ 
		/* Build Program node and move it to the caller-owned ast_root */
		ast_root = make_unique<Program>(mkLoc(@$), std::move($1));
		/* Start symbol's semantic value is not used further */
		$$ = up<Program>{};
	}			
	;

func_def
	: T_DEF header localdef_list block			{ 
		$$ = make_unique<FuncDef>(mkLoc(@$), std::move($2), std::move($3), std::move($4)); 
	}
	;

localdef_list
	: %empty									{ $$ = vec<up<Def>>{}; }
	| localdef_list local_def					{ $1.emplace_back(std::move($2)); $$ = std::move($1); }
	;

local_def
	: func_def									{ $$ = std::move($1); } 
	| func_decl 			                    { $$ = std::move($1); } 			
	| var_def									{ $$ = std::move($1); } 
	;

func_decl
	: T_DECL header								{ $$ = make_unique<FuncDecl>(mkLoc(@$), std::move($2)); }	
	;

var_def
	: T_VAR id_list T_IS type					{ $$ = make_unique<VarDef>(mkLoc(@$), std::move($2), std::move($4)); }
	;

type
	: T_INT  type_dims 							{ $$ = make_unique<Type>(mkLoc(@$), DataType::INT,  std::move($2)); }
	| T_BYTE type_dims 							{ $$ = make_unique<Type>(mkLoc(@$), DataType::BYTE, std::move($2)); }
	;

type_dims
	: %empty									{ $$ = vec<optional<int>>{}; }
	| type_dims '[' T_INT_CONST ']'				{ $1.emplace_back($3); $$ = std::move($1); }
	;

header
	: T_ID opt_ret_type opt_params				{ $$ = make_unique<Header>(mkLoc(@$), std::move($1), $2, std::move($3)); }
	;

opt_ret_type
	: %empty									{ $$ = optional<DataType>{}; }
	| T_IS T_INT     							{ $$ = DataType::INT; }
	| T_IS T_BYTE    							{ $$ = DataType::BYTE; }
	;

opt_params
	: %empty									{ $$ = vec<up<FParDef>>{}; }
	| ':' fpar_list								{ $$ = std::move($2); }
	;

fpar_list
	: fpar_def               					{ vec<up<FParDef>> v; v.emplace_back(std::move($1)); $$ = std::move(v); }
	| fpar_list ',' fpar_def 					{ $1.emplace_back(std::move($3)); $$ = std::move($1); }
	;

fpar_def
	: id_list T_AS fpar_type 					{ $$ = make_unique<FParDef>(mkLoc(@$), std::move($1), std::move($3)); }
	;

id_list
	: T_ID         								{ vec<string> v; v.emplace_back(std::move($1)); $$ = std::move(v); }
	| id_list T_ID 								{ $1.emplace_back(std::move($2)); $$ = std::move($1); }
	;

fpar_type
	: T_INT {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */false, DataType::INT);
	  }
	| T_BYTE {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */false, DataType::BYTE);
	  }
	| T_INT fpar_dims {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::INT, std::move($2)); 
	  }
	| T_BYTE fpar_dims {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::BYTE, std::move($2));
	  }
	| T_REF T_INT {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::INT); 
	  }
	| T_REF T_BYTE {
		$$ = make_unique<FParType>(mkLoc(@$), /* by-ref= */true, DataType::BYTE); 
	  }
	;

fpar_dims
	: '[' T_INT_CONST ']' type_dims				{ 
		auto v = std::move($4); v.insert(v.begin(), $2); $$ = std::move(v); 
	}
	| '[' ']' type_dims							{ 
		auto v = std::move($3); v.insert(v.begin(), optional<int>{}); $$ = std::move(v); 
	}
	;

block
	: T_BEGIN stmt_list T_END   				{ $$ = make_unique<Block>(mkLoc(@$), std::move($2) ); }
	| stmt_list T_AUTO_END      				{ $$ = make_unique<Block>(mkLoc(@$), std::move($1) ); }
	;

stmt_list
	: stmt                   					{ vec<up<Stmt>> v; v.emplace_back(std::move($1)); $$ = std::move(v); }
	| stmt_list stmt            				{ $1.emplace_back(std::move($2)); $$ = std::move($1); }
	;

stmt
	: T_SKIP                    				{ $$ = make_unique<SkipStmt>(mkLoc(@$)); }
	| assign_stmt               				{ $$ = std::move($1); }
	| proc_call                 				{ $$ = std::move($1); }
	| T_EXIT                    				{ $$ = make_unique<ExitStmt>(mkLoc(@$)); }
	| return_stmt               				{ $$ = std::move($1); }
	| if_stmt                   				{ $$ = std::move($1); }
	| loop_stmt                 				{ $$ = std::move($1); }
	| break_stmt                				{ $$ = std::move($1); }
	| continue_stmt             				{ $$ = std::move($1); }
	;

assign_stmt
	: l_value T_ASSIGN expr 					{ $$ = make_unique<AssignStmt>(mkLoc(@$), std::move($1), std::move($3)); }
	;

return_stmt
	: T_RET ':' expr 							{ $$ = make_unique<ReturnStmt>(mkLoc(@$), std::move($3)); }
	;

proc_call
	: T_ID 										{ $$ = make_unique<ProcCall>(mkLoc(@$), std::move($1), vec<up<Expr>>{}); }
	| T_ID ':' expr_list 						{ $$ = make_unique<ProcCall>(mkLoc(@$), std::move($1), std::move($3)); }
	;

break_stmt
	: T_BREAK              						{ $$ = make_unique<BreakStmt>(mkLoc(@$), optional<string>{}); }
	| T_BREAK ':' T_ID     						{ $$ = make_unique<BreakStmt>(mkLoc(@$), optional<string>{std::move($3)}); }
	;

continue_stmt
	: T_CONT               						{ $$ = make_unique<ContinueStmt>(mkLoc(@$), optional<string>{}); }
	| T_CONT ':' T_ID      						{ $$ = make_unique<ContinueStmt>(mkLoc(@$), optional<string>{std::move($3)}); }
	;

if_stmt
	: T_IF cond ':' block elif_list opt_else 	{ 
		$$ = make_unique<IfStmt>(mkLoc(@$), std::move($2), std::move($4), std::move($5), std::move($6)); 
	}
	;

elif_list
	: %empty									{ $$ = vec<std::pair<up<Cond>, up<Block>>>{}; }
	| elif_list T_ELIF cond ':' block 			{ 
		$1.emplace_back(std::make_pair(std::move($3), std::move($5))); $$ = std::move($1); 
	}
	;

opt_else
	: %empty									{ $$ = optional<up<Block>>{}; }
	| T_ELSE ':' block        					{ $$ = optional<up<Block>>{ std::move($3) }; }
	;

loop_stmt
	: T_LOOP opt_id ':' block 					{ $$ = make_unique<LoopStmt>(mkLoc(@$), std::move($2), std::move($4)); }
	;

opt_id
	: %empty									{ $$ = optional<string>{}; }
	| T_ID      								{ $$ = optional<string>{ std::move($1) }; }
	;

expr_list
	: expr										{ vec<up<Expr>> v; v.emplace_back(std::move($1)); $$ = std::move(v); }
	| expr_list ',' expr						{ $1.emplace_back(std::move($3)); $$ = std::move($1); }
	;

func_call
	: T_ID '(' ')'								{ $$ = make_unique<FuncCall>(mkLoc(@$), std::move($1), vec<up<Expr>>{}); }
	| T_ID '(' expr_list ')'					{ $$ = make_unique<FuncCall>(mkLoc(@$), std::move($1), std::move($3)); }
	;

l_value
	: T_ID                  					{ $$ = make_unique<IdLVal>(mkLoc(@$), std::move($1) ); }
	| T_STRING_CONST        					{ $$ = make_unique<StringLiteralLVal>(mkLoc(@$), std::move($1) ); }
	| l_value '[' expr ']'  					{ $$ = make_unique<IndexLVal>(mkLoc(@$), std::move($1), std::move($3) ); }
	;

expr
	: T_INT_CONST           					{ $$ = make_unique<IntConst>(mkLoc(@$), $1); }
	| T_CHAR_CONST          					{ $$ = make_unique<CharConst>(mkLoc(@$), (unsigned char)$1); }
	| T_TRUE                					{ $$ = make_unique<TrueConst>(mkLoc(@$)); }
	| T_FALSE               					{ $$ = make_unique<FalseConst>(mkLoc(@$)); }
	| l_value               					{ $$ = make_unique<LValueExpr>(mkLoc(@$), std::move($1)); }
	| '(' expr ')'          					{ $$ = make_unique<ParenExpr>(mkLoc(@$), std::move($2)); }
	| func_call             					{ $$ = std::move($1); }
	| '+' expr %prec UPLUS  					{ $$ = make_unique<UnaryExpr>(mkLoc(@$), UnOp::Plus,  std::move($2)); }
	| '-' expr %prec UMINUS 					{ $$ = make_unique<UnaryExpr>(mkLoc(@$), UnOp::Minus, std::move($2)); }
	| '!' expr              					{ $$ = make_unique<UnaryExpr>(mkLoc(@$), UnOp::Not,  std::move($2)); }
	| expr '+' expr         					{ 
		$$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Add, std::move($1), std::move($3));
	}
	| expr '-' expr         					{ 
		$$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Sub, std::move($1), std::move($3)); 
	}
	| expr '*' expr         					{
		$$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Mul, std::move($1), std::move($3)); 
	}
	| expr '/' expr         					{ 
		$$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Div, std::move($1), std::move($3)); 
	}
	| expr '%' expr         					{ 
		$$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::Mod, std::move($1), std::move($3)); 
	}
	| expr '&' expr         					{ 
		$$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::AndBits, std::move($1), std::move($3)); 
	}
	| expr '|' expr         					{ 
		$$ = make_unique<BinaryExpr>(mkLoc(@$), BinOp::OrBits,  std::move($1), std::move($3)); 
	}
	;

cond
	: expr                  					{ $$ = make_unique<ExprCond>(mkLoc(@$), std::move($1)); }
	| '(' cond ')'          					{ $$ = make_unique<ParenCond>(mkLoc(@$), std::move($2)); }
	| T_NOT cond            					{ $$ = make_unique<NotCond>(mkLoc(@$), std::move($2)); }
	| cond T_AND cond       					{ 
		$$ = make_unique<BinaryCond>(mkLoc(@$), LogicOp::And, std::move($1), std::move($3)); 
	}
	| cond T_OR cond       						{ 
		$$ = make_unique<BinaryCond>(mkLoc(@$), LogicOp::Or,  std::move($1), std::move($3)); 
	}
	| expr '=' expr        						{ 
		$$ = make_unique<RelCond>(mkLoc(@$), RelOp::Eq, std::move($1), std::move($3)); 
	}
	| expr T_NE expr        					{ 
		$$ = make_unique<RelCond>(mkLoc(@$), RelOp::Ne, std::move($1), std::move($3)); 
	}
	| expr T_LE expr        					{ 
		$$ = make_unique<RelCond>(mkLoc(@$), RelOp::Le, std::move($1), std::move($3)); 
	}
	| expr T_GE expr        					{ 
		$$ = make_unique<RelCond>(mkLoc(@$), RelOp::Ge, std::move($1), std::move($3)); 
	}
	| expr '<' expr         					{ 
		$$ = make_unique<RelCond>(mkLoc(@$), RelOp::Lt, std::move($1), std::move($3)); 
	}
	| expr '>' expr         					{ 
		$$ = make_unique<RelCond>(mkLoc(@$), RelOp::Gt, std::move($1), std::move($3)); 
	}
	;
