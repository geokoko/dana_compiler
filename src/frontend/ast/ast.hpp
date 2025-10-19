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

using std::string;
using std::make_unique;
using std::optional;
using std::move;
template <class T> using up = std::unique_ptr<T>;
template <class T> using vec = std::vector<T>;

// enum classes (primitive types and operation types)
enum class DataType { Int, Byte };
enum class UnOp { Plus, Minus, Not };
enum class BinOp { Add, Sub, Mul, Div, Mod, AndBits, OrBits };
enum class RelOp { Eq, Ne, Le, Ge, Lt, Gt };
enum class LogicOp { And, Or };

inline const char* dataTypeName(DataType type) {
    switch (type) {
        case DataType::Int: return "int";
        case DataType::Byte: return "byte";
    }
    return "unknown";
}

inline const char* unOpName(UnOp op) {
    switch (op) {
        case UnOp::Plus: return "+";
        case UnOp::Minus: return "-";
        case UnOp::Not: return "!";
    }
    return "?";
}

inline const char* binOpName(BinOp op) {
    switch (op) {
        case BinOp::Add: return "+";
        case BinOp::Sub: return "-";
        case BinOp::Mul: return "*";
        case BinOp::Div: return "/";
        case BinOp::Mod: return "%";
        case BinOp::AndBits: return "&";
        case BinOp::OrBits: return "|";
    }
    return "?";
}

inline const char* relOpName(RelOp op) {
    switch (op) {
        case RelOp::Eq: return "=";
        case RelOp::Ne: return "<>";
        case RelOp::Le: return "<=";
        case RelOp::Ge: return ">=";
        case RelOp::Lt: return "<";
        case RelOp::Gt: return ">";
    }
    return "?";
}

inline const char* logicOpName(LogicOp op) {
    switch (op) {
        case LogicOp::And: return "and";
        case LogicOp::Or: return "or";
    }
    return "?";
}

// Tracking location in code
struct SourceLoc {
    int line = 0;
    int col = 0;
};

// Base AST Node class
class ASTNode {
public:
    SourceLoc loc;
    explicit ASTNode(SourceLoc loc);
    virtual ~ASTNode() = default;
    virtual void sem();
    virtual void print(std::ostream &out) const = 0;
};

// Overload << operator for printing AST nodes
inline std::ostream& operator<<(std::ostream& out, const ASTNode& node) {
    node.print(out);
    return out;
}

/* Base classes in the hierarchy, directly derived from ASTNode */

// Expression nodes
class Expr : public ASTNode {
public:
    explicit Expr(SourceLoc loc);
    virtual ~Expr() = default;
    virtual void sem() override = 0;
    virtual void print(std::ostream &out) const override = 0;
};

// Statements
class Stmt : public ASTNode {
public:
    explicit Stmt(SourceLoc l);
    virtual ~Stmt() override = default;
    void sem() override;
    virtual void print(std::ostream& out) const override = 0;
};

// L-values
class Lval : public ASTNode {
public:
    explicit Lval(SourceLoc l);
    virtual ~Lval() override = default;
    void sem() override;
    virtual void print(std::ostream& out) const override = 0;
};

// R-Values are expressions
class Rval : public Expr {
public:
    explicit Rval(SourceLoc l);
    virtual ~Rval() override = default;
    void sem() override;
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
    DataType data_type() const;
    const vec<std::optional<int>>& dimensions() const;

    void sem() override;
    void print(std::ostream& out) const override;
};

// Blocks
class Block : public ASTNode {
protected:
    vec<up<Stmt>> statements;

public:
    Block(SourceLoc l, vec<up<Stmt>> stmts);
    void sem() override;
    void print(std::ostream& out) const override;
};

// Definitions
class Def : public ASTNode {
public:
    explicit Def(SourceLoc l);
    void sem() override;
    void print(std::ostream& out) const override;
};

// Program root node
class Program : public ASTNode {
protected:
    up<Def> top;

public:
    Program(SourceLoc l, up<Def> d);
    void print(std::ostream& out) const override;
};

// ===== High-level program and definition nodes =====
class FParType : public Type {
protected:
    bool by_ref;

public:
    FParType(SourceLoc l, bool ref, DataType type);
    FParType(SourceLoc l, bool ref, DataType type, vec<std::optional<int>> d);

    void sem() override;
    void print(std::ostream& out) const override;
};

class FParDef : public Def {
protected:
    vec<string> identifiers;
    up<FParType> type;

public:
    FParDef(SourceLoc l, vec<string> names, up<FParType> t);

    void sem() override;
    void print(std::ostream& out) const override;
};

class Header : public Def {
protected:
    string name;
    optional<DataType> return_type;
    vec<up<FParDef>> params;

public:
    Header(SourceLoc l, string n, optional<DataType> r, vec<up<FParDef>> p);

    void sem() override;
    void print(std::ostream& out) const override;
};

class VarDef : public Def {
protected:
    vec<string> names;
    up<Type> declared_type;

public:
    VarDef(SourceLoc l, vec<string> ids, up<Type> t);

    void sem() override;
    void print(std::ostream& out) const override;
};

class FuncDecl : public Def {
protected:
    up<Header> header;

public:
    explicit FuncDecl(SourceLoc l, up<Header> h);

    void sem() override;
    void print(std::ostream& out) const override;
};

class FuncDef : public Def {
protected:
    up<Header> header;
    vec<up<Def>> locals;
    up<Block> body;

public:
    FuncDef(SourceLoc l, up<Header> h, vec<up<Def>> defs, up<Block> b);

    void sem() override;
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
};

class ReturnStmt : public Stmt {
protected:
    up<Expr> value;

public:
    ReturnStmt(SourceLoc l, up<Expr> expr);
    void print(std::ostream& out) const override;
};

class ProcCall : public Stmt {
protected:
    string name;
    vec<up<Expr>> args;

public:
    ProcCall(SourceLoc l, string id, vec<up<Expr>> a);
    void print(std::ostream& out) const override;
};

class BreakStmt : public Stmt {
protected:
    optional<string> label;

public:
    BreakStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
};

class ContinueStmt : public Stmt {
protected:
    optional<string> label;

public:
    ContinueStmt(SourceLoc l, optional<string> lbl);
    void print(std::ostream& out) const override;
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
};

class LoopStmt : public Stmt {
public:
    optional<string> label;
    up<Block> body;

    LoopStmt(SourceLoc l, std::optional<string> lbl, up<Block> blk);
    void print(std::ostream& out) const override;
};

// ===== L-values =====

class IdLVal : public Lval {
private:
    string name;

public:
    IdLVal(SourceLoc l, string id);
    void print(std::ostream& out) const override;
};

class StringLiteralLVal : public Lval {
private:
    string value;

public:
    StringLiteralLVal(SourceLoc l, string v);
    void print(std::ostream& out) const override;
};

class IndexLVal : public Lval {
protected:
    up<Lval> base;
    up<Expr> index;

public:
    IndexLVal(SourceLoc l, up<Lval> b, up<Expr> idx);
    void print(std::ostream& out) const override;
};

// ===== R-values =====

class IntConst : public Rval {
private:
    int value;

public:
    IntConst(SourceLoc l, int v);
    void sem() override;
    void print(std::ostream& out) const override;
};

class CharConst : public Rval {
private:
    unsigned char value;

public:
    CharConst(SourceLoc l, unsigned char v);
    void sem() override;
    void print(std::ostream& out) const override;
};

class TrueConst : public Rval {
public:
    explicit TrueConst(SourceLoc l);
    void sem() override;
    void print(std::ostream& out) const override;
};

class FalseConst : public Rval {
public:
    explicit FalseConst(SourceLoc l);
    void sem() override;
    void print(std::ostream& out) const override;
};

// ===== Expressions =====

class LValueExpr : public Expr {
protected:
    up<Lval> value;

public:
    LValueExpr(SourceLoc l, up<Lval> val);
    void sem() override;
    void print(std::ostream& out) const override;
};

class ParenExpr : public Expr {
protected:
    up<Expr> inner;

public:
    ParenExpr(SourceLoc l, up<Expr> expr);
    void sem() override;
    void print(std::ostream& out) const override;
};

class FuncCall : public Expr {
protected:
    string name;
    vec<up<Expr>> args;

public:
    FuncCall(SourceLoc l, string id, vec<up<Expr>> a);
    void sem() override;
    void print(std::ostream& out) const override;
};

class UnaryExpr : public Expr {
protected:
    UnOp op;
    up<Expr> operand;

public:
    UnaryExpr(SourceLoc l, UnOp operation, up<Expr> expr);
    void sem() override;
    void print(std::ostream& out) const override;
};

class BinaryExpr : public Expr {
protected:
    BinOp op;
    up<Expr> lhs;
    up<Expr> rhs;

public:
    BinaryExpr(SourceLoc l, BinOp operation, up<Expr> left, up<Expr> right);
    void sem() override;
    void print(std::ostream& out) const override;
};

// ===== Conditions =====

class Cond : public Expr {
public:
    explicit Cond(SourceLoc l);
    ~Cond() override = default;
    void sem() override;
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
