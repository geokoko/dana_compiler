#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <variant>
#include <utility>
#include <cstdint>
#include <cstddef>

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

// Base AST Node class
class ASTNode {
public:
    SourceLoc loc;
    explicit ASTNode(SourceLoc loc);
    virtual ~ASTNode() = default;
    virtual void sem(SemContext& ctx);
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
    virtual ~Expr() = default;
    virtual void sem(SemContext& ctx) override = 0;
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
    virtual ~Stmt() override = default;
    void sem(SemContext& ctx) override = 0;
    virtual void print(std::ostream& out) const override = 0;
};

// L-values - the left part of an assignment statement
class Lval : public ASTNode {
protected:
	SemaTypePtr resolvedType_;
	bool assignable_ = true;
public:
    explicit Lval(SourceLoc l);
    virtual ~Lval() override = default;
    void sem(SemContext& ctx) override = 0;
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
    void sem(SemContext& ctx) override = 0;
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

    virtual void sem(SemContext& ctx) override = 0;
    virtual void print(std::ostream& out) const override;
};

class FParType : public Type {
protected:
    bool by_ref;

public:
    FParType(SourceLoc l, bool ref, DataType type);
    FParType(SourceLoc l, bool ref, DataType type, vec<std::optional<int>> d);

	bool isByRef() const;
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

// Blocks
class Block : public ASTNode {
protected:
    vec<up<Stmt>> statements;

public:
    Block(SourceLoc l, vec<up<Stmt>> stmts);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

// Definitions
class Def : public ASTNode {
public:
    explicit Def(SourceLoc l);
    virtual void sem(SemContext& ctx) override = 0;
    void print(std::ostream& out) const override;
};

// Program root node
class Program : public ASTNode {
protected:
    up<Def> top;

public:
    Program(SourceLoc l, up<Def> d);
	void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

// ===== High-level program and definition nodes =====

class FParDef : public Def {
protected:
    vec<string> identifiers;
	up<FParType> type;

public:
    FParDef(SourceLoc l, vec<string> names, up<FParType> t);
    void sem(SemContext& ctx) override;
	const vec<string>& names() const;
	const FParType* parameterType() const;
    void print(std::ostream& out) const override;
};

class Header : public Def {
protected:
    string name;
    optional<DataType> return_type;
    vec<up<FParDef>> params;

public:
    Header(SourceLoc l, string n, optional<DataType> r, vec<up<FParDef>> p);

    void sem(SemContext& ctx) override;
	const string& identifier() const;
	const vec<up<FParDef>>& parameters() const;
    void print(std::ostream& out) const override;
};

class VarDef : public Def {
private:
    vec<string> names;
    up<Type> declared_type;

public:
    VarDef(SourceLoc l, vec<string> ids, up<Type> t);

    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class FuncDecl : public Def {
protected:
    up<Header> header;

public:
    explicit FuncDecl(SourceLoc l, up<Header> h);

    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class FuncDef : public Def {
protected:
    up<Header> header;
    vec<up<Def>> locals;
    up<Block> body;

public:
    FuncDef(SourceLoc l, up<Header> h, vec<up<Def>> defs, up<Block> b);

    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

// ===== Blocks and statements =====

class SkipStmt : public Stmt {
public:
    explicit SkipStmt(SourceLoc l);
    void print(std::ostream& out) const override;
};

class ExitStmt : public Stmt {
public:
    explicit ExitStmt(SourceLoc l);
    void print(std::ostream& out) const override;
};

class AssignStmt : public Stmt {
protected:
    up<Lval> lhs;
    up<Expr> rhs;

public:
    AssignStmt(SourceLoc l, up<Lval> left, up<Expr> right);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

class ReturnStmt : public Stmt {
protected:
    up<Expr> value;

public:
    ReturnStmt(SourceLoc l, up<Expr> expr);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

class ProcCall : public Stmt {
protected:
    string name;
    vec<up<Expr>> args;

public:
    ProcCall(SourceLoc l, string id, vec<up<Expr>> a);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

class BreakStmt : public Stmt {
protected:
    optional<string> label;

public:
    BreakStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

class ContinueStmt : public Stmt {
protected:
    optional<string> label;

public:
    ContinueStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
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
	void sem(SemContext& ctx) override;
};

class LoopStmt : public Stmt {
public:
    optional<string> label;
    up<Block> body;

    LoopStmt(SourceLoc l, std::optional<string> lbl, up<Block> blk);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

// ===== L-values =====

class IdLVal : public Lval {
private:
    string name;

public:
    IdLVal(SourceLoc l, string id);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

class StringLiteralLVal : public Lval {
private:
    string value;

public:
    StringLiteralLVal(SourceLoc l, string v);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

class IndexLVal : public Lval {
protected:
    up<Lval> base;
    up<Expr> index;

public:
    IndexLVal(SourceLoc l, up<Lval> b, up<Expr> idx);
    void print(std::ostream& out) const override;
	void sem(SemContext& ctx) override;
};

// ===== R-values =====

class IntConst : public Rval {
private:
    int value;

public:
    IntConst(SourceLoc l, int v);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class CharConst : public Rval {
private:
    unsigned char value;

public:
    CharConst(SourceLoc l, unsigned char v);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class TrueConst : public Rval {
public:
    explicit TrueConst(SourceLoc l);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class FalseConst : public Rval {
public:
    explicit FalseConst(SourceLoc l);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

// ===== Expressions =====

class LValueExpr : public Expr {
protected:
    up<Lval> value;

public:
    LValueExpr(SourceLoc l, up<Lval> val);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class ParenExpr : public Expr {
protected:
    up<Expr> inner;

public:
    ParenExpr(SourceLoc l, up<Expr> expr);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class FuncCall : public Expr {
protected:
    string name;
    vec<up<Expr>> args;

public:
    FuncCall(SourceLoc l, string id, vec<up<Expr>> a);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class UnaryExpr : public Expr {
protected:
    UnOp op;
    up<Expr> operand;

public:
    UnaryExpr(SourceLoc l, UnOp operation, up<Expr> expr);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

class BinaryExpr : public Expr {
protected:
    BinOp op;
    up<Expr> lhs;
    up<Expr> rhs;

public:
    BinaryExpr(SourceLoc l, BinOp operation, up<Expr> left, up<Expr> right);
    void sem(SemContext& ctx) override;
    void print(std::ostream& out) const override;
};

// ===== Conditions =====

class Cond : public Expr {
public:
    explicit Cond(SourceLoc l);
    ~Cond() override = default;
    void sem(SemContext& ctx) override;
    virtual void print(std::ostream& out) const override = 0;
};

class ExprCond : public Cond {
protected:
    up<Expr> expr;

public:
    ExprCond(SourceLoc l, up<Expr> e);
    void print(std::ostream& out) const override;
};

class ParenCond : public Cond {
protected:
    up<Cond> condition;

public:
    ParenCond(SourceLoc l, up<Cond> c);
    void print(std::ostream& out) const override;
};

class NotCond : public Cond {
protected:
    up<Cond> condition;

public:
    NotCond(SourceLoc l, up<Cond> c);
    void print(std::ostream& out) const override;
};

class BinaryCond : public Cond {
protected:
    LogicOp op;
    up<Cond> lhs;
    up<Cond> rhs;

public:
    BinaryCond(SourceLoc l, LogicOp operation, up<Cond> left, up<Cond> right);
    void print(std::ostream& out) const override;
};

class RelCond : public Cond {
protected:
    RelOp op;
    up<Expr> lhs;
    up<Expr> rhs;

public:
    RelCond(SourceLoc l, RelOp operation, up<Expr> left, up<Expr> right);
    void print(std::ostream& out) const override;
};
