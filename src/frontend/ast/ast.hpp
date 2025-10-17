#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <variant>
#include <utility>
#include <cstdint>

using std::unique_ptr;
using std::make_unique;
template <class T> using up = std::unique_ptr<T>;
template <class T> using vec = std::vector<T>;

// enum classes (primitive types and operation types)
enum class DataType { Int, Byte };
enum class UnOp { Plus, Minus, Not };
enum class BinOp { Add, Sub, Mul, Div, Mod, AndBits, OrBits };
enum class RelOp { Eq, Ne, Le, Ge, Lt, Gt };
enum class LogicOp { And, Or };

// Tracking location in code
struct SourceLoc {
	int line = 0;
	int col = 0;
};

// Base AST Node class
class ASTNode {
public:
	SourceLoc loc;
	ASTNode (SourceLoc loc) : loc(loc) {}
	virtual ~ASTNode() = default;
	virtual void sem () {}
	virtual void print(std::ostream &out) const = 0;
};

// Overload << operator for printing AST nodes
inline std::ostream& operator << (std::ostream& out, const ASTNode& node) {
	node.print(out);
	return out;
}

/* Base classes in the hierarchy, directly derived from ASTNode */
// Program
class Program : public ASTNode {
public:
    Program(SourceLoc l, up<FuncDef>) : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

// Expression nodes
class Expr : public ASTNode {
public:
    Expr (SourceLoc loc) : ASTNode (loc) {}
    virtual ~Expr() = default;
    virtual void sem() override = 0;
    virtual void print(std::ostream &out) const = 0;
};

// Statements
class Stmt : public ASTNode {
public:
    virtual ~Stmt() override = default;
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override = 0;
};

// L-values
class Lval : public ASTNode {
public:
    virtual ~Lval() override = default;
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override = 0;
};

// Types
class Type : public ASTNode {
public:
    Type(SourceLoc l, DataType, vec<std::optional<int>>)
        : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

// Blocks
class Block : public ASTNode {
public:
    Block(SourceLoc l, vec<up<Stmt>>) : ASTNode(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

// Definitions
class Def : public ASTNode {
public:
	Def(SourceLoc l) : ASTNode(l) {}
	virtual void sem() override {}
	virtual void print(std::ostream& out) const override {}
};

// Conditions
class Cond : public Expr {
public:
    virtual ~Cond() override = default;
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override = 0;
};

// ===== High-level program and definition nodes =====
class FParType : public ASTNode {
public:
    FParType(SourceLoc l, bool, DataType)
        : ASTNode(l) {}

    FParType(SourceLoc l, bool, DataType, vec<std::optional<int>>)
        : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class FParDef : public ASTNode {
public:
    FParDef(SourceLoc l, vec<std::string>, up<FParType>)
        : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class Header : public ASTNode {
public:
    Header(SourceLoc l, std::string, std::optional<DataType>, vec<up<FParDef>>)
        : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class VarDef : public ASTNode {
public:
    VarDef(SourceLoc l, vec<std::string>, up<Type>)
        : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class FuncDecl : public ASTNode {
public:
    explicit FuncDecl(SourceLoc l, up<Header>)
        : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class FuncDef : public ASTNode {
public:
    FuncDef(SourceLoc l, up<Header>, vec<up<Def>>, up<Block>)
        : ASTNode(l) {}

    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

// ===== Blocks and statements =====

class SkipStmt : public Stmt {
public:
    explicit SkipStmt(SourceLoc l) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class ExitStmt : public Stmt {
public:
    explicit ExitStmt(SourceLoc l) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class AssignStmt : public Stmt {
public:
    AssignStmt(SourceLoc l, up<Lval>, up<Expr>) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(SourceLoc l, up<Expr>) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class ProcCall : public Stmt {
public:
    ProcCall(SourceLoc l, std::string, vec<up<Expr>>) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class BreakStmt : public Stmt {
public:
    BreakStmt(SourceLoc l, std::optional<std::string>) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt(SourceLoc l, std::optional<std::string>) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class IfStmt : public Stmt {
public:
    IfStmt(SourceLoc l,
           up<Cond>,
           up<Block>,
           vec<std::pair<up<Cond>, up<Block>>>,
           std::optional<up<Block>>) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

class LoopStmt : public Stmt {
public:
    LoopStmt(SourceLoc l, std::optional<std::string>, up<Block>) : Stmt(l) {}
    virtual void print(std::ostream& out) const override {}
};

// ===== L-values =====

class IdLVal : public Lval {
public:
    IdLVal(SourceLoc l, std::string) : Lval(l) {}
    virtual void print(std::ostream& out) const override {}
};

class StringLiteralLVal : public Lval {
public:
    StringLiteralLVal(SourceLoc l, std::string) : Lval(l) {}
    virtual void print(std::ostream& out) const override {}
};

class IndexLVal : public Lval {
public:
    IndexLVal(SourceLoc l, up<Lval>, up<Expr>) : Lval(l) {}
    virtual void print(std::ostream& out) const override {}
};

// ===== Expressions =====

class IntConst : public Expr {
public:
    IntConst(SourceLoc l, int) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class CharConst : public Expr {
public:
    CharConst(SourceLoc l, unsigned char) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class TrueConst : public Expr {
public:
    explicit TrueConst(SourceLoc l) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class FalseConst : public Expr {
public:
    explicit FalseConst(SourceLoc l) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class LValueExpr : public Expr {
public:
    LValueExpr(SourceLoc l, up<Lval>) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class ParenExpr : public Expr {
public:
    ParenExpr(SourceLoc l, up<Expr>) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class FuncCall : public Expr {
public:
    FuncCall(SourceLoc l, std::string, vec<up<Expr>>) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(SourceLoc l, UnOp, up<Expr>) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(SourceLoc l, BinOp, up<Expr>, up<Expr>) : Expr(l) {}
    virtual void sem() override {}
    virtual void print(std::ostream& out) const override {}
};

// ===== Conditions =====

class ExprCond : public Cond {
public:
    ExprCond(SourceLoc l, up<Expr>) : Cond(l) {}
    virtual void print(std::ostream& out) const override {}
};

class ParenCond : public Cond {
public:
    ParenCond(SourceLoc l, up<Cond>) : Cond(l) {}
    virtual void print(std::ostream& out) const override {}
};

class NotCond : public Cond {
public:
    NotCond(SourceLoc l, up<Cond>) : Cond(l) {}
    virtual void print(std::ostream& out) const override {}
};

class BinaryCond : public Cond {
public:
    BinaryCond(SourceLoc l, LogicOp, up<Cond>, up<Cond>) : Cond(l) {}
    virtual void print(std::ostream& out) const override {}
};

class RelCond : public Cond {
public:
    RelCond(SourceLoc l, RelOp, up<Expr>, up<Expr>) : Cond(l) {}
    virtual void print(std::ostream& out) const override {}
};
