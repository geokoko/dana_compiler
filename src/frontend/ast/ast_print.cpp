#include "ast.hpp"
#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>

namespace tree {
	// Stack of whether each ancestor is the last child
	thread_local std::vector<bool> last;

	inline void prefix(std::ostream& out) {
		for (std::size_t i = 0; i + 1 < last.size(); ++i) {
			out << (last[i] ? "    " : "│   ");
		}
	}

	// Print a single tree line
	inline void line(std::ostream& out, const std::string& text) {
		if (last.empty()) {
			out << text << '\n';
		} else {
			prefix(out);
			out << (last.back() ? "└── " : "├── ") << text << '\n';
		}
	}

	// Escapes a string for safe single-line display
	inline std::string escape(const std::string& s) {
		std::string r;
		r.reserve(s.size());
		for (unsigned char c : s) {
			switch (c) {
				case '\\': r += "\\\\"; break;
				case '"':  r += "\\\""; break;
				case '\n': r += "\\n"; break;
				case '\r': r += "\\r"; break;
				case '\t': r += "\\t"; break;
				default:
					if (c < 0x20) {
						char buf[5];
						std::snprintf(buf, sizeof(buf), "\\x%02X", static_cast<unsigned>(c));
						r += buf;
					} else {
						r.push_back(static_cast<char>(c));
					}
			}
		}
		return r;
	}

	inline std::string quote(const std::string& s) {
		return std::string("\"") + escape(s) + "\"";
	}

	// Child helpers
	template <class T>
	inline void child(std::ostream& out, const up<T>& ptr, bool is_last) {
		last.push_back(is_last);
		if (ptr) ptr->print(out); else line(out, "null");
		last.pop_back();
	}

	template <class T>
	inline void children(std::ostream& out, const vec<up<T>>& xs) {
		for (std::size_t i = 0; i < xs.size(); ++i) {
			child(out, xs[i], i + 1 == xs.size());
		}
	}

	// Small format helpers
	inline std::string tag(const char* name, const SourceLoc& loc) {
		std::ostringstream oss;
		oss << name << '[' << loc.line << ':' << loc.col << ']';
		return oss.str();
	}

	inline std::string join(const vec<string>& xs, const char* sep = ", ") {
		std::string r;
		for (std::size_t i = 0; i < xs.size(); ++i) { if (i) r += sep; r += xs[i]; }
		return r;
	}

	inline std::string dims_str(const vec<std::optional<int>>& dims) {
		std::ostringstream oss;
		oss << '[';
		for (std::size_t i = 0; i < dims.size(); ++i) {
			if (i) oss << ", ";
			if (dims[i]) oss << *dims[i]; else oss << '?';
		}
		oss << ']';
		return oss.str();
	}
} // namespace tree

// ---- Printing implementations ----

void Type::print(std::ostream& out) const {
	tree::line(out, tree::tag("Type", loc) + " base=" + dataTypeName(base) + " dims=" + tree::dims_str(dims));
}

void Type::printTypeDetails(std::ostream& out) const {
	out << "base=" << dataTypeName(base) << ", dims=[";
	for (std::size_t i = 0; i < dims.size(); ++i) {
		if (i) out << ", ";
		if (dims[i]) {
			out << *dims[i];
		} else {
			out << '?';
		}
	}
	out << ']';
}

void Block::print(std::ostream& out) const {
	tree::line(out, tree::tag("Block", loc));
	tree::children(out, statements);
}

void Def::print(std::ostream& out) const {
	(void)out; // Base never printed directly; derived classes override
}

void Program::print(std::ostream& out) const {
	tree::line(out, tree::tag("Program", loc));
	if (top) tree::child(out, top, true);
}

void FParType::print(std::ostream& out) const {
	tree::line(out, tree::tag("FParType", loc) +
			std::string(" by_ref=") + (by_ref ? "true" : "false") +
			" base=" + dataTypeName(base) +
			" dims=" + tree::dims_str(dims));
}

void FParDef::print(std::ostream& out) const {
	tree::line(out, tree::tag("FParDef", loc) + " ids=[" + tree::join(identifiers) + "]");
	if (type) tree::child(out, type, true);
}

void Header::print(std::ostream& out) const {
	tree::line(out, tree::tag("Header", loc) + " name=" + name +
			" return=" + (return_type ? dataTypeName(*return_type) : std::string("null")));
	tree::children(out, params);
}

void VarDef::print(std::ostream& out) const {
	tree::line(out, tree::tag("VarDef", loc) + " names=[" + tree::join(names) + "]");
	if (declared_type) tree::child(out, declared_type, true);
}

void FuncDecl::print(std::ostream& out) const {
	tree::line(out, tree::tag("FuncDecl", loc));
	if (header) tree::child(out, header, true);
}

void FuncDef::print(std::ostream& out) const {
	tree::line(out, tree::tag("FuncDef", loc));
	int n = (header ? 1 : 0) + static_cast<int>(locals.size()) + (body ? 1 : 0);
	int k = 0;
	if (header) tree::child(out, header, ++k == n);
	for (std::size_t i = 0; i < locals.size(); ++i) tree::child(out, locals[i], ++k == n);
	if (body) tree::child(out, body, ++k == n);
}

void SkipStmt::print(std::ostream& out) const { tree::line(out, tree::tag("SkipStmt", loc)); }

void ExitStmt::print(std::ostream& out) const { tree::line(out, tree::tag("ExitStmt", loc)); }

void AssignStmt::print(std::ostream& out) const {
	tree::line(out, tree::tag("AssignStmt", loc));
	int n = (lhs ? 1 : 0) + (rhs ? 1 : 0), k = 0;
	if (lhs) tree::child(out, lhs, ++k == n);
	if (rhs) tree::child(out, rhs, ++k == n);
}

void ReturnStmt::print(std::ostream& out) const {
	tree::line(out, tree::tag("ReturnStmt", loc));
	if (value) tree::child(out, value, true);
}

void ProcCall::print(std::ostream& out) const {
	tree::line(out, tree::tag("ProcCall", loc) + " name=" + name);
	tree::children(out, args);
}

void BreakStmt::print(std::ostream& out) const {
	tree::line(out, tree::tag("BreakStmt", loc) + " label=" + (label ? *label : std::string("null")));
}

void ContinueStmt::print(std::ostream& out) const {
	tree::line(out, tree::tag("ContinueStmt", loc) + " label=" + (label ? *label : std::string("null")));
}

void IfStmt::print(std::ostream& out) const {
	tree::line(out, tree::tag("IfStmt", loc));
	int n = (condition ? 1 : 0) + (then_branch ? 1 : 0) + static_cast<int>(elif_branches.size()) + ((else_branch && *else_branch) ? 1 : 0);
	int k = 0;
	if (condition) tree::child(out, condition, ++k == n);
	if (then_branch) tree::child(out, then_branch, ++k == n);
	for (std::size_t i = 0; i < elif_branches.size(); ++i) {
		bool last_elif = (++k == n);
		tree::last.push_back(last_elif);
		tree::line(out, std::string("Elif[") + std::to_string(i) + "]");
		// Two children: condition, block
		tree::last.push_back(false);
		if (elif_branches[i].first) elif_branches[i].first->print(out); else tree::line(out, "null");
		tree::last.pop_back();
		tree::last.push_back(true);
		if (elif_branches[i].second) elif_branches[i].second->print(out); else tree::line(out, "null");
		tree::last.pop_back();
		tree::last.pop_back();
	}
	if (else_branch && *else_branch) {
		bool last_else = (++k == n);
		tree::last.push_back(last_else);
		tree::line(out, "Else");
		tree::last.push_back(true);
		(**else_branch).print(out);
		tree::last.pop_back();
		tree::last.pop_back();
	}
}

void LoopStmt::print(std::ostream& out) const {
	tree::line(out, tree::tag("LoopStmt", loc) + " label=" + (label ? *label : std::string("null")));
	if (body) tree::child(out, body, true);
}

void IdLVal::print(std::ostream& out) const {
	tree::line(out, tree::tag("IdLVal", loc) + " name=" + name);
}

void StringLiteralLVal::print(std::ostream& out) const {
	tree::line(out, tree::tag("StringLiteralLVal", loc) + " value=" + tree::quote(value));
}

void IndexLVal::print(std::ostream& out) const {
	tree::line(out, tree::tag("IndexLVal", loc));
	int n = (base ? 1 : 0) + (index ? 1 : 0), k = 0;
	if (base) tree::child(out, base, ++k == n);
	if (index) tree::child(out, index, ++k == n);
}

void IntConst::print(std::ostream& out) const {
	tree::line(out, tree::tag("IntConst", loc) + " value=" + std::to_string(value));
}

void CharConst::print(std::ostream& out) const {
	tree::line(out, tree::tag("CharConst", loc) + " value=" + std::to_string(static_cast<int>(value)));
}

void TrueConst::print(std::ostream& out) const { tree::line(out, tree::tag("TrueConst", loc)); }

void FalseConst::print(std::ostream& out) const { tree::line(out, tree::tag("FalseConst", loc)); }

void LValueExpr::print(std::ostream& out) const {
	tree::line(out, tree::tag("LValueExpr", loc));
	if (value) tree::child(out, value, true);
}

void ParenExpr::print(std::ostream& out) const {
	tree::line(out, tree::tag("ParenExpr", loc));
	if (inner) tree::child(out, inner, true);
}

void FuncCall::print(std::ostream& out) const {
	tree::line(out, tree::tag("FuncCall", loc) + " name=" + name);
	tree::children(out, args);
}

void UnaryExpr::print(std::ostream& out) const {
	tree::line(out, tree::tag("UnaryExpr", loc) + " op=" + unOpName(op));
	if (operand) tree::child(out, operand, true);
}

void BinaryExpr::print(std::ostream& out) const {
	tree::line(out, tree::tag("BinaryExpr", loc) + " op=" + binOpName(op));
	int n = (lhs ? 1 : 0) + (rhs ? 1 : 0), k = 0;
	if (lhs) tree::child(out, lhs, ++k == n);
	if (rhs) tree::child(out, rhs, ++k == n);
}

void ExprCond::print(std::ostream& out) const {
	tree::line(out, tree::tag("ExprCond", loc));
	if (expr) tree::child(out, expr, true);
}

void ParenCond::print(std::ostream& out) const {
	tree::line(out, tree::tag("ParenCond", loc));
	if (condition) tree::child(out, condition, true);
}

void NotCond::print(std::ostream& out) const {
	tree::line(out, tree::tag("NotCond", loc));
	if (condition) tree::child(out, condition, true);
}

void BinaryCond::print(std::ostream& out) const {
	tree::line(out, tree::tag("BinaryCond", loc) + " op=" + logicOpName(op));
	int n = (lhs ? 1 : 0) + (rhs ? 1 : 0), k = 0;
	if (lhs) tree::child(out, lhs, ++k == n);
	if (rhs) tree::child(out, rhs, ++k == n);
}

void RelCond::print(std::ostream& out) const {
	tree::line(out, tree::tag("RelCond", loc) + " op=" + relOpName(op));
	int n = (lhs ? 1 : 0) + (rhs ? 1 : 0), k = 0;
	if (lhs) tree::child(out, lhs, ++k == n);
	if (rhs) tree::child(out, rhs, ++k == n);
}
