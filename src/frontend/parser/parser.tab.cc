// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.tab.hh"


// Unqualified %code blocks.
#line 35 "frontend/parser/parser.y"

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


#line 69 "frontend/parser/parser.tab.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 3 "frontend/parser/parser.y"
namespace dana {
#line 162 "frontend/parser/parser.tab.cc"

  /// Build a parser object.
  parser::parser (up<Program>& ast_root_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      ast_root (ast_root_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.copy< char > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.copy< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.copy< optional<DataType> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.copy< optional<string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.copy< optional<up<Block>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.copy< string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block: // block
        value.copy< up<Block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_cond: // cond
        value.copy< up<Cond> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_local_def: // local_def
        value.copy< up<Def> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.copy< up<Expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.copy< up<FParDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.copy< up<FParType> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.copy< up<FuncDecl> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.copy< up<FuncDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_header: // header
        value.copy< up<Header> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_l_value: // l_value
        value.copy< up<Lval> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.copy< up<Program> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.copy< up<Stmt> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.copy< up<Type> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_var_def: // var_def
        value.copy< up<VarDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.copy< vec<optional<int>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.copy< vec<std::pair<up<Cond>, up<Block>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_id_list: // id_list
        value.copy< vec<string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.copy< vec<up<Def>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.copy< vec<up<Expr>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.copy< vec<up<FParDef>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.copy< vec<up<Stmt>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.move< char > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.move< int > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.move< optional<DataType> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.move< optional<string> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.move< optional<up<Block>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.move< string > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_block: // block
        value.move< up<Block> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_cond: // cond
        value.move< up<Cond> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_local_def: // local_def
        value.move< up<Def> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.move< up<Expr> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.move< up<FParDef> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.move< up<FParType> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.move< up<FuncDecl> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< up<FuncDef> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_header: // header
        value.move< up<Header> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_l_value: // l_value
        value.move< up<Lval> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_program: // program
        value.move< up<Program> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< up<Stmt> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type: // type
        value.move< up<Type> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_var_def: // var_def
        value.move< up<VarDef> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.move< vec<optional<int>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.move< vec<std::pair<up<Cond>, up<Block>>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_id_list: // id_list
        value.move< vec<string> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.move< vec<up<Def>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< vec<up<Expr>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.move< vec<up<FParDef>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.move< vec<up<Stmt>> > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.YY_MOVE_OR_COPY< char > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.YY_MOVE_OR_COPY< optional<DataType> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.YY_MOVE_OR_COPY< optional<string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.YY_MOVE_OR_COPY< optional<up<Block>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.YY_MOVE_OR_COPY< string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block: // block
        value.YY_MOVE_OR_COPY< up<Block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_cond: // cond
        value.YY_MOVE_OR_COPY< up<Cond> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_local_def: // local_def
        value.YY_MOVE_OR_COPY< up<Def> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.YY_MOVE_OR_COPY< up<Expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.YY_MOVE_OR_COPY< up<FParDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.YY_MOVE_OR_COPY< up<FParType> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.YY_MOVE_OR_COPY< up<FuncDecl> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.YY_MOVE_OR_COPY< up<FuncDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_header: // header
        value.YY_MOVE_OR_COPY< up<Header> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_l_value: // l_value
        value.YY_MOVE_OR_COPY< up<Lval> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.YY_MOVE_OR_COPY< up<Program> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.YY_MOVE_OR_COPY< up<Stmt> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.YY_MOVE_OR_COPY< up<Type> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_var_def: // var_def
        value.YY_MOVE_OR_COPY< up<VarDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.YY_MOVE_OR_COPY< vec<optional<int>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.YY_MOVE_OR_COPY< vec<std::pair<up<Cond>, up<Block>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_id_list: // id_list
        value.YY_MOVE_OR_COPY< vec<string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.YY_MOVE_OR_COPY< vec<up<Def>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.YY_MOVE_OR_COPY< vec<up<Expr>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.YY_MOVE_OR_COPY< vec<up<FParDef>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.YY_MOVE_OR_COPY< vec<up<Stmt>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.move< char > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.move< optional<DataType> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.move< optional<string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.move< optional<up<Block>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.move< string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block: // block
        value.move< up<Block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_cond: // cond
        value.move< up<Cond> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_local_def: // local_def
        value.move< up<Def> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.move< up<Expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.move< up<FParDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.move< up<FParType> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.move< up<FuncDecl> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< up<FuncDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_header: // header
        value.move< up<Header> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_l_value: // l_value
        value.move< up<Lval> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< up<Program> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< up<Stmt> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.move< up<Type> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_var_def: // var_def
        value.move< up<VarDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.move< vec<optional<int>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.move< vec<std::pair<up<Cond>, up<Block>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_id_list: // id_list
        value.move< vec<string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.move< vec<up<Def>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< vec<up<Expr>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.move< vec<up<FParDef>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.move< vec<up<Stmt>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.copy< char > (that.value);
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.copy< optional<DataType> > (that.value);
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.copy< optional<string> > (that.value);
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.copy< optional<up<Block>> > (that.value);
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.copy< string > (that.value);
        break;

      case symbol_kind::S_block: // block
        value.copy< up<Block> > (that.value);
        break;

      case symbol_kind::S_cond: // cond
        value.copy< up<Cond> > (that.value);
        break;

      case symbol_kind::S_local_def: // local_def
        value.copy< up<Def> > (that.value);
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.copy< up<Expr> > (that.value);
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.copy< up<FParDef> > (that.value);
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.copy< up<FParType> > (that.value);
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.copy< up<FuncDecl> > (that.value);
        break;

      case symbol_kind::S_func_def: // func_def
        value.copy< up<FuncDef> > (that.value);
        break;

      case symbol_kind::S_header: // header
        value.copy< up<Header> > (that.value);
        break;

      case symbol_kind::S_l_value: // l_value
        value.copy< up<Lval> > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.copy< up<Program> > (that.value);
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.copy< up<Stmt> > (that.value);
        break;

      case symbol_kind::S_type: // type
        value.copy< up<Type> > (that.value);
        break;

      case symbol_kind::S_var_def: // var_def
        value.copy< up<VarDef> > (that.value);
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.copy< vec<optional<int>> > (that.value);
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.copy< vec<std::pair<up<Cond>, up<Block>>> > (that.value);
        break;

      case symbol_kind::S_id_list: // id_list
        value.copy< vec<string> > (that.value);
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.copy< vec<up<Def>> > (that.value);
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.copy< vec<up<Expr>> > (that.value);
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.copy< vec<up<FParDef>> > (that.value);
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.copy< vec<up<Stmt>> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.move< char > (that.value);
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.move< int > (that.value);
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.move< optional<DataType> > (that.value);
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.move< optional<string> > (that.value);
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.move< optional<up<Block>> > (that.value);
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.move< string > (that.value);
        break;

      case symbol_kind::S_block: // block
        value.move< up<Block> > (that.value);
        break;

      case symbol_kind::S_cond: // cond
        value.move< up<Cond> > (that.value);
        break;

      case symbol_kind::S_local_def: // local_def
        value.move< up<Def> > (that.value);
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.move< up<Expr> > (that.value);
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.move< up<FParDef> > (that.value);
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.move< up<FParType> > (that.value);
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.move< up<FuncDecl> > (that.value);
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< up<FuncDef> > (that.value);
        break;

      case symbol_kind::S_header: // header
        value.move< up<Header> > (that.value);
        break;

      case symbol_kind::S_l_value: // l_value
        value.move< up<Lval> > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.move< up<Program> > (that.value);
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< up<Stmt> > (that.value);
        break;

      case symbol_kind::S_type: // type
        value.move< up<Type> > (that.value);
        break;

      case symbol_kind::S_var_def: // var_def
        value.move< up<VarDef> > (that.value);
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.move< vec<optional<int>> > (that.value);
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.move< vec<std::pair<up<Cond>, up<Block>>> > (that.value);
        break;

      case symbol_kind::S_id_list: // id_list
        value.move< vec<string> > (that.value);
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.move< vec<up<Def>> > (that.value);
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< vec<up<Expr>> > (that.value);
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.move< vec<up<FParDef>> > (that.value);
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.move< vec<up<Stmt>> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        yylhs.value.emplace< char > ();
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        yylhs.value.emplace< optional<DataType> > ();
        break;

      case symbol_kind::S_opt_id: // opt_id
        yylhs.value.emplace< optional<string> > ();
        break;

      case symbol_kind::S_opt_else: // opt_else
        yylhs.value.emplace< optional<up<Block>> > ();
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        yylhs.value.emplace< string > ();
        break;

      case symbol_kind::S_block: // block
        yylhs.value.emplace< up<Block> > ();
        break;

      case symbol_kind::S_cond: // cond
        yylhs.value.emplace< up<Cond> > ();
        break;

      case symbol_kind::S_local_def: // local_def
        yylhs.value.emplace< up<Def> > ();
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        yylhs.value.emplace< up<Expr> > ();
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        yylhs.value.emplace< up<FParDef> > ();
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        yylhs.value.emplace< up<FParType> > ();
        break;

      case symbol_kind::S_func_decl: // func_decl
        yylhs.value.emplace< up<FuncDecl> > ();
        break;

      case symbol_kind::S_func_def: // func_def
        yylhs.value.emplace< up<FuncDef> > ();
        break;

      case symbol_kind::S_header: // header
        yylhs.value.emplace< up<Header> > ();
        break;

      case symbol_kind::S_l_value: // l_value
        yylhs.value.emplace< up<Lval> > ();
        break;

      case symbol_kind::S_program: // program
        yylhs.value.emplace< up<Program> > ();
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        yylhs.value.emplace< up<Stmt> > ();
        break;

      case symbol_kind::S_type: // type
        yylhs.value.emplace< up<Type> > ();
        break;

      case symbol_kind::S_var_def: // var_def
        yylhs.value.emplace< up<VarDef> > ();
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        yylhs.value.emplace< vec<optional<int>> > ();
        break;

      case symbol_kind::S_elif_list: // elif_list
        yylhs.value.emplace< vec<std::pair<up<Cond>, up<Block>>> > ();
        break;

      case symbol_kind::S_id_list: // id_list
        yylhs.value.emplace< vec<string> > ();
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        yylhs.value.emplace< vec<up<Def>> > ();
        break;

      case symbol_kind::S_expr_list: // expr_list
        yylhs.value.emplace< vec<up<Expr>> > ();
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        yylhs.value.emplace< vec<up<FParDef>> > ();
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        yylhs.value.emplace< vec<up<Stmt>> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: func_def
#line 126 "frontend/parser/parser.y"
                                                                                        { 
		/* Build Program node and move it to the caller-owned ast_root */
		ast_root = make_unique<Program>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < up<FuncDef> > ()));
		/* Start symbol's semantic value is not used further */
		yylhs.value.as < up<Program> > () = up<Program>{};
	}
#line 1489 "frontend/parser/parser.tab.cc"
    break;

  case 3: // func_def: T_DEF header localdef_list block
#line 135 "frontend/parser/parser.y"
                                                                { 
		yylhs.value.as < up<FuncDef> > () = make_unique<FuncDef>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < up<Header> > ()), std::move(yystack_[1].value.as < vec<up<Def>> > ()), std::move(yystack_[0].value.as < up<Block> > ())); 
	}
#line 1497 "frontend/parser/parser.tab.cc"
    break;

  case 4: // localdef_list: %empty
#line 141 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < vec<up<Def>> > () = vec<up<Def>>{}; }
#line 1503 "frontend/parser/parser.tab.cc"
    break;

  case 5: // localdef_list: localdef_list local_def
#line 142 "frontend/parser/parser.y"
                                                                        { yystack_[1].value.as < vec<up<Def>> > ().emplace_back(std::move(yystack_[0].value.as < up<Def> > ())); yylhs.value.as < vec<up<Def>> > () = std::move(yystack_[1].value.as < vec<up<Def>> > ()); }
#line 1509 "frontend/parser/parser.tab.cc"
    break;

  case 6: // local_def: func_def
#line 146 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < up<Def> > () = std::move(yystack_[0].value.as < up<FuncDef> > ()); }
#line 1515 "frontend/parser/parser.tab.cc"
    break;

  case 7: // local_def: func_decl
#line 147 "frontend/parser/parser.y"
                                                            { yylhs.value.as < up<Def> > () = std::move(yystack_[0].value.as < up<FuncDecl> > ()); }
#line 1521 "frontend/parser/parser.tab.cc"
    break;

  case 8: // local_def: var_def
#line 148 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < up<Def> > () = std::move(yystack_[0].value.as < up<VarDef> > ()); }
#line 1527 "frontend/parser/parser.tab.cc"
    break;

  case 9: // func_decl: T_DECL header
#line 152 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < up<FuncDecl> > () = make_unique<FuncDecl>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < up<Header> > ())); }
#line 1533 "frontend/parser/parser.tab.cc"
    break;

  case 10: // var_def: T_VAR id_list T_IS type
#line 156 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<VarDef> > () = make_unique<VarDef>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < vec<string> > ()), std::move(yystack_[0].value.as < up<Type> > ())); }
#line 1539 "frontend/parser/parser.tab.cc"
    break;

  case 11: // type: T_INT type_dims
#line 160 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < up<Type> > () = make_unique<Type>(mkLoc(yylhs.location), DataType::INT,  std::move(yystack_[0].value.as < vec<optional<int>> > ())); }
#line 1545 "frontend/parser/parser.tab.cc"
    break;

  case 12: // type: T_BYTE type_dims
#line 161 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < up<Type> > () = make_unique<Type>(mkLoc(yylhs.location), DataType::BYTE, std::move(yystack_[0].value.as < vec<optional<int>> > ())); }
#line 1551 "frontend/parser/parser.tab.cc"
    break;

  case 13: // type_dims: %empty
#line 165 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < vec<optional<int>> > () = vec<optional<int>>{}; }
#line 1557 "frontend/parser/parser.tab.cc"
    break;

  case 14: // type_dims: type_dims '[' T_INT_CONST ']'
#line 166 "frontend/parser/parser.y"
                                                                { yystack_[3].value.as < vec<optional<int>> > ().emplace_back(yystack_[1].value.as < int > ()); yylhs.value.as < vec<optional<int>> > () = std::move(yystack_[3].value.as < vec<optional<int>> > ()); }
#line 1563 "frontend/parser/parser.tab.cc"
    break;

  case 15: // header: T_ID opt_ret_type opt_params
#line 170 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Header> > () = make_unique<Header>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < string > ()), yystack_[1].value.as < optional<DataType> > (), std::move(yystack_[0].value.as < vec<up<FParDef>> > ())); }
#line 1569 "frontend/parser/parser.tab.cc"
    break;

  case 16: // opt_ret_type: %empty
#line 174 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < optional<DataType> > () = optional<DataType>{}; }
#line 1575 "frontend/parser/parser.tab.cc"
    break;

  case 17: // opt_ret_type: T_IS T_INT
#line 175 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < optional<DataType> > () = DataType::INT; }
#line 1581 "frontend/parser/parser.tab.cc"
    break;

  case 18: // opt_ret_type: T_IS T_BYTE
#line 176 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < optional<DataType> > () = DataType::BYTE; }
#line 1587 "frontend/parser/parser.tab.cc"
    break;

  case 19: // opt_params: %empty
#line 180 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < vec<up<FParDef>> > () = vec<up<FParDef>>{}; }
#line 1593 "frontend/parser/parser.tab.cc"
    break;

  case 20: // opt_params: ':' fpar_list
#line 181 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < vec<up<FParDef>> > () = std::move(yystack_[0].value.as < vec<up<FParDef>> > ()); }
#line 1599 "frontend/parser/parser.tab.cc"
    break;

  case 21: // fpar_list: fpar_def
#line 185 "frontend/parser/parser.y"
                                                                        { vec<up<FParDef>> v; v.emplace_back(std::move(yystack_[0].value.as < up<FParDef> > ())); yylhs.value.as < vec<up<FParDef>> > () = std::move(v); }
#line 1605 "frontend/parser/parser.tab.cc"
    break;

  case 22: // fpar_list: fpar_list ',' fpar_def
#line 186 "frontend/parser/parser.y"
                                                                        { yystack_[2].value.as < vec<up<FParDef>> > ().emplace_back(std::move(yystack_[0].value.as < up<FParDef> > ())); yylhs.value.as < vec<up<FParDef>> > () = std::move(yystack_[2].value.as < vec<up<FParDef>> > ()); }
#line 1611 "frontend/parser/parser.tab.cc"
    break;

  case 23: // fpar_def: id_list T_AS fpar_type
#line 190 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<FParDef> > () = make_unique<FParDef>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < vec<string> > ()), std::move(yystack_[0].value.as < up<FParType> > ())); }
#line 1617 "frontend/parser/parser.tab.cc"
    break;

  case 24: // id_list: T_ID
#line 194 "frontend/parser/parser.y"
                                                                                { vec<string> v; v.emplace_back(std::move(yystack_[0].value.as < string > ())); yylhs.value.as < vec<string> > () = std::move(v); }
#line 1623 "frontend/parser/parser.tab.cc"
    break;

  case 25: // id_list: id_list T_ID
#line 195 "frontend/parser/parser.y"
                                                                                { yystack_[1].value.as < vec<string> > ().emplace_back(std::move(yystack_[0].value.as < string > ())); yylhs.value.as < vec<string> > () = std::move(yystack_[1].value.as < vec<string> > ()); }
#line 1629 "frontend/parser/parser.tab.cc"
    break;

  case 26: // fpar_type: T_INT
#line 199 "frontend/parser/parser.y"
                {
		yylhs.value.as < up<FParType> > () = make_unique<FParType>(mkLoc(yylhs.location), /* by-ref= */false, DataType::INT);
	  }
#line 1637 "frontend/parser/parser.tab.cc"
    break;

  case 27: // fpar_type: T_BYTE
#line 202 "frontend/parser/parser.y"
                 {
		yylhs.value.as < up<FParType> > () = make_unique<FParType>(mkLoc(yylhs.location), /* by-ref= */false, DataType::BYTE);
	  }
#line 1645 "frontend/parser/parser.tab.cc"
    break;

  case 28: // fpar_type: T_INT fpar_dims
#line 205 "frontend/parser/parser.y"
                          {
		yylhs.value.as < up<FParType> > () = make_unique<FParType>(mkLoc(yylhs.location), /* by-ref= */true, DataType::INT, std::move(yystack_[0].value.as < vec<optional<int>> > ())); 
	  }
#line 1653 "frontend/parser/parser.tab.cc"
    break;

  case 29: // fpar_type: T_BYTE fpar_dims
#line 208 "frontend/parser/parser.y"
                           {
		yylhs.value.as < up<FParType> > () = make_unique<FParType>(mkLoc(yylhs.location), /* by-ref= */true, DataType::BYTE, std::move(yystack_[0].value.as < vec<optional<int>> > ()));
	  }
#line 1661 "frontend/parser/parser.tab.cc"
    break;

  case 30: // fpar_type: T_REF T_INT
#line 211 "frontend/parser/parser.y"
                      {
		yylhs.value.as < up<FParType> > () = make_unique<FParType>(mkLoc(yylhs.location), /* by-ref= */true, DataType::INT); 
	  }
#line 1669 "frontend/parser/parser.tab.cc"
    break;

  case 31: // fpar_type: T_REF T_BYTE
#line 214 "frontend/parser/parser.y"
                       {
		yylhs.value.as < up<FParType> > () = make_unique<FParType>(mkLoc(yylhs.location), /* by-ref= */true, DataType::BYTE); 
	  }
#line 1677 "frontend/parser/parser.tab.cc"
    break;

  case 32: // fpar_dims: '[' T_INT_CONST ']' type_dims
#line 220 "frontend/parser/parser.y"
                                                                { 
		auto v = std::move(yystack_[0].value.as < vec<optional<int>> > ()); v.insert(v.begin(), yystack_[2].value.as < int > ()); yylhs.value.as < vec<optional<int>> > () = std::move(v); 
	}
#line 1685 "frontend/parser/parser.tab.cc"
    break;

  case 33: // fpar_dims: '[' ']' type_dims
#line 223 "frontend/parser/parser.y"
                                                                                { 
		auto v = std::move(yystack_[0].value.as < vec<optional<int>> > ()); v.insert(v.begin(), optional<int>{}); yylhs.value.as < vec<optional<int>> > () = std::move(v); 
	}
#line 1693 "frontend/parser/parser.tab.cc"
    break;

  case 34: // block: T_BEGIN stmt_list T_END
#line 229 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Block> > () = make_unique<Block>(mkLoc(yylhs.location), std::move(yystack_[1].value.as < vec<up<Stmt>> > ()) ); }
#line 1699 "frontend/parser/parser.tab.cc"
    break;

  case 35: // block: stmt_list T_AUTO_END
#line 230 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Block> > () = make_unique<Block>(mkLoc(yylhs.location), std::move(yystack_[1].value.as < vec<up<Stmt>> > ()) ); }
#line 1705 "frontend/parser/parser.tab.cc"
    break;

  case 36: // stmt_list: stmt
#line 234 "frontend/parser/parser.y"
                                                                        { vec<up<Stmt>> v; v.emplace_back(std::move(yystack_[0].value.as < up<Stmt> > ())); yylhs.value.as < vec<up<Stmt>> > () = std::move(v); }
#line 1711 "frontend/parser/parser.tab.cc"
    break;

  case 37: // stmt_list: stmt_list stmt
#line 235 "frontend/parser/parser.y"
                                                                { yystack_[1].value.as < vec<up<Stmt>> > ().emplace_back(std::move(yystack_[0].value.as < up<Stmt> > ())); yylhs.value.as < vec<up<Stmt>> > () = std::move(yystack_[1].value.as < vec<up<Stmt>> > ()); }
#line 1717 "frontend/parser/parser.tab.cc"
    break;

  case 38: // stmt: T_SKIP
#line 239 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = make_unique<SkipStmt>(mkLoc(yylhs.location)); }
#line 1723 "frontend/parser/parser.tab.cc"
    break;

  case 39: // stmt: assign_stmt
#line 240 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = std::move(yystack_[0].value.as < up<Stmt> > ()); }
#line 1729 "frontend/parser/parser.tab.cc"
    break;

  case 40: // stmt: proc_call
#line 241 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = std::move(yystack_[0].value.as < up<Stmt> > ()); }
#line 1735 "frontend/parser/parser.tab.cc"
    break;

  case 41: // stmt: T_EXIT
#line 242 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = make_unique<ExitStmt>(mkLoc(yylhs.location)); }
#line 1741 "frontend/parser/parser.tab.cc"
    break;

  case 42: // stmt: return_stmt
#line 243 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = std::move(yystack_[0].value.as < up<Stmt> > ()); }
#line 1747 "frontend/parser/parser.tab.cc"
    break;

  case 43: // stmt: if_stmt
#line 244 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = std::move(yystack_[0].value.as < up<Stmt> > ()); }
#line 1753 "frontend/parser/parser.tab.cc"
    break;

  case 44: // stmt: loop_stmt
#line 245 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = std::move(yystack_[0].value.as < up<Stmt> > ()); }
#line 1759 "frontend/parser/parser.tab.cc"
    break;

  case 45: // stmt: break_stmt
#line 246 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = std::move(yystack_[0].value.as < up<Stmt> > ()); }
#line 1765 "frontend/parser/parser.tab.cc"
    break;

  case 46: // stmt: continue_stmt
#line 247 "frontend/parser/parser.y"
                                                                { yylhs.value.as < up<Stmt> > () = std::move(yystack_[0].value.as < up<Stmt> > ()); }
#line 1771 "frontend/parser/parser.tab.cc"
    break;

  case 47: // assign_stmt: l_value T_ASSIGN expr
#line 251 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<AssignStmt>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < up<Lval> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); }
#line 1777 "frontend/parser/parser.tab.cc"
    break;

  case 48: // return_stmt: T_RET ':' expr
#line 255 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < up<Stmt> > () = make_unique<ReturnStmt>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < up<Expr> > ())); }
#line 1783 "frontend/parser/parser.tab.cc"
    break;

  case 49: // proc_call: T_ID
#line 259 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<ProcCall>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < string > ()), vec<up<Expr>>{}); }
#line 1789 "frontend/parser/parser.tab.cc"
    break;

  case 50: // proc_call: T_ID ':' expr_list
#line 260 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<ProcCall>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < string > ()), std::move(yystack_[0].value.as < vec<up<Expr>> > ())); }
#line 1795 "frontend/parser/parser.tab.cc"
    break;

  case 51: // break_stmt: T_BREAK
#line 264 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<BreakStmt>(mkLoc(yylhs.location), optional<string>{}); }
#line 1801 "frontend/parser/parser.tab.cc"
    break;

  case 52: // break_stmt: T_BREAK ':' T_ID
#line 265 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<BreakStmt>(mkLoc(yylhs.location), optional<string>{std::move(yystack_[0].value.as < string > ())}); }
#line 1807 "frontend/parser/parser.tab.cc"
    break;

  case 53: // continue_stmt: T_CONT
#line 269 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<ContinueStmt>(mkLoc(yylhs.location), optional<string>{}); }
#line 1813 "frontend/parser/parser.tab.cc"
    break;

  case 54: // continue_stmt: T_CONT ':' T_ID
#line 270 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<ContinueStmt>(mkLoc(yylhs.location), optional<string>{std::move(yystack_[0].value.as < string > ())}); }
#line 1819 "frontend/parser/parser.tab.cc"
    break;

  case 55: // if_stmt: T_IF cond ':' block elif_list opt_else
#line 274 "frontend/parser/parser.y"
                                                        { 
		yylhs.value.as < up<Stmt> > () = make_unique<IfStmt>(mkLoc(yylhs.location), std::move(yystack_[4].value.as < up<Cond> > ()), std::move(yystack_[2].value.as < up<Block> > ()), std::move(yystack_[1].value.as < vec<std::pair<up<Cond>, up<Block>>> > ()), std::move(yystack_[0].value.as < optional<up<Block>> > ())); 
	}
#line 1827 "frontend/parser/parser.tab.cc"
    break;

  case 56: // elif_list: %empty
#line 280 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < vec<std::pair<up<Cond>, up<Block>>> > () = vec<std::pair<up<Cond>, up<Block>>>{}; }
#line 1833 "frontend/parser/parser.tab.cc"
    break;

  case 57: // elif_list: elif_list T_ELIF cond ':' block
#line 281 "frontend/parser/parser.y"
                                                                { 
		yystack_[4].value.as < vec<std::pair<up<Cond>, up<Block>>> > ().emplace_back(std::make_pair(std::move(yystack_[2].value.as < up<Cond> > ()), std::move(yystack_[0].value.as < up<Block> > ()))); yylhs.value.as < vec<std::pair<up<Cond>, up<Block>>> > () = std::move(yystack_[4].value.as < vec<std::pair<up<Cond>, up<Block>>> > ()); 
	}
#line 1841 "frontend/parser/parser.tab.cc"
    break;

  case 58: // opt_else: %empty
#line 287 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < optional<up<Block>> > () = optional<up<Block>>{}; }
#line 1847 "frontend/parser/parser.tab.cc"
    break;

  case 59: // opt_else: T_ELSE ':' block
#line 288 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < optional<up<Block>> > () = optional<up<Block>>{ std::move(yystack_[0].value.as < up<Block> > ()) }; }
#line 1853 "frontend/parser/parser.tab.cc"
    break;

  case 60: // loop_stmt: T_LOOP opt_id ':' block
#line 292 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Stmt> > () = make_unique<LoopStmt>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < optional<string> > ()), std::move(yystack_[0].value.as < up<Block> > ())); }
#line 1859 "frontend/parser/parser.tab.cc"
    break;

  case 61: // opt_id: %empty
#line 296 "frontend/parser/parser.y"
                                                                                        { yylhs.value.as < optional<string> > () = optional<string>{}; }
#line 1865 "frontend/parser/parser.tab.cc"
    break;

  case 62: // opt_id: T_ID
#line 297 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < optional<string> > () = optional<string>{ std::move(yystack_[0].value.as < string > ()) }; }
#line 1871 "frontend/parser/parser.tab.cc"
    break;

  case 63: // expr_list: expr
#line 301 "frontend/parser/parser.y"
                                                                                        { vec<up<Expr>> v; v.emplace_back(std::move(yystack_[0].value.as < up<Expr> > ())); yylhs.value.as < vec<up<Expr>> > () = std::move(v); }
#line 1877 "frontend/parser/parser.tab.cc"
    break;

  case 64: // expr_list: expr_list ',' expr
#line 302 "frontend/parser/parser.y"
                                                                        { yystack_[2].value.as < vec<up<Expr>> > ().emplace_back(std::move(yystack_[0].value.as < up<Expr> > ())); yylhs.value.as < vec<up<Expr>> > () = std::move(yystack_[2].value.as < vec<up<Expr>> > ()); }
#line 1883 "frontend/parser/parser.tab.cc"
    break;

  case 65: // func_call: T_ID '(' ')'
#line 306 "frontend/parser/parser.y"
                                                                                { yylhs.value.as < up<Expr> > () = make_unique<FuncCall>(mkLoc(yylhs.location), std::move(yystack_[2].value.as < string > ()), vec<up<Expr>>{}); }
#line 1889 "frontend/parser/parser.tab.cc"
    break;

  case 66: // func_call: T_ID '(' expr_list ')'
#line 307 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<FuncCall>(mkLoc(yylhs.location), std::move(yystack_[3].value.as < string > ()), std::move(yystack_[1].value.as < vec<up<Expr>> > ())); }
#line 1895 "frontend/parser/parser.tab.cc"
    break;

  case 67: // l_value: T_ID
#line 311 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Lval> > () = make_unique<IdLVal>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < string > ()) ); }
#line 1901 "frontend/parser/parser.tab.cc"
    break;

  case 68: // l_value: T_STRING_CONST
#line 312 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Lval> > () = make_unique<StringLiteralLVal>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < string > ()) ); }
#line 1907 "frontend/parser/parser.tab.cc"
    break;

  case 69: // l_value: l_value '[' expr ']'
#line 313 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Lval> > () = make_unique<IndexLVal>(mkLoc(yylhs.location), std::move(yystack_[3].value.as < up<Lval> > ()), std::move(yystack_[1].value.as < up<Expr> > ()) ); }
#line 1913 "frontend/parser/parser.tab.cc"
    break;

  case 70: // expr: T_INT_CONST
#line 317 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<IntConst>(mkLoc(yylhs.location), yystack_[0].value.as < int > ()); }
#line 1919 "frontend/parser/parser.tab.cc"
    break;

  case 71: // expr: T_CHAR_CONST
#line 318 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<CharConst>(mkLoc(yylhs.location), (unsigned char)yystack_[0].value.as < char > ()); }
#line 1925 "frontend/parser/parser.tab.cc"
    break;

  case 72: // expr: T_TRUE
#line 319 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<TrueConst>(mkLoc(yylhs.location)); }
#line 1931 "frontend/parser/parser.tab.cc"
    break;

  case 73: // expr: T_FALSE
#line 320 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<FalseConst>(mkLoc(yylhs.location)); }
#line 1937 "frontend/parser/parser.tab.cc"
    break;

  case 74: // expr: l_value
#line 321 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<LValueExpr>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < up<Lval> > ())); }
#line 1943 "frontend/parser/parser.tab.cc"
    break;

  case 75: // expr: '(' expr ')'
#line 322 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<ParenExpr>(mkLoc(yylhs.location), std::move(yystack_[1].value.as < up<Expr> > ())); }
#line 1949 "frontend/parser/parser.tab.cc"
    break;

  case 76: // expr: func_call
#line 323 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = std::move(yystack_[0].value.as < up<Expr> > ()); }
#line 1955 "frontend/parser/parser.tab.cc"
    break;

  case 77: // expr: '+' expr
#line 324 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<UnaryExpr>(mkLoc(yylhs.location), UnOp::Plus,  std::move(yystack_[0].value.as < up<Expr> > ())); }
#line 1961 "frontend/parser/parser.tab.cc"
    break;

  case 78: // expr: '-' expr
#line 325 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<UnaryExpr>(mkLoc(yylhs.location), UnOp::Minus, std::move(yystack_[0].value.as < up<Expr> > ())); }
#line 1967 "frontend/parser/parser.tab.cc"
    break;

  case 79: // expr: '!' expr
#line 326 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Expr> > () = make_unique<UnaryExpr>(mkLoc(yylhs.location), UnOp::Not,  std::move(yystack_[0].value.as < up<Expr> > ())); }
#line 1973 "frontend/parser/parser.tab.cc"
    break;

  case 80: // expr: expr '+' expr
#line 327 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Expr> > () = make_unique<BinaryExpr>(mkLoc(yylhs.location), BinOp::Add, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ()));
	}
#line 1981 "frontend/parser/parser.tab.cc"
    break;

  case 81: // expr: expr '-' expr
#line 330 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Expr> > () = make_unique<BinaryExpr>(mkLoc(yylhs.location), BinOp::Sub, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 1989 "frontend/parser/parser.tab.cc"
    break;

  case 82: // expr: expr '*' expr
#line 333 "frontend/parser/parser.y"
                                                                        {
		yylhs.value.as < up<Expr> > () = make_unique<BinaryExpr>(mkLoc(yylhs.location), BinOp::Mul, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 1997 "frontend/parser/parser.tab.cc"
    break;

  case 83: // expr: expr '/' expr
#line 336 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Expr> > () = make_unique<BinaryExpr>(mkLoc(yylhs.location), BinOp::Div, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2005 "frontend/parser/parser.tab.cc"
    break;

  case 84: // expr: expr '%' expr
#line 339 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Expr> > () = make_unique<BinaryExpr>(mkLoc(yylhs.location), BinOp::Mod, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2013 "frontend/parser/parser.tab.cc"
    break;

  case 85: // expr: expr '&' expr
#line 342 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Expr> > () = make_unique<BinaryExpr>(mkLoc(yylhs.location), BinOp::AndBits, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2021 "frontend/parser/parser.tab.cc"
    break;

  case 86: // expr: expr '|' expr
#line 345 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Expr> > () = make_unique<BinaryExpr>(mkLoc(yylhs.location), BinOp::OrBits,  std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2029 "frontend/parser/parser.tab.cc"
    break;

  case 87: // cond: expr
#line 351 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Cond> > () = make_unique<ExprCond>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < up<Expr> > ())); }
#line 2035 "frontend/parser/parser.tab.cc"
    break;

  case 88: // cond: T_NOT cond
#line 354 "frontend/parser/parser.y"
                                                                        { yylhs.value.as < up<Cond> > () = make_unique<NotCond>(mkLoc(yylhs.location), std::move(yystack_[0].value.as < up<Cond> > ())); }
#line 2041 "frontend/parser/parser.tab.cc"
    break;

  case 89: // cond: cond T_AND cond
#line 355 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<BinaryCond>(mkLoc(yylhs.location), LogicOp::And, std::move(yystack_[2].value.as < up<Cond> > ()), std::move(yystack_[0].value.as < up<Cond> > ())); 
	}
#line 2049 "frontend/parser/parser.tab.cc"
    break;

  case 90: // cond: cond T_OR cond
#line 358 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<BinaryCond>(mkLoc(yylhs.location), LogicOp::Or,  std::move(yystack_[2].value.as < up<Cond> > ()), std::move(yystack_[0].value.as < up<Cond> > ())); 
	}
#line 2057 "frontend/parser/parser.tab.cc"
    break;

  case 91: // cond: expr '=' expr
#line 361 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<RelCond>(mkLoc(yylhs.location), RelOp::Eq, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2065 "frontend/parser/parser.tab.cc"
    break;

  case 92: // cond: expr T_NE expr
#line 364 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<RelCond>(mkLoc(yylhs.location), RelOp::Ne, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2073 "frontend/parser/parser.tab.cc"
    break;

  case 93: // cond: expr T_LE expr
#line 367 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<RelCond>(mkLoc(yylhs.location), RelOp::Le, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2081 "frontend/parser/parser.tab.cc"
    break;

  case 94: // cond: expr T_GE expr
#line 370 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<RelCond>(mkLoc(yylhs.location), RelOp::Ge, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2089 "frontend/parser/parser.tab.cc"
    break;

  case 95: // cond: expr '<' expr
#line 373 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<RelCond>(mkLoc(yylhs.location), RelOp::Lt, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2097 "frontend/parser/parser.tab.cc"
    break;

  case 96: // cond: expr '>' expr
#line 376 "frontend/parser/parser.y"
                                                                        { 
		yylhs.value.as < up<Cond> > () = make_unique<RelCond>(mkLoc(yylhs.location), RelOp::Gt, std::move(yystack_[2].value.as < up<Expr> > ()), std::move(yystack_[0].value.as < up<Expr> > ())); 
	}
#line 2105 "frontend/parser/parser.tab.cc"
    break;


#line 2109 "frontend/parser/parser.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    static const char *const yy_sname[] =
    {
    "end of file", "error", "invalid token", "T_AND", "T_AS", "T_BEGIN",
  "T_BREAK", "T_BYTE", "T_CONT", "T_DECL", "T_DEF", "T_ELIF", "T_ELSE",
  "T_END", "T_EXIT", "T_FALSE", "T_IF", "T_IS", "T_INT", "T_LOOP", "T_NOT",
  "T_OR", "T_REF", "T_RET", "T_SKIP", "T_TRUE", "T_VAR", "T_INT_CONST",
  "T_CHAR_CONST", "T_STRING_CONST", "T_ID", "T_ASSIGN", "T_NE", "T_LE",
  "T_GE", "T_AUTO_END", "'|'", "'&'", "'='", "'<'", "'>'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "UPLUS", "UMINUS", "'!'", "'['", "']'", "':'",
  "','", "'('", "')'", "$accept", "program", "func_def", "localdef_list",
  "local_def", "func_decl", "var_def", "type", "type_dims", "header",
  "opt_ret_type", "opt_params", "fpar_list", "fpar_def", "id_list",
  "fpar_type", "fpar_dims", "block", "stmt_list", "stmt", "assign_stmt",
  "return_stmt", "proc_call", "break_stmt", "continue_stmt", "if_stmt",
  "elif_list", "opt_else", "loop_stmt", "opt_id", "expr_list", "func_call",
  "l_value", "expr", "cond", YY_NULLPTR
    };
    return yy_sname[yysymbol];
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -92;

  const signed char parser::yytable_ninf_ = -68;

  const short
  parser::yypact_[] =
  {
      -3,   -15,    12,   -92,    24,   -92,   -92,    68,    31,   143,
     -92,   -92,    67,   -92,   200,    47,    55,   -15,   -92,    51,
      72,    60,   -92,    67,   -92,    36,   -92,   -92,   -92,   -92,
     -92,   111,   -92,   -92,   -92,   -92,   -92,   -92,   -92,   -92,
     -21,   -92,    61,   -92,     2,   181,    84,    85,   -92,   -92,
      51,   -92,   -92,   -92,    63,    80,    80,    80,    80,   -92,
      69,   219,    -1,   -92,    73,    80,    53,    80,   -92,   -92,
      80,    80,    67,    22,   -92,   -92,   -92,   -92,   -92,   -11,
     -92,   -92,   -92,   190,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    51,    51,   169,
     169,   229,    70,    71,   229,   229,   204,   -92,    77,    77,
      94,   -92,   -92,    42,   -92,   229,   229,   229,   238,   137,
     229,   229,   229,    29,    29,   -92,   -92,   -92,   -92,   117,
     -92,   -92,   -92,   -92,   -92,    80,   -92,   -16,   -92,   -92,
     -92,   -92,   -92,    34,    82,    82,   229,    79,   -92,    51,
      81,   -92,   109,   -92,    82,     0,   169,    87,    82,   169,
     -92,   -92,   -92
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     0,     0,     2,    16,     4,     1,     0,    19,     0,
      18,    17,     0,    15,     0,    51,    53,     0,    41,     0,
      61,     0,    38,     0,    68,    49,     6,     5,     7,     8,
       3,     0,    36,    39,    42,    40,    45,    46,    43,    44,
       0,    24,    20,    21,     0,     0,     0,     0,     9,    73,
       0,    72,    70,    71,    67,     0,     0,     0,     0,    76,
      74,    87,     0,    62,     0,     0,     0,     0,    35,    37,
       0,     0,     0,     0,    25,    34,    52,    54,    88,     0,
      77,    78,    79,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,    50,    63,    47,     0,    22,    27,    26,
       0,    23,    65,     0,    75,    92,    93,    94,    86,    85,
      91,    95,    96,    80,    81,    82,    83,    84,    89,    90,
      56,    60,    13,    13,    10,     0,    69,     0,    29,    28,
      31,    30,    66,    58,    12,    11,    64,     0,    13,     0,
       0,    55,     0,    13,    33,     0,     0,     0,    32,     0,
      59,    14,    57
  };

  const short
  parser::yypgoto_[] =
  {
     -92,   -92,   129,   -92,   -92,   -92,   -92,   -92,   -64,   122,
     -92,   -92,   -92,    83,   119,   -92,    35,   -91,   131,   -18,
     -92,   -92,   -92,   -92,   -92,   -92,   -92,   -92,   -92,   -92,
      64,   -92,    -9,   -32,   -49
  };

  const unsigned char
  parser::yydefgoto_[] =
  {
       0,     2,     3,     9,    27,    28,    29,   134,   144,     5,
       8,    13,    42,    43,    44,   111,   138,    30,    31,    32,
      33,    34,    35,    36,    37,    38,   143,   151,    39,    64,
     103,    59,    60,    61,    62
  };

  const short
  parser::yytable_[] =
  {
      40,    78,    97,    97,    49,    40,    73,     1,   130,   131,
      70,   147,     6,    69,    51,     4,    52,    53,    24,    54,
      98,    98,    40,    80,    81,    82,    83,    69,    71,   108,
      55,    56,    74,   101,   148,   104,    40,    57,   105,   106,
     109,     7,    58,   112,   110,   149,   150,   104,   128,   129,
      99,   159,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   160,    49,   -67,   162,   145,
     102,    50,    94,    95,    96,    10,    51,   132,    52,    53,
      24,    54,    12,    74,   154,   -67,    11,    67,   133,   158,
      40,    40,    55,    56,   135,    49,   142,    41,    46,    57,
     155,   140,    63,   146,    58,    51,    47,    52,    53,    24,
      54,    65,   141,    72,    76,    77,    79,    15,    71,    16,
      97,    55,    56,   135,   100,    18,   137,    19,    57,   153,
      20,   152,   156,    58,    21,    22,   157,   161,    26,    48,
      24,    25,    66,   113,   139,    45,    68,    40,    14,    15,
      40,    16,    17,     1,     0,   107,     0,    18,     0,    19,
       0,     0,    20,     0,     0,     0,    21,    22,     0,    23,
       0,     0,    24,    25,    14,    15,     0,    16,    92,    93,
      94,    95,    96,    18,     0,    19,     0,    15,    20,    16,
       0,     0,    21,    22,    75,    18,     0,    19,    24,    25,
      20,     0,     0,     0,    21,    22,    15,     0,    16,     0,
      24,    25,     0,     0,    18,     0,    19,     0,     0,    20,
       0,     0,     0,    21,    22,     0,    87,    88,     0,    24,
      25,    92,    93,    94,    95,    96,     0,     0,     0,     0,
      87,    88,     0,     0,   114,    92,    93,    94,    95,    96,
       0,    84,    85,    86,   136,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    87,    88,     0,     0,     0,
      92,    93,    94,    95,    96,    88,     0,     0,     0,    92,
      93,    94,    95,    96
  };

  const short
  parser::yycheck_[] =
  {
       9,    50,     3,     3,    15,    14,     4,    10,    99,   100,
      31,    27,     0,    31,    25,    30,    27,    28,    29,    30,
      21,    21,    31,    55,    56,    57,    58,    45,    49,     7,
      41,    42,    30,    65,    50,    67,    45,    48,    70,    71,
      18,    17,    53,    54,    22,    11,    12,    79,    97,    98,
      51,    51,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,   156,    15,    31,   159,   133,
      17,    20,    43,    44,    45,     7,    25,     7,    27,    28,
      29,    30,    51,    30,   148,    49,    18,    51,    18,   153,
      99,   100,    41,    42,    52,    15,    54,    30,    51,    48,
     149,     7,    30,   135,    53,    25,    51,    27,    28,    29,
      30,    51,    18,    52,    30,    30,    53,     6,    49,     8,
       3,    41,    42,    52,    51,    14,    49,    16,    48,    50,
      19,    49,    51,    53,    23,    24,    27,    50,     9,    17,
      29,    30,    23,    79,   109,    14,    35,   156,     5,     6,
     159,     8,     9,    10,    -1,    72,    -1,    14,    -1,    16,
      -1,    -1,    19,    -1,    -1,    -1,    23,    24,    -1,    26,
      -1,    -1,    29,    30,     5,     6,    -1,     8,    41,    42,
      43,    44,    45,    14,    -1,    16,    -1,     6,    19,     8,
      -1,    -1,    23,    24,    13,    14,    -1,    16,    29,    30,
      19,    -1,    -1,    -1,    23,    24,     6,    -1,     8,    -1,
      29,    30,    -1,    -1,    14,    -1,    16,    -1,    -1,    19,
      -1,    -1,    -1,    23,    24,    -1,    36,    37,    -1,    29,
      30,    41,    42,    43,    44,    45,    -1,    -1,    -1,    -1,
      36,    37,    -1,    -1,    54,    41,    42,    43,    44,    45,
      -1,    32,    33,    34,    50,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    36,    37,    -1,    -1,    -1,
      41,    42,    43,    44,    45,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    45
  };

  const signed char
  parser::yystos_[] =
  {
       0,    10,    56,    57,    30,    64,     0,    17,    65,    58,
       7,    18,    51,    66,     5,     6,     8,     9,    14,    16,
      19,    23,    24,    26,    29,    30,    57,    59,    60,    61,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    83,
      87,    30,    67,    68,    69,    73,    51,    51,    64,    15,
      20,    25,    27,    28,    30,    41,    42,    48,    53,    86,
      87,    88,    89,    30,    84,    51,    69,    51,    35,    74,
      31,    49,    52,     4,    30,    13,    30,    30,    89,    53,
      88,    88,    88,    88,    32,    33,    34,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,     3,    21,    51,
      51,    88,    17,    85,    88,    88,    88,    68,     7,    18,
      22,    70,    54,    85,    54,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    89,    89,
      72,    72,     7,    18,    62,    52,    50,    49,    71,    71,
       7,    18,    54,    81,    63,    63,    88,    27,    50,    11,
      12,    82,    49,    50,    63,    89,    51,    27,    63,    51,
      72,    50,    72
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    55,    56,    57,    58,    58,    59,    59,    59,    60,
      61,    62,    62,    63,    63,    64,    65,    65,    65,    66,
      66,    67,    67,    68,    69,    69,    70,    70,    70,    70,
      70,    70,    71,    71,    72,    72,    73,    73,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    75,    76,    77,
      77,    78,    78,    79,    79,    80,    81,    81,    82,    82,
      83,    84,    84,    85,    85,    86,    86,    87,    87,    87,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     4,     0,     2,     1,     1,     1,     2,
       4,     2,     2,     0,     4,     3,     0,     2,     2,     0,
       2,     1,     3,     3,     1,     2,     1,     1,     2,     2,
       2,     2,     4,     3,     3,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     6,     0,     5,     0,     3,
       4,     0,     1,     1,     3,     3,     4,     1,     1,     4,
       1,     1,     1,     1,     1,     3,     1,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     1,     2,     3,
       3,     3,     3,     3,     3,     3,     3
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   126,   126,   135,   141,   142,   146,   147,   148,   152,
     156,   160,   161,   165,   166,   170,   174,   175,   176,   180,
     181,   185,   186,   190,   194,   195,   199,   202,   205,   208,
     211,   214,   220,   223,   229,   230,   234,   235,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   251,   255,   259,
     260,   264,   265,   269,   270,   274,   280,   281,   287,   288,
     292,   296,   297,   301,   302,   306,   307,   311,   312,   313,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   330,   333,   336,   339,   342,   345,   351,   354,   355,
     358,   361,   364,   367,   370,   373,   376
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    48,     2,     2,     2,    45,    37,     2,
      53,    54,    43,    41,    52,    42,     2,    44,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    51,     2,
      39,    38,    40,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    36,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    46,    47
    };
    // Last valid token kind.
    const int code_max = 292;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 3 "frontend/parser/parser.y"
} // dana
#line 2723 "frontend/parser/parser.tab.cc"

