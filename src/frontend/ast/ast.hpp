#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <utility>

#include "../common/source_location.hpp"
#include "../common/types.hpp"
#include "../symbol/sematype.hpp"
#include "operators.hpp"

using std::string;
using std::make_unique;
using std::optional;
using std::move;
template <class T> using up = std::unique_ptr<T>;
template <class T> using vec = std::vector<T>;

class SemContext;
class Codegen;
class Symbol;
class FuncSymbol;
class VarSymbol;

// Base AST Node class
class ASTNode {
public:
    SourceLoc loc;
    explicit ASTNode(SourceLoc loc);
    virtual ~ASTNode() = 0;
    virtual void sem(SemContext& context) = 0;
    virtual void agen(Codegen& v) = 0;
    virtual void print(std::ostream &out) const = 0;
};

// Overload << operator for printing AST nodes
inline std::ostream& operator<<(std::ostream& out, const ASTNode& node) {
    node.print(out);
    return out;
}

/* Base classes in the hierarchy, directly derived from ASTNode 
 * Each node class stores certain attributes. The pointer attributes (up=unique_ptr)
 * are the actual children nodes of a node in the AST. The other are just attributes
 * that uniquely define an AST node.
 * */

// Expression nodes
class Expr : public ASTNode {
protected:
    SemaTypePtr resolvedType_;
    bool isLValue_ = false;
    bool assignable_ = false;
    bool constExpr_ = false;
public:
    explicit Expr(SourceLoc loc);
    virtual ~Expr();
    virtual void sem(SemContext& context) override = 0;
    virtual void print(std::ostream &out) const override = 0;
	SemaTypePtr type() const;
	void setType(SemaTypePtr type);
	bool isLValue() const;
	void setLValue(bool v);
	bool isAssignable() const;
	void setAssignable(bool v);
	bool isConstExpr() const;
	void setConstExpr(bool v);
};

// Statements
class Stmt : public ASTNode {
public:
    explicit Stmt(SourceLoc l);
    virtual ~Stmt() override;
    void sem(SemContext& context) override = 0;
    virtual void print(std::ostream& out) const override = 0;
};

// L-values - the left part of an assignment statement
class Lval : public ASTNode {
protected:
	SemaTypePtr resolvedType_;
	bool assignable_ = true;
public:
    explicit Lval(SourceLoc l);
    virtual ~Lval() override;
    void sem(SemContext& context) override = 0;
    virtual void print(std::ostream& out) const override = 0;
	SemaTypePtr type() const;
	void setType(SemaTypePtr type);
	bool isAssignable() const;
	void setAssignable(bool v);
};

// R-Values are expressions - the right part of an assignment statement
class Rval : public Expr {
protected:
	bool assignable_ = false;
public:
    explicit Rval(SourceLoc l);
    virtual ~Rval() override = default;
    void sem(SemContext& context) override = 0;
    virtual void print(std::ostream& out) const override = 0;
};

// Types
class Type : public ASTNode {
protected:
    DataType base;
    vec<std::optional<int>> dims; // dimensions for array types
    void printTypeDetails(std::ostream& out) const;

public:
    Type(SourceLoc l, DataType b, vec<std::optional<int>> d = {});
    virtual ~Type() override = default;
	DataType data_type() const;
    const vec<std::optional<int>>& dimensions() const;

    virtual void sem(SemContext& context) override;
    virtual void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class FParType : public Type {
protected:
    bool by_ref;

public:
    FParType(SourceLoc l, bool ref, DataType type);
    FParType(SourceLoc l, bool ref, DataType type, vec<std::optional<int>> d);

	bool isByRef() const;
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

// Blocks
class Block : public ASTNode {
protected:
    vec<up<Stmt>> statements;

public:
    Block(SourceLoc l, vec<up<Stmt>> stmts);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

// Definitions
class Def : public ASTNode {
public:
    explicit Def(SourceLoc l);
    virtual void sem(SemContext& context) override = 0;
    void print(std::ostream& out) const override;
};

class FuncDef; 
// Program root node
class Program : public ASTNode {
protected:
    up<FuncDef> top;

public:
    Program(SourceLoc l, up<FuncDef> d);
	void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

// ===== High-level program and definition nodes =====

class FParDef : public Def {
protected:
    vec<string> identifiers;
	up<FParType> type;

public:
    FParDef(SourceLoc l, vec<string> names, up<FParType> t);
    void sem(SemContext& context) override;
	const vec<string>& names() const;
	const FParType* parameterType() const;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class Header : public Def {
protected:
    string name;
    optional<DataType> return_type;
    vec<up<FParDef>> params;
	// function symbol initialized with header
    FuncSymbol* symbol_ = nullptr;

public:
    Header(SourceLoc l, string n, optional<DataType> r, vec<up<FParDef>> p);

    void sem(SemContext& context) override;
	const string& identifier() const;
	const vec<up<FParDef>>& parameters() const;
	optional<DataType> returnType() const;
    FuncSymbol* symbol() const { return symbol_; }
    void setSymbol(FuncSymbol* sym) { symbol_ = sym; }
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class VarDef : public Def {
private:
    vec<string> names;
    up<Type> declared_type;
    vec<VarSymbol*> symbols_;

public:
    VarDef(SourceLoc l, vec<string> ids, up<Type> t);

    void sem(SemContext& context) override;
    const vec<VarSymbol*>& symbols() const { return symbols_; }
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class FuncDecl : public Def {
protected:
    up<Header> header;

public:
    explicit FuncDecl(SourceLoc l, up<Header> h);

    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class FuncDef : public Def {
protected:
    up<Header> header;
    vec<up<Def>> locals;
    up<Block> body;

public:
    FuncDef(SourceLoc l, up<Header> h, vec<up<Def>> defs, up<Block> b);

    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

// ===== Blocks and statements =====

class SkipStmt : public Stmt {
public:
    explicit SkipStmt(SourceLoc l);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class ExitStmt : public Stmt {
public:
    explicit ExitStmt(SourceLoc l);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class AssignStmt : public Stmt {
protected:
    up<Lval> lhs;
    up<Expr> rhs;

public:
    AssignStmt(SourceLoc l, up<Lval> left, up<Expr> right);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

class ReturnStmt : public Stmt {
protected:
    up<Expr> value;

public:
    ReturnStmt(SourceLoc l, up<Expr> expr);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

class ProcCall : public Stmt {
protected:
    string name;
    vec<up<Expr>> args;
	// store associated symbol (callee)
    Symbol* symbol_ = nullptr;

public:
    ProcCall(SourceLoc l, string id, vec<up<Expr>> a);
    Symbol* symbol() const { return symbol_; }
    void setSymbol(Symbol* sym) { symbol_ = sym; }
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

class BreakStmt : public Stmt {
protected:
    optional<string> label;

public:
    BreakStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

class ContinueStmt : public Stmt {
protected:
    optional<string> label;

public:
    ContinueStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

// need to forward declare Cond for IfStmt
class Cond;
class IfStmt : public Stmt {
protected:
    up<Cond> condition;
    up<Block> then_branch;
    vec<std::pair<up<Cond>, up<Block>>> elif_branches;
    optional<up<Block>> else_branch;

public:
    IfStmt(SourceLoc l,
           up<Cond> cond,
           up<Block> then_block,
           vec<std::pair<up<Cond>, up<Block>>> elifs,
           std::optional<up<Block>> else_block);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

class LoopStmt : public Stmt {
public:
    optional<string> label;
    up<Block> body;

    LoopStmt(SourceLoc l, std::optional<string> lbl, up<Block> blk);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

// ===== L-values =====

class IdLVal : public Lval {
private:
    string name;
	// store associated symbol
    Symbol* symbol_ = nullptr;

public:
    IdLVal(SourceLoc l, string id);
    Symbol* symbol() const { return symbol_; }
    void setSymbol(Symbol* sym) { symbol_ = sym; }
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

class StringLiteralLVal : public Lval {
private:
    string value;

public:
    StringLiteralLVal(SourceLoc l, string v);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

class IndexLVal : public Lval {
protected:
    up<Lval> base;
    up<Expr> index;

public:
    IndexLVal(SourceLoc l, up<Lval> b, up<Expr> idx);
    void print(std::ostream& out) const override;
	void sem(SemContext& context) override;
    void agen(Codegen& v) override;
};

// ===== R-values =====

class IntConst : public Rval {
private:
    int value;

public:
    IntConst(SourceLoc l, int v);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class CharConst : public Rval {
private:
    unsigned char value;

public:
    CharConst(SourceLoc l, unsigned char v);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class TrueConst : public Rval {
public:
    explicit TrueConst(SourceLoc l);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class FalseConst : public Rval {
public:
    explicit FalseConst(SourceLoc l);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

// ===== Expressions =====

class LValueExpr : public Expr {
protected:
    up<Lval> value;

public:
    LValueExpr(SourceLoc l, up<Lval> val);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class ParenExpr : public Expr {
protected:
    up<Expr> inner;

public:
    ParenExpr(SourceLoc l, up<Expr> expr);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class FuncCall : public Expr {
protected:
    string name;
    vec<up<Expr>> args;
	// store associated symbol (callee)
    Symbol* symbol_ = nullptr;

public:
    FuncCall(SourceLoc l, string id, vec<up<Expr>> a);
    Symbol* symbol() const { return symbol_; }
    void setSymbol(Symbol* sym) { symbol_ = sym; }
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class UnaryExpr : public Expr {
protected:
    UnOp op;
    up<Expr> operand;

public:
    UnaryExpr(SourceLoc l, UnOp operation, up<Expr> expr);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class BinaryExpr : public Expr {
protected:
    BinOp op;
    up<Expr> lhs;
    up<Expr> rhs;

public:
    BinaryExpr(SourceLoc l, BinOp operation, up<Expr> left, up<Expr> right);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

// ===== Conditions =====

class Cond : public Expr {
public:
    explicit Cond(SourceLoc l);
    ~Cond() override = default;
    void sem(SemContext& context) override;
    virtual void print(std::ostream& out) const override = 0;
    void agen(Codegen& v) override = 0;
};

class ExprCond : public Cond {
protected:
    up<Expr> expr;

public:
    ExprCond(SourceLoc l, up<Expr> e);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class ParenCond : public Cond {
protected:
    up<Cond> condition;

public:
    ParenCond(SourceLoc l, up<Cond> c);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class NotCond : public Cond {
protected:
    up<Cond> condition;

public:
    NotCond(SourceLoc l, up<Cond> c);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class BinaryCond : public Cond {
protected:
    LogicOp op;
    up<Cond> lhs;
    up<Cond> rhs;

public:
    BinaryCond(SourceLoc l, LogicOp operation, up<Cond> left, up<Cond> right);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};

class RelCond : public Cond {
protected:
    RelOp op;
    up<Expr> lhs;
    up<Expr> rhs;

public:
    RelCond(SourceLoc l, RelOp operation, up<Expr> left, up<Expr> right);
    void sem(SemContext& context) override;
    void print(std::ostream& out) const override;
    void agen(Codegen& v) override;
};
