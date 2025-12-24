#pragma once

class Type;
class FParType;
class Program;
class FParDef;
class Header;
class VarDef;
class FuncDecl;
class FuncDef;
class Block;
class SkipStmt;
class ExitStmt;
class AssignStmt;
class ReturnStmt;
class ProcCall;
class BreakStmt;
class ContinueStmt;
class IfStmt;
class LoopStmt;
class IdLVal;
class StringLiteralLVal;
class IndexLVal;
class IntConst;
class CharConst;
class TrueConst;
class FalseConst;
class LValueExpr;
class ParenExpr;
class FuncCall;
class UnaryExpr;
class BinaryExpr;
class ExprCond;
class ParenCond;
class NotCond;
class BinaryCond;
class RelCond;

class AstVisitor {
public:
	virtual ~AstVisitor() = default;

	virtual void visit(Type&) = 0;
	virtual void visit(FParType&) = 0;
	virtual void visit(Program&) = 0;
	virtual void visit(FParDef&) = 0;
	virtual void visit(Header&) = 0;
	virtual void visit(VarDef&) = 0;
	virtual void visit(FuncDecl&) = 0;
	virtual void visit(FuncDef&) = 0;
	virtual void visit(Block&) = 0;

	virtual void visit(SkipStmt&) = 0;
	virtual void visit(ExitStmt&) = 0;
	virtual void visit(AssignStmt&) = 0;
	virtual void visit(ReturnStmt&) = 0;
	virtual void visit(ProcCall&) = 0;
	virtual void visit(BreakStmt&) = 0;
	virtual void visit(ContinueStmt&) = 0;
	virtual void visit(IfStmt&) = 0;
	virtual void visit(LoopStmt&) = 0;

	virtual void visit(IdLVal&) = 0;
	virtual void visit(StringLiteralLVal&) = 0;
	virtual void visit(IndexLVal&) = 0;
	virtual void visit(IntConst&) = 0;
	virtual void visit(CharConst&) = 0;
	virtual void visit(TrueConst&) = 0;
	virtual void visit(FalseConst&) = 0;
	virtual void visit(LValueExpr&) = 0;
	virtual void visit(ParenExpr&) = 0;
	virtual void visit(FuncCall&) = 0;
	virtual void visit(UnaryExpr&) = 0;
	virtual void visit(BinaryExpr&) = 0;
	virtual void visit(ExprCond&) = 0;
	virtual void visit(ParenCond&) = 0;
	virtual void visit(NotCond&) = 0;
	virtual void visit(BinaryCond&) = 0;
	virtual void visit(RelCond&) = 0;
};
