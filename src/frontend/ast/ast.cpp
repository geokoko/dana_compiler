#include "ast.hpp"

// ===== Base nodes =====

ASTNode::ASTNode(SourceLoc loc) : loc(loc) {}
ASTNode::~ASTNode() = default;

Expr::Expr(SourceLoc loc) : ASTNode(loc) {}
Expr::~Expr() = default;

Stmt::Stmt(SourceLoc l) : ASTNode(l) {}
Stmt::~Stmt() = default;

Lval::Lval(SourceLoc l) : ASTNode(l) {}
Lval::~Lval() = default;

Rval::Rval(SourceLoc l) : Expr(l) {}

// ===== Helpers on Expr/Lval =====

SemaTypePtr Expr::type() const { return resolvedType_; }
void Expr::setType(SemaTypePtr type) { resolvedType_ = std::move(type); }
bool Expr::isLValue() const { return isLValue_; }
void Expr::setLValue(bool v) { isLValue_ = v; }
bool Expr::isAssignable() const { return assignable_; }
void Expr::setAssignable(bool v) { assignable_ = v; }
bool Expr::isConstExpr() const { return constExpr_; }
void Expr::setConstExpr(bool v) { constExpr_ = v; }

SemaTypePtr Lval::type() const { return resolvedType_; }
void Lval::setType(SemaTypePtr type) { resolvedType_ = std::move(type); }
bool Lval::isAssignable() const { return assignable_; }
void Lval::setAssignable(bool v) { assignable_ = v; }

// ===== Types =====

Type::Type(SourceLoc l, DataType b, vec<std::optional<int>> d)
    : ASTNode(l), base(b), dims(std::move(d)) {}
void Type::agen(Codegen& v) { v.gen(*this); }
DataType Type::data_type() const { return base; }
const vec<std::optional<int>>& Type::dimensions() const { return dims; }
void Type::sem(SemContext&) {}

FParType::FParType(SourceLoc l, bool ref, DataType type)
    : Type(l, type), by_ref(ref) {}
FParType::FParType(SourceLoc l, bool ref, DataType type, vec<std::optional<int>> d)
    : Type(l, type, std::move(d)), by_ref(ref) {}
void FParType::agen(Codegen& v) { v.gen(*this); }
bool FParType::isByRef() const { return by_ref; }

// ===== Blocks =====

Block::Block(SourceLoc l, vec<up<Stmt>> stmts)
    : ASTNode(l), statements(std::move(stmts)) {}
void Block::agen(Codegen& v) { v.gen(*this); }

// ===== Definitions =====

Def::Def(SourceLoc l) : ASTNode(l) {}

Program::Program(SourceLoc l, up<FuncDef> d)
    : ASTNode(l), top(std::move(d)) {}
void Program::agen(Codegen& v) { v.gen(*this); }

FParDef::FParDef(SourceLoc l, vec<string> names, up<FParType> t)
    : Def(l), identifiers(std::move(names)), type(std::move(t)) {}
void FParDef::agen(Codegen& v) { v.gen(*this); }
const vec<string>& FParDef::names() const { return identifiers; }
const FParType* FParDef::parameterType() const { return type.get(); }

Header::Header(SourceLoc l, string n, optional<DataType> r, vec<up<FParDef>> p)
    : Def(l), name(std::move(n)), return_type(std::move(r)), params(std::move(p)) {}
void Header::agen(Codegen& v) { v.gen(*this); }
const string& Header::identifier() const { return name; }
const vec<up<FParDef>>& Header::parameters() const { return params; }
optional<DataType> Header::returnType() const { return return_type; }

VarDef::VarDef(SourceLoc l, vec<string> ids, up<Type> t)
    : Def(l), names(std::move(ids)), declared_type(std::move(t)) {}
void VarDef::agen(Codegen& v) { v.gen(*this); }

FuncDecl::FuncDecl(SourceLoc l, up<Header> h)
    : Def(l), header(std::move(h)) {}
void FuncDecl::agen(Codegen& v) { v.gen(*this); }

FuncDef::FuncDef(SourceLoc l, up<Header> h, vec<up<Def>> defs, up<Block> b)
    : Def(l), header(std::move(h)), locals(std::move(defs)), body(std::move(b)) {}
void FuncDef::agen(Codegen& v) { v.gen(*this); }

// ===== Statements =====

SkipStmt::SkipStmt(SourceLoc l) : Stmt(l) {}
void SkipStmt::agen(Codegen& v) { v.gen(*this); }

ExitStmt::ExitStmt(SourceLoc l) : Stmt(l) {}
void ExitStmt::agen(Codegen& v) { v.gen(*this); }

AssignStmt::AssignStmt(SourceLoc l, up<Lval> left, up<Expr> right)
    : Stmt(l), lhs(std::move(left)), rhs(std::move(right)) {}
void AssignStmt::agen(Codegen& v) { v.gen(*this); }

ReturnStmt::ReturnStmt(SourceLoc l, up<Expr> expr)
    : Stmt(l), value(std::move(expr)) {}
void ReturnStmt::agen(Codegen& v) { v.gen(*this); }

ProcCall::ProcCall(SourceLoc l, string id, vec<up<Expr>> a)
    : Stmt(l), name(std::move(id)), args(std::move(a)) {}
void ProcCall::agen(Codegen& v) { v.gen(*this); }

BreakStmt::BreakStmt(SourceLoc l, optional<string> lbl)
    : Stmt(l), label(std::move(lbl)) {}
void BreakStmt::agen(Codegen& v) { v.gen(*this); }

ContinueStmt::ContinueStmt(SourceLoc l, optional<string> lbl)
    : Stmt(l), label(std::move(lbl)) {}
void ContinueStmt::agen(Codegen& v) { v.gen(*this); }

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
void IfStmt::agen(Codegen& v) { v.gen(*this); }

LoopStmt::LoopStmt(SourceLoc l, std::optional<string> lbl, up<Block> blk)
    : Stmt(l), label(std::move(lbl)), body(std::move(blk)) {}
void LoopStmt::agen(Codegen& v) { v.gen(*this); }

// ===== L-values =====

IdLVal::IdLVal(SourceLoc l, string id)
    : Lval(l), name(std::move(id)) {}
void IdLVal::agen(Codegen& v) { v.gen(*this); }

StringLiteralLVal::StringLiteralLVal(SourceLoc l, string v)
    : Lval(l), value(std::move(v)) {}
void StringLiteralLVal::agen(Codegen& v) { v.gen(*this); }

IndexLVal::IndexLVal(SourceLoc l, up<Lval> b, up<Expr> idx)
    : Lval(l), base(std::move(b)), index(std::move(idx)) {}
void IndexLVal::agen(Codegen& v) { v.gen(*this); }

// ===== R-values / expressions =====

IntConst::IntConst(SourceLoc l, int v)
    : Rval(l), value(v) {}
void IntConst::agen(Codegen& v) { v.gen(*this); }

CharConst::CharConst(SourceLoc l, unsigned char v)
    : Rval(l), value(v) {}
void CharConst::agen(Codegen& v) { v.gen(*this); }

TrueConst::TrueConst(SourceLoc l)
    : Rval(l) {}
void TrueConst::agen(Codegen& v) { v.gen(*this); }

FalseConst::FalseConst(SourceLoc l)
    : Rval(l) {}
void FalseConst::agen(Codegen& v) { v.gen(*this); }

LValueExpr::LValueExpr(SourceLoc l, up<Lval> val)
    : Expr(l), value(std::move(val)) {}
void LValueExpr::agen(Codegen& v) { v.gen(*this); }

ParenExpr::ParenExpr(SourceLoc l, up<Expr> expr)
    : Expr(l), inner(std::move(expr)) {}
void ParenExpr::agen(Codegen& v) { v.gen(*this); }

FuncCall::FuncCall(SourceLoc l, string id, vec<up<Expr>> a)
    : Expr(l), name(std::move(id)), args(std::move(a)) {}
void FuncCall::agen(Codegen& v) { v.gen(*this); }

UnaryExpr::UnaryExpr(SourceLoc l, UnOp operation, up<Expr> expr)
    : Expr(l), op(operation), operand(std::move(expr)) {}
void UnaryExpr::agen(Codegen& v) { v.gen(*this); }

BinaryExpr::BinaryExpr(SourceLoc l, BinOp operation, up<Expr> left, up<Expr> right)
    : Expr(l), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}
void BinaryExpr::agen(Codegen& v) { v.gen(*this); }

// ===== Conditions =====

Cond::Cond(SourceLoc l)
    : Expr(l) {}

ExprCond::ExprCond(SourceLoc l, up<Expr> e)
    : Cond(l), expr(std::move(e)) {}
void ExprCond::agen(Codegen& v) { v.gen(*this); }

ParenCond::ParenCond(SourceLoc l, up<Cond> c)
    : Cond(l), condition(std::move(c)) {}
void ParenCond::agen(Codegen& v) { v.gen(*this); }

NotCond::NotCond(SourceLoc l, up<Cond> c)
    : Cond(l), condition(std::move(c)) {}
void NotCond::agen(Codegen& v) { v.gen(*this); }

BinaryCond::BinaryCond(SourceLoc l, LogicOp operation, up<Cond> left, up<Cond> right)
    : Cond(l), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}
void BinaryCond::agen(Codegen& v) { v.gen(*this); }

RelCond::RelCond(SourceLoc l, RelOp operation, up<Expr> left, up<Expr> right)
    : Cond(l), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}
void RelCond::agen(Codegen& v) { v.gen(*this); }
