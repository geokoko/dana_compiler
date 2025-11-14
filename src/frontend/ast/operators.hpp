// operators -- purely syntax-related enums and functions for operator names
#pragma once
enum class UnOp { Plus, Minus, Not };
enum class BinOp { Add, Sub, Mul, Div, Mod, AndBits, OrBits };
enum class RelOp { Eq, Ne, Le, Ge, Lt, Gt };
enum class LogicOp { And, Or };

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

