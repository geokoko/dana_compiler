#include <iostream>
#include <cstdio>
#include <string>
#include "./frontend/parser/parser.tab.hh"
// Bring in AST types and printing support
#include "./frontend/ast/ast.hpp"

extern FILE* yyin;

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [--ast-tree] <file.dana>\n";
}

int main(int argc, char** argv) {
    bool want_tree = false;
    const char* fname = nullptr;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--ast-tree") {
            want_tree = true;
        } else if (!fname) {
            fname = argv[i];
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!fname) {
        print_usage(argv[0]);
        return 1;
    }

    yyin = std::fopen(fname, "r");
    if (!yyin) {
        std::cerr << "Error: could not open file " << fname << "\n";
        return 1;
    }

    // to show location of errors
    dana::parser::location_type loc;

    std::string filename = fname;
    loc.initialize();
    loc.begin.filename = &filename;
    loc.end.filename = &filename;

    // Parse and collect AST root via parser's parse-param
    up<Program> ast_root;
    dana::parser parser(ast_root);
    int res = parser.parse();

    std::fclose(yyin);

    if (res == 0) {
        std::cout << "Parsing finished: success (parse() returned 0)\n";
        if (want_tree) {
            if (ast_root) {
                // Pretty tree output is implemented by print()
                ast_root->print(std::cout);
            } else {
                std::cout << "AST: null (no AST produced)\n";
            }
        }
    } else {
        std::cerr << "Parsing finished: failure (parse() returned " << res << ")\n";
    }

    return res;
}
