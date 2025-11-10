#include "ast.hpp"

ASTNode::ASTNode(SourceLoc loc) : loc(loc) {}

Expr::Expr(SourceLoc loc) : ASTNode(loc) {}

Stmt::Stmt(SourceLoc l) : ASTNode(l) {}

Lval::Lval(SourceLoc l) : ASTNode(l) {}

Rval::Rval(SourceLoc l) : Expr(l) {}

Type::Type(SourceLoc l, DataType b, vec<std::optional<int>> d)
    : ASTNode(l), base(b), dims(std::move(d)) {}

DataType Type::data_type() const { 
	return base; 
}

const vec<std::optional<int>>& Type::dimensions() const {
    return dims;
}

Block::Block(SourceLoc l, vec<up<Stmt>> stmts)
    : ASTNode(l), statements(std::move(stmts)) {}

Def::Def(SourceLoc l) : ASTNode(l) {}

Program::Program(SourceLoc l, up<Def> d)
    : ASTNode(l), top(std::move(d)) {}

FParType::FParType(SourceLoc l, bool ref, DataType type)
    : Type(l, type), by_ref(ref) {}

FParType::FParType(SourceLoc l, bool ref, DataType type, vec<std::optional<int>> d)
    : Type(l, type, std::move(d)), by_ref(ref) {}

FParDef::FParDef(SourceLoc l, vec<string> names, up<FParType> t)
    : Def(l), identifiers(std::move(names)), type(std::move(t)) {}

Header::Header(SourceLoc l, string n, optional<DataType> r, vec<up<FParDef>> p)
    : Def(l), name(std::move(n)), return_type(std::move(r)), params(std::move(p)) {}

VarDef::VarDef(SourceLoc l, vec<string> ids, up<Type> t)
    : Def(l), names(std::move(ids)), declared_type(std::move(t)) {}

FuncDecl::FuncDecl(SourceLoc l, up<Header> h)
    : Def(l), header(std::move(h)) {}

FuncDef::FuncDef(SourceLoc l, up<Header> h, vec<up<Def>> defs, up<Block> b)
    : Def(l), header(std::move(h)), locals(std::move(defs)), body(std::move(b)) {}

SkipStmt::SkipStmt(SourceLoc l) : Stmt(l) {}

ExitStmt::ExitStmt(SourceLoc l) : Stmt(l) {}

AssignStmt::AssignStmt(SourceLoc l, up<Lval> left, up<Expr> right)
    : Stmt(l), lhs(std::move(left)), rhs(std::move(right)) {}

ReturnStmt::ReturnStmt(SourceLoc l, up<Expr> expr)
    : Stmt(l), value(std::move(expr)) {}

ProcCall::ProcCall(SourceLoc l, string id, vec<up<Expr>> a)
    : Stmt(l), name(std::move(id)), args(std::move(a)) {}

BreakStmt::BreakStmt(SourceLoc l, optional<string> lbl)
    : Stmt(l), label(std::move(lbl)) {}

ContinueStmt::ContinueStmt(SourceLoc l, optional<string> lbl)
    : Stmt(l), label(std::move(lbl)) {}

IfStmt::IfStmt(SourceLoc l,
               up<Cond> cond,
               up<Block> then_block,
               vec<std::pair<up<Cond>, up<Block>>> elifs,
               std::optional<up<Block>> else_block)
    : Stmt(l),
      condition(std::move(cond)),
      then_branch(std::move(then_block)),
      elif_branches(std::move(elifs)),
      else_branch(std::move(else_block)) {}

LoopStmt::LoopStmt(SourceLoc l, std::optional<string> lbl, up<Block> blk)
    : Stmt(l), label(std::move(lbl)), body(std::move(blk)) {}

IdLVal::IdLVal(SourceLoc l, string id)
    : Lval(l), name(std::move(id)) {}

StringLiteralLVal::StringLiteralLVal(SourceLoc l, string v)
    : Lval(l), value(std::move(v)) {}

IndexLVal::IndexLVal(SourceLoc l, up<Lval> b, up<Expr> idx)
    : Lval(l), base(std::move(b)), index(std::move(idx)) {}

IntConst::IntConst(SourceLoc l, int v)
    : Rval(l), value(v) {}

CharConst::CharConst(SourceLoc l, unsigned char v)
    : Rval(l), value(v) {}

TrueConst::TrueConst(SourceLoc l)
    : Rval(l) {}

FalseConst::FalseConst(SourceLoc l)
    : Rval(l) {}

LValueExpr::LValueExpr(SourceLoc l, up<Lval> val)
    : Expr(l), value(std::move(val)) {}

ParenExpr::ParenExpr(SourceLoc l, up<Expr> expr)
    : Expr(l), inner(std::move(expr)) {}

FuncCall::FuncCall(SourceLoc l, string id, vec<up<Expr>> a)
    : Expr(l), name(std::move(id)), args(std::move(a)) {}

UnaryExpr::UnaryExpr(SourceLoc l, UnOp operation, up<Expr> expr)
    : Expr(l), op(operation), operand(std::move(expr)) {}

BinaryExpr::BinaryExpr(SourceLoc l, BinOp operation, up<Expr> left, up<Expr> right)
    : Expr(l), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}

Cond::Cond(SourceLoc l)
    : Expr(l) {}

ExprCond::ExprCond(SourceLoc l, up<Expr> e)
    : Cond(l), expr(std::move(e)) {}

ParenCond::ParenCond(SourceLoc l, up<Cond> c)
    : Cond(l), condition(std::move(c)) {}

NotCond::NotCond(SourceLoc l, up<Cond> c)
    : Cond(l), condition(std::move(c)) {}

BinaryCond::BinaryCond(SourceLoc l, LogicOp operation, up<Cond> left, up<Cond> right)
    : Cond(l), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}

RelCond::RelCond(SourceLoc l, RelOp operation, up<Expr> left, up<Expr> right)
    : Cond(l), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}

// ---- Default no-op semantic passes ----
void ASTNode::sem(SemContext& ctx) {}
void Stmt::sem(SemContext& ctx) {}
void Lval::sem(SemContext& ctx) {}
void Type::sem(SemContext& ctx) {}
void Block::sem(SemContext& ctx) {}
void Def::sem(SemContext& ctx) {}
void FParType::sem(SemContext& ctx) {}
void FParDef::sem(SemContext& ctx) {}
void Header::sem(SemContext& ctx) {}
void VarDef::sem(SemContext& ctx) {}
void FuncDecl::sem(SemContext& ctx) {}
void FuncDef::sem(SemContext& ctx) {}
void LValueExpr::sem(SemContext& ctx) {}
void ParenExpr::sem(SemContext& ctx) {}
void FuncCall::sem(SemContext& ctx) {}
void UnaryExpr::sem(SemContext& ctx) {}
void BinaryExpr::sem(SemContext& ctx) {}
void IntConst::sem(SemContext& ctx) {}
void CharConst::sem(SemContext& ctx) {}
void TrueConst::sem(SemContext& ctx) {}
void FalseConst::sem(SemContext& ctx) {}
void Cond::sem(SemContext& ctx) {}
void Rval::sem(SemContext& ctx) {}
