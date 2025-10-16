#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <string>

using std::unique_ptr;
using std::make_unique;
template <class T> using up = std::unique_ptr<T>;

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

// Expression nodes
class Expr : public ASTNode {
public:
	Expr (SourceLoc loc) : ASTNode (loc) {}
	virtual ~Expr() = default;
	virtual void sem() override = 0;
	virtual void print(std::ostream &out) const = 0;
};
