// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file frontend/parser/parser.tab.hh
 ** Define the dana::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_FRONTEND_PARSER_PARSER_TAB_HH_INCLUDED
# define YY_YY_FRONTEND_PARSER_PARSER_TAB_HH_INCLUDED
// "%code requires" blocks.
#line 12 "frontend/parser/parser.y"

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

#line 72 "frontend/parser/parser.tab.hh"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

#line 3 "frontend/parser/parser.y"
namespace dana {
#line 208 "frontend/parser/parser.tab.hh"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // T_CHAR_CONST
      char dummy1[sizeof (char)];

      // T_INT_CONST
      char dummy2[sizeof (int)];

      // opt_ret_type
      char dummy3[sizeof (optional<DataType>)];

      // opt_id
      char dummy4[sizeof (optional<string>)];

      // opt_else
      char dummy5[sizeof (optional<up<Block>>)];

      // T_STRING_CONST
      // T_ID
      char dummy6[sizeof (string)];

      // block
      char dummy7[sizeof (up<Block>)];

      // cond
      char dummy8[sizeof (up<Cond>)];

      // local_def
      char dummy9[sizeof (up<Def>)];

      // func_call
      // expr
      char dummy10[sizeof (up<Expr>)];

      // fpar_def
      char dummy11[sizeof (up<FParDef>)];

      // fpar_type
      char dummy12[sizeof (up<FParType>)];

      // func_decl
      char dummy13[sizeof (up<FuncDecl>)];

      // func_def
      char dummy14[sizeof (up<FuncDef>)];

      // header
      char dummy15[sizeof (up<Header>)];

      // l_value
      char dummy16[sizeof (up<Lval>)];

      // program
      char dummy17[sizeof (up<Program>)];

      // stmt
      // assign_stmt
      // return_stmt
      // proc_call
      // break_stmt
      // continue_stmt
      // if_stmt
      // loop_stmt
      char dummy18[sizeof (up<Stmt>)];

      // type
      char dummy19[sizeof (up<Type>)];

      // var_def
      char dummy20[sizeof (up<VarDef>)];

      // type_dims
      // fpar_dims
      char dummy21[sizeof (vec<optional<int>>)];

      // elif_list
      char dummy22[sizeof (vec<std::pair<up<Cond>, up<Block>>>)];

      // id_list
      char dummy23[sizeof (vec<string>)];

      // localdef_list
      char dummy24[sizeof (vec<up<Def>>)];

      // expr_list
      char dummy25[sizeof (vec<up<Expr>>)];

      // opt_params
      // fpar_list
      char dummy26[sizeof (vec<up<FParDef>>)];

      // stmt_list
      char dummy27[sizeof (vec<up<Stmt>>)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        YYEMPTY = -2,
    YYEOF = 0,                     // "end of file"
    YYerror = 256,                 // error
    YYUNDEF = 257,                 // "invalid token"
    T_AND = 258,                   // T_AND
    T_AS = 259,                    // T_AS
    T_BEGIN = 260,                 // T_BEGIN
    T_BREAK = 261,                 // T_BREAK
    T_BYTE = 262,                  // T_BYTE
    T_CONT = 263,                  // T_CONT
    T_DECL = 264,                  // T_DECL
    T_DEF = 265,                   // T_DEF
    T_ELIF = 266,                  // T_ELIF
    T_ELSE = 267,                  // T_ELSE
    T_END = 268,                   // T_END
    T_EXIT = 269,                  // T_EXIT
    T_FALSE = 270,                 // T_FALSE
    T_IF = 271,                    // T_IF
    T_IS = 272,                    // T_IS
    T_INT = 273,                   // T_INT
    T_LOOP = 274,                  // T_LOOP
    T_NOT = 275,                   // T_NOT
    T_OR = 276,                    // T_OR
    T_REF = 277,                   // T_REF
    T_RET = 278,                   // T_RET
    T_SKIP = 279,                  // T_SKIP
    T_TRUE = 280,                  // T_TRUE
    T_VAR = 281,                   // T_VAR
    T_INT_CONST = 282,             // T_INT_CONST
    T_CHAR_CONST = 283,            // T_CHAR_CONST
    T_STRING_CONST = 284,          // T_STRING_CONST
    T_ID = 285,                    // T_ID
    T_ASSIGN = 286,                // T_ASSIGN
    T_NE = 287,                    // T_NE
    T_LE = 288,                    // T_LE
    T_GE = 289,                    // T_GE
    T_AUTO_END = 290,              // T_AUTO_END
    UPLUS = 291,                   // UPLUS
    UMINUS = 292                   // UMINUS
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 55, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_T_AND = 3,                             // T_AND
        S_T_AS = 4,                              // T_AS
        S_T_BEGIN = 5,                           // T_BEGIN
        S_T_BREAK = 6,                           // T_BREAK
        S_T_BYTE = 7,                            // T_BYTE
        S_T_CONT = 8,                            // T_CONT
        S_T_DECL = 9,                            // T_DECL
        S_T_DEF = 10,                            // T_DEF
        S_T_ELIF = 11,                           // T_ELIF
        S_T_ELSE = 12,                           // T_ELSE
        S_T_END = 13,                            // T_END
        S_T_EXIT = 14,                           // T_EXIT
        S_T_FALSE = 15,                          // T_FALSE
        S_T_IF = 16,                             // T_IF
        S_T_IS = 17,                             // T_IS
        S_T_INT = 18,                            // T_INT
        S_T_LOOP = 19,                           // T_LOOP
        S_T_NOT = 20,                            // T_NOT
        S_T_OR = 21,                             // T_OR
        S_T_REF = 22,                            // T_REF
        S_T_RET = 23,                            // T_RET
        S_T_SKIP = 24,                           // T_SKIP
        S_T_TRUE = 25,                           // T_TRUE
        S_T_VAR = 26,                            // T_VAR
        S_T_INT_CONST = 27,                      // T_INT_CONST
        S_T_CHAR_CONST = 28,                     // T_CHAR_CONST
        S_T_STRING_CONST = 29,                   // T_STRING_CONST
        S_T_ID = 30,                             // T_ID
        S_T_ASSIGN = 31,                         // T_ASSIGN
        S_T_NE = 32,                             // T_NE
        S_T_LE = 33,                             // T_LE
        S_T_GE = 34,                             // T_GE
        S_T_AUTO_END = 35,                       // T_AUTO_END
        S_36_ = 36,                              // '|'
        S_37_ = 37,                              // '&'
        S_38_ = 38,                              // '='
        S_39_ = 39,                              // '<'
        S_40_ = 40,                              // '>'
        S_41_ = 41,                              // '+'
        S_42_ = 42,                              // '-'
        S_43_ = 43,                              // '*'
        S_44_ = 44,                              // '/'
        S_45_ = 45,                              // '%'
        S_UPLUS = 46,                            // UPLUS
        S_UMINUS = 47,                           // UMINUS
        S_48_ = 48,                              // '!'
        S_49_ = 49,                              // '['
        S_50_ = 50,                              // ']'
        S_51_ = 51,                              // ':'
        S_52_ = 52,                              // ','
        S_53_ = 53,                              // '('
        S_54_ = 54,                              // ')'
        S_YYACCEPT = 55,                         // $accept
        S_program = 56,                          // program
        S_func_def = 57,                         // func_def
        S_localdef_list = 58,                    // localdef_list
        S_local_def = 59,                        // local_def
        S_func_decl = 60,                        // func_decl
        S_var_def = 61,                          // var_def
        S_type = 62,                             // type
        S_type_dims = 63,                        // type_dims
        S_header = 64,                           // header
        S_opt_ret_type = 65,                     // opt_ret_type
        S_opt_params = 66,                       // opt_params
        S_fpar_list = 67,                        // fpar_list
        S_fpar_def = 68,                         // fpar_def
        S_id_list = 69,                          // id_list
        S_fpar_type = 70,                        // fpar_type
        S_fpar_dims = 71,                        // fpar_dims
        S_block = 72,                            // block
        S_stmt_list = 73,                        // stmt_list
        S_stmt = 74,                             // stmt
        S_assign_stmt = 75,                      // assign_stmt
        S_return_stmt = 76,                      // return_stmt
        S_proc_call = 77,                        // proc_call
        S_break_stmt = 78,                       // break_stmt
        S_continue_stmt = 79,                    // continue_stmt
        S_if_stmt = 80,                          // if_stmt
        S_elif_list = 81,                        // elif_list
        S_opt_else = 82,                         // opt_else
        S_loop_stmt = 83,                        // loop_stmt
        S_opt_id = 84,                           // opt_id
        S_expr_list = 85,                        // expr_list
        S_func_call = 86,                        // func_call
        S_l_value = 87,                          // l_value
        S_expr = 88,                             // expr
        S_cond = 89                              // cond
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.move< char > (std::move (that.value));
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.move< int > (std::move (that.value));
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.move< optional<DataType> > (std::move (that.value));
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.move< optional<string> > (std::move (that.value));
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.move< optional<up<Block>> > (std::move (that.value));
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.move< string > (std::move (that.value));
        break;

      case symbol_kind::S_block: // block
        value.move< up<Block> > (std::move (that.value));
        break;

      case symbol_kind::S_cond: // cond
        value.move< up<Cond> > (std::move (that.value));
        break;

      case symbol_kind::S_local_def: // local_def
        value.move< up<Def> > (std::move (that.value));
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.move< up<Expr> > (std::move (that.value));
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.move< up<FParDef> > (std::move (that.value));
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.move< up<FParType> > (std::move (that.value));
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.move< up<FuncDecl> > (std::move (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< up<FuncDef> > (std::move (that.value));
        break;

      case symbol_kind::S_header: // header
        value.move< up<Header> > (std::move (that.value));
        break;

      case symbol_kind::S_l_value: // l_value
        value.move< up<Lval> > (std::move (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< up<Program> > (std::move (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< up<Stmt> > (std::move (that.value));
        break;

      case symbol_kind::S_type: // type
        value.move< up<Type> > (std::move (that.value));
        break;

      case symbol_kind::S_var_def: // var_def
        value.move< up<VarDef> > (std::move (that.value));
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.move< vec<optional<int>> > (std::move (that.value));
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.move< vec<std::pair<up<Cond>, up<Block>>> > (std::move (that.value));
        break;

      case symbol_kind::S_id_list: // id_list
        value.move< vec<string> > (std::move (that.value));
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.move< vec<up<Def>> > (std::move (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< vec<up<Expr>> > (std::move (that.value));
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.move< vec<up<FParDef>> > (std::move (that.value));
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.move< vec<up<Stmt>> > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, char&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const char& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, int&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const int& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, optional<DataType>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const optional<DataType>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, optional<string>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const optional<string>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, optional<up<Block>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const optional<up<Block>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Block>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Block>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Cond>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Cond>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Def>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Def>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Expr>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Expr>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<FParDef>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<FParDef>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<FParType>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<FParType>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<FuncDecl>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<FuncDecl>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<FuncDef>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<FuncDef>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Header>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Header>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Lval>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Lval>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Program>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Program>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Stmt>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Stmt>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<Type>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<Type>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, up<VarDef>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const up<VarDef>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, vec<optional<int>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const vec<optional<int>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, vec<std::pair<up<Cond>, up<Block>>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const vec<std::pair<up<Cond>, up<Block>>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, vec<string>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const vec<string>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, vec<up<Def>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const vec<up<Def>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, vec<up<Expr>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const vec<up<Expr>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, vec<up<FParDef>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const vec<up<FParDef>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, vec<up<Stmt>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const vec<up<Stmt>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_T_CHAR_CONST: // T_CHAR_CONST
        value.template destroy< char > ();
        break;

      case symbol_kind::S_T_INT_CONST: // T_INT_CONST
        value.template destroy< int > ();
        break;

      case symbol_kind::S_opt_ret_type: // opt_ret_type
        value.template destroy< optional<DataType> > ();
        break;

      case symbol_kind::S_opt_id: // opt_id
        value.template destroy< optional<string> > ();
        break;

      case symbol_kind::S_opt_else: // opt_else
        value.template destroy< optional<up<Block>> > ();
        break;

      case symbol_kind::S_T_STRING_CONST: // T_STRING_CONST
      case symbol_kind::S_T_ID: // T_ID
        value.template destroy< string > ();
        break;

      case symbol_kind::S_block: // block
        value.template destroy< up<Block> > ();
        break;

      case symbol_kind::S_cond: // cond
        value.template destroy< up<Cond> > ();
        break;

      case symbol_kind::S_local_def: // local_def
        value.template destroy< up<Def> > ();
        break;

      case symbol_kind::S_func_call: // func_call
      case symbol_kind::S_expr: // expr
        value.template destroy< up<Expr> > ();
        break;

      case symbol_kind::S_fpar_def: // fpar_def
        value.template destroy< up<FParDef> > ();
        break;

      case symbol_kind::S_fpar_type: // fpar_type
        value.template destroy< up<FParType> > ();
        break;

      case symbol_kind::S_func_decl: // func_decl
        value.template destroy< up<FuncDecl> > ();
        break;

      case symbol_kind::S_func_def: // func_def
        value.template destroy< up<FuncDef> > ();
        break;

      case symbol_kind::S_header: // header
        value.template destroy< up<Header> > ();
        break;

      case symbol_kind::S_l_value: // l_value
        value.template destroy< up<Lval> > ();
        break;

      case symbol_kind::S_program: // program
        value.template destroy< up<Program> > ();
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_assign_stmt: // assign_stmt
      case symbol_kind::S_return_stmt: // return_stmt
      case symbol_kind::S_proc_call: // proc_call
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_loop_stmt: // loop_stmt
        value.template destroy< up<Stmt> > ();
        break;

      case symbol_kind::S_type: // type
        value.template destroy< up<Type> > ();
        break;

      case symbol_kind::S_var_def: // var_def
        value.template destroy< up<VarDef> > ();
        break;

      case symbol_kind::S_type_dims: // type_dims
      case symbol_kind::S_fpar_dims: // fpar_dims
        value.template destroy< vec<optional<int>> > ();
        break;

      case symbol_kind::S_elif_list: // elif_list
        value.template destroy< vec<std::pair<up<Cond>, up<Block>>> > ();
        break;

      case symbol_kind::S_id_list: // id_list
        value.template destroy< vec<string> > ();
        break;

      case symbol_kind::S_localdef_list: // localdef_list
        value.template destroy< vec<up<Def>> > ();
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.template destroy< vec<up<Expr>> > ();
        break;

      case symbol_kind::S_opt_params: // opt_params
      case symbol_kind::S_fpar_list: // fpar_list
        value.template destroy< vec<up<FParDef>> > ();
        break;

      case symbol_kind::S_stmt_list: // stmt_list
        value.template destroy< vec<up<Stmt>> > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, char v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const char& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, int v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const int& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, string v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const string& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    parser (up<Program>& ast_root_yyarg);
    virtual ~parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    parser (const parser&) = delete;
    /// Non copyable.
    parser& operator= (const parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF (location_type l)
      {
        return symbol_type (token::YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYEOF (const location_type& l)
      {
        return symbol_type (token::YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_AND (location_type l)
      {
        return symbol_type (token::T_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_T_AND (const location_type& l)
      {
        return symbol_type (token::T_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_AS (location_type l)
      {
        return symbol_type (token::T_AS, std::move (l));
      }
#else
      static
      symbol_type
      make_T_AS (const location_type& l)
      {
        return symbol_type (token::T_AS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_BEGIN (location_type l)
      {
        return symbol_type (token::T_BEGIN, std::move (l));
      }
#else
      static
      symbol_type
      make_T_BEGIN (const location_type& l)
      {
        return symbol_type (token::T_BEGIN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_BREAK (location_type l)
      {
        return symbol_type (token::T_BREAK, std::move (l));
      }
#else
      static
      symbol_type
      make_T_BREAK (const location_type& l)
      {
        return symbol_type (token::T_BREAK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_BYTE (location_type l)
      {
        return symbol_type (token::T_BYTE, std::move (l));
      }
#else
      static
      symbol_type
      make_T_BYTE (const location_type& l)
      {
        return symbol_type (token::T_BYTE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_CONT (location_type l)
      {
        return symbol_type (token::T_CONT, std::move (l));
      }
#else
      static
      symbol_type
      make_T_CONT (const location_type& l)
      {
        return symbol_type (token::T_CONT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_DECL (location_type l)
      {
        return symbol_type (token::T_DECL, std::move (l));
      }
#else
      static
      symbol_type
      make_T_DECL (const location_type& l)
      {
        return symbol_type (token::T_DECL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_DEF (location_type l)
      {
        return symbol_type (token::T_DEF, std::move (l));
      }
#else
      static
      symbol_type
      make_T_DEF (const location_type& l)
      {
        return symbol_type (token::T_DEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_ELIF (location_type l)
      {
        return symbol_type (token::T_ELIF, std::move (l));
      }
#else
      static
      symbol_type
      make_T_ELIF (const location_type& l)
      {
        return symbol_type (token::T_ELIF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_ELSE (location_type l)
      {
        return symbol_type (token::T_ELSE, std::move (l));
      }
#else
      static
      symbol_type
      make_T_ELSE (const location_type& l)
      {
        return symbol_type (token::T_ELSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_END (location_type l)
      {
        return symbol_type (token::T_END, std::move (l));
      }
#else
      static
      symbol_type
      make_T_END (const location_type& l)
      {
        return symbol_type (token::T_END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_EXIT (location_type l)
      {
        return symbol_type (token::T_EXIT, std::move (l));
      }
#else
      static
      symbol_type
      make_T_EXIT (const location_type& l)
      {
        return symbol_type (token::T_EXIT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_FALSE (location_type l)
      {
        return symbol_type (token::T_FALSE, std::move (l));
      }
#else
      static
      symbol_type
      make_T_FALSE (const location_type& l)
      {
        return symbol_type (token::T_FALSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_IF (location_type l)
      {
        return symbol_type (token::T_IF, std::move (l));
      }
#else
      static
      symbol_type
      make_T_IF (const location_type& l)
      {
        return symbol_type (token::T_IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_IS (location_type l)
      {
        return symbol_type (token::T_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_T_IS (const location_type& l)
      {
        return symbol_type (token::T_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_INT (location_type l)
      {
        return symbol_type (token::T_INT, std::move (l));
      }
#else
      static
      symbol_type
      make_T_INT (const location_type& l)
      {
        return symbol_type (token::T_INT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_LOOP (location_type l)
      {
        return symbol_type (token::T_LOOP, std::move (l));
      }
#else
      static
      symbol_type
      make_T_LOOP (const location_type& l)
      {
        return symbol_type (token::T_LOOP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_NOT (location_type l)
      {
        return symbol_type (token::T_NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_T_NOT (const location_type& l)
      {
        return symbol_type (token::T_NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_OR (location_type l)
      {
        return symbol_type (token::T_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_T_OR (const location_type& l)
      {
        return symbol_type (token::T_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_REF (location_type l)
      {
        return symbol_type (token::T_REF, std::move (l));
      }
#else
      static
      symbol_type
      make_T_REF (const location_type& l)
      {
        return symbol_type (token::T_REF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_RET (location_type l)
      {
        return symbol_type (token::T_RET, std::move (l));
      }
#else
      static
      symbol_type
      make_T_RET (const location_type& l)
      {
        return symbol_type (token::T_RET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_SKIP (location_type l)
      {
        return symbol_type (token::T_SKIP, std::move (l));
      }
#else
      static
      symbol_type
      make_T_SKIP (const location_type& l)
      {
        return symbol_type (token::T_SKIP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_TRUE (location_type l)
      {
        return symbol_type (token::T_TRUE, std::move (l));
      }
#else
      static
      symbol_type
      make_T_TRUE (const location_type& l)
      {
        return symbol_type (token::T_TRUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_VAR (location_type l)
      {
        return symbol_type (token::T_VAR, std::move (l));
      }
#else
      static
      symbol_type
      make_T_VAR (const location_type& l)
      {
        return symbol_type (token::T_VAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_INT_CONST (int v, location_type l)
      {
        return symbol_type (token::T_INT_CONST, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_T_INT_CONST (const int& v, const location_type& l)
      {
        return symbol_type (token::T_INT_CONST, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_CHAR_CONST (char v, location_type l)
      {
        return symbol_type (token::T_CHAR_CONST, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_T_CHAR_CONST (const char& v, const location_type& l)
      {
        return symbol_type (token::T_CHAR_CONST, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_STRING_CONST (string v, location_type l)
      {
        return symbol_type (token::T_STRING_CONST, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_T_STRING_CONST (const string& v, const location_type& l)
      {
        return symbol_type (token::T_STRING_CONST, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_ID (string v, location_type l)
      {
        return symbol_type (token::T_ID, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_T_ID (const string& v, const location_type& l)
      {
        return symbol_type (token::T_ID, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_ASSIGN (location_type l)
      {
        return symbol_type (token::T_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_T_ASSIGN (const location_type& l)
      {
        return symbol_type (token::T_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_NE (location_type l)
      {
        return symbol_type (token::T_NE, std::move (l));
      }
#else
      static
      symbol_type
      make_T_NE (const location_type& l)
      {
        return symbol_type (token::T_NE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_LE (location_type l)
      {
        return symbol_type (token::T_LE, std::move (l));
      }
#else
      static
      symbol_type
      make_T_LE (const location_type& l)
      {
        return symbol_type (token::T_LE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_GE (location_type l)
      {
        return symbol_type (token::T_GE, std::move (l));
      }
#else
      static
      symbol_type
      make_T_GE (const location_type& l)
      {
        return symbol_type (token::T_GE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T_AUTO_END (location_type l)
      {
        return symbol_type (token::T_AUTO_END, std::move (l));
      }
#else
      static
      symbol_type
      make_T_AUTO_END (const location_type& l)
      {
        return symbol_type (token::T_AUTO_END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UPLUS (location_type l)
      {
        return symbol_type (token::UPLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_UPLUS (const location_type& l)
      {
        return symbol_type (token::UPLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UMINUS (location_type l)
      {
        return symbol_type (token::UMINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_UMINUS (const location_type& l)
      {
        return symbol_type (token::UMINUS, l);
      }
#endif


    class context
    {
    public:
      context (const parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    parser (const parser&);
    /// Non copyable.
    parser& operator= (const parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef unsigned char state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;



    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const unsigned char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 283,     ///< Last index in yytable_.
      yynnts_ = 35,  ///< Number of nonterminal symbols.
      yyfinal_ = 6 ///< Termination state number.
    };


    // User arguments.
    up<Program>& ast_root;

  };


#line 3 "frontend/parser/parser.y"
} // dana
#line 2463 "frontend/parser/parser.tab.hh"




#endif // !YY_YY_FRONTEND_PARSER_PARSER_TAB_HH_INCLUDED
