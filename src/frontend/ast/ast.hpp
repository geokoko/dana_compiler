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

class AstVisitor;
class Symbol;
class FuncSymbol;
class VarSymbol;

// Base AST Node class
class ASTNode {
public:
    SourceLoc loc;
    explicit ASTNode(SourceLoc loc);
    virtual ~ASTNode() = 0;
    virtual void accept(AstVisitor& v) = 0;
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
public:
    explicit Expr(SourceLoc loc);
    virtual ~Expr();
    virtual void print(std::ostream &out) const override = 0;
	SemaTypePtr type() const;
	void setType(SemaTypePtr type);
	bool isLValue() const;
	void setLValue(bool v);
	bool isAssignable() const;
	void setAssignable(bool v);
	bool isConstExpr() const;
	void setConstExpr(bool v);
private:
    SemaTypePtr resolvedType_;
    bool isLValue_ = false;
    bool assignable_ = false;
    bool constExpr_ = false;
};

// Statements
class Stmt : public ASTNode {
public:
    explicit Stmt(SourceLoc l);
    virtual ~Stmt() override;
    virtual void print(std::ostream& out) const override = 0;
};

// L-values - the left part of an assignment statement
class Lval : public ASTNode {
public:
    explicit Lval(SourceLoc l);
    virtual ~Lval() override;
    virtual void print(std::ostream& out) const override = 0;
	SemaTypePtr type() const;
	void setType(SemaTypePtr type);
	bool isAssignable() const;
	void setAssignable(bool v);
private:
	SemaTypePtr resolvedType_;
	bool assignable_ = true;
};

// R-Values are expressions - the right part of an assignment statement
class Rval : public Expr {
public:
    explicit Rval(SourceLoc l);
    virtual ~Rval() override = default;
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
    void accept(AstVisitor& v) override;
    virtual void print(std::ostream& out) const override;
};

class FParType : public Type {
public:
    FParType(SourceLoc l, bool ref, DataType type);
    FParType(SourceLoc l, bool ref, DataType type, vec<std::optional<int>> d);

	bool isByRef() const;
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
private:
    bool by_ref;
};

// Blocks
class Block : public ASTNode {
public:
    Block(SourceLoc l, vec<up<Stmt>> stmts);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    const vec<up<Stmt>>& statementsList() const { return statements; }
private:
    vec<up<Stmt>> statements;
};

// Definitions
class Def : public ASTNode {
public:
    explicit Def(SourceLoc l);
    void print(std::ostream& out) const override;
};

class FuncDef; 
// Program root node
class Program : public ASTNode {
public:
    Program(SourceLoc l, up<FuncDef> d);
	void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    FuncDef* definition() const { return top.get(); }
private:
    up<FuncDef> top;
};

// ===== High-level program and definition nodes =====

class FParDef : public Def {
public:
    FParDef(SourceLoc l, vec<string> names, up<FParType> t);
    void accept(AstVisitor& v) override;
	const vec<string>& names() const;
	FParType* parameterType() { return type.get(); }
	const FParType* parameterType() const;
    void print(std::ostream& out) const override;
private:
    vec<string> identifiers;
	up<FParType> type;
};

class Header : public Def {
public:
    Header(SourceLoc l, string n, optional<DataType> r, vec<up<FParDef>> p);

    void accept(AstVisitor& v) override;
	const string& identifier() const;
	const vec<up<FParDef>>& parameters() const;
	optional<DataType> returnType() const;
    FuncSymbol* symbol() const { return symbol_; }
    void setSymbol(FuncSymbol* sym) { symbol_ = sym; }
    void print(std::ostream& out) const override;
private:
    string name;
    optional<DataType> return_type;
    vec<up<FParDef>> params;
	// function symbol initialized with header
    FuncSymbol* symbol_ = nullptr;
};

class VarDef : public Def {
private:
    vec<string> names;
    up<Type> declared_type;
    vec<VarSymbol*> symbols_;

public:
    VarDef(SourceLoc l, vec<string> ids, up<Type> t);

    void accept(AstVisitor& v) override;
    vec<VarSymbol*>& symbols() { return symbols_; }
    const vec<VarSymbol*>& symbols() const { return symbols_; }
    const vec<string>& identifiers() const { return names; }
    Type* declaredType() { return declared_type.get(); }
    const Type* declaredType() const { return declared_type.get(); }
    void print(std::ostream& out) const override;
};

class FuncDecl : public Def {
public:
    explicit FuncDecl(SourceLoc l, up<Header> h);

    void accept(AstVisitor& v) override;
    Header* funcHeader() const { return header.get(); }
    void print(std::ostream& out) const override;
private:
    up<Header> header;
};

class FuncDef : public Def {
public:
    FuncDef(SourceLoc l, up<Header> h, vec<up<Def>> defs, up<Block> b);

    void accept(AstVisitor& v) override;
    Header* funcHeader() const { return header.get(); }
    const vec<up<Def>>& localDefs() const { return locals; }
    Block* funcBody() const { return body.get(); }
    void print(std::ostream& out) const override;
	bool isEntrypoint();
	void setEntrypoint(bool cond);
private:
    up<Header> header;
    vec<up<Def>> locals;
    up<Block> body;
	bool isEntrypoint_;
};

// ===== Blocks and statements =====

class SkipStmt : public Stmt {
public:
    explicit SkipStmt(SourceLoc l);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
};

class ExitStmt : public Stmt {
public:
    explicit ExitStmt(SourceLoc l);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
};

class AssignStmt : public Stmt {
public:
    AssignStmt(SourceLoc l, up<Lval> left, up<Expr> right);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    Lval* left() const { return lhs.get(); }
    Expr* right() const { return rhs.get(); }
private:
    up<Lval> lhs;
    up<Expr> rhs;
};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(SourceLoc l, up<Expr> expr);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    Expr* returnValue() const { return value.get(); }
private:
    up<Expr> value;
};

class ProcCall : public Stmt {
public:
    ProcCall(SourceLoc l, string id, vec<up<Expr>> a);
    FuncSymbol* funcSymbol() const { return symbol_; }
    void setFuncSymbol(FuncSymbol* sym) { symbol_ = sym; }
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    const string& identifier() const { return name; }
    const vec<up<Expr>>& arguments() const { return args; }
private:
    string name;
    vec<up<Expr>> args;
	// store associated symbol (callee)
    FuncSymbol* symbol_ = nullptr;
};

class BreakStmt : public Stmt {
public:
    BreakStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    const optional<string>& loopLabel() const { return label; }
private:
    optional<string> label;
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    const optional<string>& loopLabel() const { return label; }
private:
    optional<string> label;
};

// need to forward declare Cond for IfStmt
class Cond;
class IfStmt : public Stmt {
public:
    IfStmt(SourceLoc l,
           up<Cond> cond,
           up<Block> then_block,
           vec<std::pair<up<Cond>, up<Block>>> elifs,
           std::optional<up<Block>> else_block);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    Cond* conditionExpr() const { return condition.get(); }
    Block* thenBlock() const { return then_branch.get(); }
    const vec<std::pair<up<Cond>, up<Block>>>& elifs() const { return elif_branches; }
    Block* elseBlock() const { return else_branch ? else_branch->get() : nullptr; }
private:
    up<Cond> condition;
    up<Block> then_branch;
    vec<std::pair<up<Cond>, up<Block>>> elif_branches;
    optional<up<Block>> else_branch;
};

class LoopStmt : public Stmt {
private:
	optional<string> label;
    up<Block> body;

public:
    LoopStmt(SourceLoc l, std::optional<string> lbl, up<Block> blk);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    const optional<string>& loopLabel() const { return label; }
    Block* loopBody() const { return body.get(); }
};

// ===== L-values =====

class IdLVal : public Lval {
public:
    IdLVal(SourceLoc l, string id);
    Symbol* symbol() const { return symbol_; }
    void setSymbol(Symbol* sym) { symbol_ = sym; }
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    const string& identifier() const { return name; }
private:
    string name;
	// store associated symbol
    Symbol* symbol_ = nullptr;
};

class StringLiteralLVal : public Lval {
public:
    StringLiteralLVal(SourceLoc l, string v);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    const string& literal() const { return value; }
private:
    string value;
};

class IndexLVal : public Lval {
public:
    IndexLVal(SourceLoc l, up<Lval> b, up<Expr> idx);
    void print(std::ostream& out) const override;
	void accept(AstVisitor& v) override;
    Lval* baseExpr() const { return base.get(); }
    Expr* indexExpr() const { return index.get(); }
private:
    up<Lval> base;
    up<Expr> index;
};

// ===== R-values =====

class IntConst : public Rval {
private:
    int value;

public:
    IntConst(SourceLoc l, int v);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    int getValue() const { return value; }
};

class CharConst : public Rval {
private:
    unsigned char value;

public:
    CharConst(SourceLoc l, unsigned char v);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    unsigned char getValue() const { return value; }
};

class TrueConst : public Rval {
public:
    explicit TrueConst(SourceLoc l);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
};

class FalseConst : public Rval {
public:
    explicit FalseConst(SourceLoc l);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
};

// ===== Expressions =====

class LValueExpr : public Expr {
public:
    LValueExpr(SourceLoc l, up<Lval> val);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    Lval* lvalue() const { return value.get(); }
private:
    up<Lval> value;
};

class ParenExpr : public Expr {
public:
    ParenExpr(SourceLoc l, up<Expr> expr);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    Expr* innerExpr() const { return inner.get(); }
private:
    up<Expr> inner;
};

class FuncCall : public Expr {
public:
    FuncCall(SourceLoc l, string id, vec<up<Expr>> a);
    const FuncSymbol* funcSymbol() const { return symbol_; }
    void setFuncSymbol(FuncSymbol* sym) { symbol_ = sym; }
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    const string& identifier() const { return name; }
    const vec<up<Expr>>& arguments() const { return args; }
private:
    string name;
    vec<up<Expr>> args;
	// store associated symbol (callee)
    FuncSymbol* symbol_ = nullptr;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(SourceLoc l, UnOp operation, up<Expr> expr);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    UnOp opKind() const { return op; }
    Expr* operandExpr() const { return operand.get(); }
private:
    UnOp op;
    up<Expr> operand;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(SourceLoc l, BinOp operation, up<Expr> left, up<Expr> right);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    BinOp opKind() const { return op; }
    Expr* leftExpr() const { return lhs.get(); }
    Expr* rightExpr() const { return rhs.get(); }
private:
    BinOp op;
    up<Expr> lhs;
    up<Expr> rhs;
};

// ===== Conditions =====

class Cond : public Expr {
public:
    explicit Cond(SourceLoc l);
    ~Cond() override = default;
    virtual void print(std::ostream& out) const override = 0;
};

class ExprCond : public Cond {
public:
    ExprCond(SourceLoc l, up<Expr> e);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    Expr* expression() const { return expr.get(); }
private:
    up<Expr> expr;
};

class ParenCond : public Cond {
public:
    ParenCond(SourceLoc l, up<Cond> c);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    Cond* conditionExpr() const { return condition.get(); }
private:
    up<Cond> condition;
};

class NotCond : public Cond {
public:
    NotCond(SourceLoc l, up<Cond> c);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    Cond* conditionExpr() const { return condition.get(); }
private:
    up<Cond> condition;
};

class BinaryCond : public Cond {
public:
    BinaryCond(SourceLoc l, LogicOp operation, up<Cond> left, up<Cond> right);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    LogicOp opKind() const { return op; }
    Cond* leftCond() const { return lhs.get(); }
    Cond* rightCond() const { return rhs.get(); }
private:
    LogicOp op;
    up<Cond> lhs;
    up<Cond> rhs;
};

class RelCond : public Cond {
public:
    RelCond(SourceLoc l, RelOp operation, up<Expr> left, up<Expr> right);
    void accept(AstVisitor& v) override;
    void print(std::ostream& out) const override;
    RelOp opKind() const { return op; }
    Expr* leftExpr() const { return lhs.get(); }
    Expr* rightExpr() const { return rhs.get(); }
private:
    RelOp op;
    up<Expr> lhs;
    up<Expr> rhs;
};
