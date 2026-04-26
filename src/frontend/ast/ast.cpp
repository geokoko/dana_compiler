#include "ast.hpp"
#include "ast_visitor.hpp"
// ===== Base nodes =====

ASTNode::ASTNode(SourceLoc loc) : loc(loc) {}
ASTNode::~ASTNode() = default;

Expr::Expr(SourceLoc loc, ExprKind kind) : ASTNode(loc), kind_(kind) {}
Expr::~Expr() = default;
Expr::ExprKind Expr::getKind() const { return kind_; }

Stmt::Stmt(SourceLoc l, StmtKind kind) : ASTNode(l), kind_(kind) {}
Stmt::~Stmt() = default;
Stmt::StmtKind Stmt::getKind() const { return kind_; }

Lval::Lval(SourceLoc l) : ASTNode(l) {}
Lval::~Lval() = default;

Rval::Rval(SourceLoc l, ExprKind kind) : Expr(l, kind) {}

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
void Type::accept(AstVisitor& v) { v.visit(*this); }
DataType Type::data_type() const { return base; }
const vec<std::optional<int>>& Type::dimensions() const { return dims; }
FParType::FParType(SourceLoc l, bool ref, DataType type)
    : Type(l, type), by_ref(ref) {}
FParType::FParType(SourceLoc l, bool ref, DataType type, vec<std::optional<int>> d)
    : Type(l, type, std::move(d)), by_ref(ref) {}
void FParType::accept(AstVisitor& v) { v.visit(*this); }
bool FParType::isByRef() const { return by_ref; }

// ===== Blocks =====

Block::Block(SourceLoc l, vec<up<Stmt>> stmts)
    : ASTNode(l), statements(std::move(stmts)) {}
void Block::accept(AstVisitor& v) { v.visit(*this); }

// ===== Definitions =====

Def::Def(SourceLoc l, DefKind kind) : ASTNode(l), kind_(kind) {}
Def::DefKind Def::getKind() const { return kind_; }

Program::Program(SourceLoc l, up<FuncDef> d)
    : ASTNode(l), top(std::move(d)) {}

void Program::accept(AstVisitor& v) { v.visit(*this); }

FParDef::FParDef(SourceLoc l, vec<string> names, up<FParType> t)
    : Def(l, DefKind::FPar), identifiers(std::move(names)), type(std::move(t)) {}

void FParDef::accept(AstVisitor& v) { v.visit(*this); }
const vec<string>& FParDef::names() const { return identifiers; }
const FParType* FParDef::parameterType() const { return type.get(); }

Header::Header(SourceLoc l, string n, optional<DataType> r, vec<up<FParDef>> p)
    : Def(l, DefKind::Header), name(std::move(n)), return_type(std::move(r)), params(std::move(p)) {}
void Header::accept(AstVisitor& v) { v.visit(*this); }
const string& Header::identifier() const { return name; }
const vec<up<FParDef>>& Header::parameters() const { return params; }
optional<DataType> Header::returnType() const { return return_type; }

VarDef::VarDef(SourceLoc l, vec<string> ids, up<Type> t)
    : Def(l, DefKind::Var), names(std::move(ids)), declared_type(std::move(t)) {}
void VarDef::accept(AstVisitor& v) { v.visit(*this); }

FuncDecl::FuncDecl(SourceLoc l, up<Header> h)
    : Def(l, DefKind::FuncDecl), header(std::move(h)) {}
void FuncDecl::accept(AstVisitor& v) { v.visit(*this); }

FuncDef::FuncDef(SourceLoc l, up<Header> h, vec<up<Def>> defs, up<Block> b)
    : Def(l, DefKind::FuncDef), header(std::move(h)), locals(std::move(defs)), body(std::move(b)) {}
void FuncDef::accept(AstVisitor& v) { v.visit(*this); }
bool FuncDef::isEntrypoint() { return isEntrypoint_; }
void FuncDef::setEntrypoint(bool cond) { isEntrypoint_ = cond; }

// ===== Statements =====

SkipStmt::SkipStmt(SourceLoc l) : Stmt(l, StmtKind::Skip) {}
void SkipStmt::accept(AstVisitor& v) { v.visit(*this); }

ExitStmt::ExitStmt(SourceLoc l) : Stmt(l, StmtKind::Exit) {}
void ExitStmt::accept(AstVisitor& v) { v.visit(*this); }

AssignStmt::AssignStmt(SourceLoc l, up<Lval> left, up<Expr> right)
    : Stmt(l, StmtKind::Assign), lhs(std::move(left)), rhs(std::move(right)) {}
void AssignStmt::accept(AstVisitor& v) { v.visit(*this); }

ReturnStmt::ReturnStmt(SourceLoc l, up<Expr> expr)
    : Stmt(l, StmtKind::Return), value(std::move(expr)) {}
void ReturnStmt::accept(AstVisitor& v) { v.visit(*this); }

// ProcCall
ProcCall::ProcCall(SourceLoc l, string id, vec<up<Expr>> a) :
    Stmt(l, StmtKind::ProcCall), name(std::move(id)), args(std::move(a)) {}

void ProcCall::accept(AstVisitor& v) { v.visit(*this); }

BreakStmt::BreakStmt(SourceLoc l, optional<string> lbl)
    : Stmt(l, StmtKind::Break), label(std::move(lbl)) {}
void BreakStmt::accept(AstVisitor& v) { v.visit(*this); }

ContinueStmt::ContinueStmt(SourceLoc l, optional<string> lbl)
    : Stmt(l, StmtKind::Continue), label(std::move(lbl)) {}
void ContinueStmt::accept(AstVisitor& v) { v.visit(*this); }

IfStmt::IfStmt(SourceLoc l,
               up<Cond> cond,
               up<Block> then_block,
               vec<std::pair<up<Cond>, up<Block>>> elifs,
               std::optional<up<Block>> else_block)
	: Stmt(l, StmtKind::If),
      condition(std::move(cond)),
      then_branch(std::move(then_block)),
      elif_branches(std::move(elifs)),
      else_branch(std::move(else_block)) {}
void IfStmt::accept(AstVisitor& v) { v.visit(*this); }

LoopStmt::LoopStmt(SourceLoc l, std::optional<string> lbl, up<Block> blk)
    : Stmt(l, StmtKind::Loop), label(std::move(lbl)), body(std::move(blk)) {}
void LoopStmt::accept(AstVisitor& v) { v.visit(*this); }

// ===== L-values =====

IdLVal::IdLVal(SourceLoc l, string id)
    : Lval(l), name(std::move(id)) {}
void IdLVal::accept(AstVisitor& v) { v.visit(*this); }

StringLiteralLVal::StringLiteralLVal(SourceLoc l, string v)
    : Lval(l), value(std::move(v)) {}
void StringLiteralLVal::accept(AstVisitor& v) { v.visit(*this); }

IndexLVal::IndexLVal(SourceLoc l, up<Lval> b, up<Expr> idx)
    : Lval(l), base(std::move(b)), index(std::move(idx)) {}
void IndexLVal::accept(AstVisitor& v) { v.visit(*this); }

// ===== R-values / expressions =====

IntConst::IntConst(SourceLoc l, int v)
    : Rval(l, ExprKind::IntConst), value(v) {}
void IntConst::accept(AstVisitor& v) { v.visit(*this); }

CharConst::CharConst(SourceLoc l, unsigned char v)
    : Rval(l, ExprKind::CharConst), value(v) {}
void CharConst::accept(AstVisitor& v) { v.visit(*this); }

TrueConst::TrueConst(SourceLoc l)
    : Rval(l, ExprKind::TrueConst) {}
void TrueConst::accept(AstVisitor& v) { v.visit(*this); }

FalseConst::FalseConst(SourceLoc l)
    : Rval(l, ExprKind::FalseConst) {}
void FalseConst::accept(AstVisitor& v) { v.visit(*this); }

LValueExpr::LValueExpr(SourceLoc l, up<Lval> val)
    : Expr(l, ExprKind::LValueExpr), value(std::move(val)) {}
void LValueExpr::accept(AstVisitor& v) { v.visit(*this); }

ParenExpr::ParenExpr(SourceLoc l, up<Expr> expr)
    : Expr(l, ExprKind::ParenExpr), inner(std::move(expr)) {}
void ParenExpr::accept(AstVisitor& v) { v.visit(*this); }

FuncCall::FuncCall(SourceLoc l, string id, vec<up<Expr>> a)
    : Expr(l, ExprKind::FuncCall), name(std::move(id)), args(std::move(a)) {}
void FuncCall::accept(AstVisitor& v) { v.visit(*this); }

UnaryExpr::UnaryExpr(SourceLoc l, UnOp operation, up<Expr> expr)
    : Expr(l, ExprKind::UnaryExpr), op(operation), operand(std::move(expr)) {}
void UnaryExpr::accept(AstVisitor& v) { v.visit(*this); }

BinaryExpr::BinaryExpr(SourceLoc l, BinOp operation, up<Expr> left, up<Expr> right)
    : Expr(l, ExprKind::BinaryExpr), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}
void BinaryExpr::accept(AstVisitor& v) { v.visit(*this); }

// ===== Conditions =====

Cond::Cond(SourceLoc l, ExprKind kind)
    : Expr(l, kind) {}

ExprCond::ExprCond(SourceLoc l, up<Expr> e)
    : Cond(l, ExprKind::ExprCond), expr(std::move(e)) {}
void ExprCond::accept(AstVisitor& v) { v.visit(*this); }

ParenCond::ParenCond(SourceLoc l, up<Cond> c)
    : Cond(l, ExprKind::ParenCond), condition(std::move(c)) {}
void ParenCond::accept(AstVisitor& v) { v.visit(*this); }

NotCond::NotCond(SourceLoc l, up<Cond> c)
    : Cond(l, ExprKind::NotCond), condition(std::move(c)) {}
void NotCond::accept(AstVisitor& v) { v.visit(*this); }

BinaryCond::BinaryCond(SourceLoc l, LogicOp operation, up<Cond> left, up<Cond> right)
    : Cond(l, ExprKind::BinaryCond), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}
void BinaryCond::accept(AstVisitor& v) { v.visit(*this); }

RelCond::RelCond(SourceLoc l, RelOp operation, up<Expr> left, up<Expr> right)
    : Cond(l, ExprKind::RelCond), op(operation), lhs(std::move(left)), rhs(std::move(right)) {}
void RelCond::accept(AstVisitor& v) { v.visit(*this); }
