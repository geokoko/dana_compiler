#include <cstdio>
#include <iostream>
#include <string>

#include "./frontend/ast/ast.hpp"
#include "./frontend/parser/parser.tab.hh"
#include "./frontend/semantic/diagnostics.hpp"
#include "./frontend/semantic/sema_context.hpp"
#include "./frontend/symbol/symbol_table.hpp"

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

    if (res != 0) {
        std::cerr << "Parsing finished: failure (parse() returned " << res << ")\n";
        return res;
    }

    if (!ast_root) {
        std::cerr << "Parsing produced no AST\n";
        return 1;
    }

    if (want_tree) {
        ast_root->print(std::cout);
    }

    SymbolTable symtab;
    Diagnostics diags;
    SemContext semCtx(symtab, diags);

    ast_root->sem(semCtx);

    if (semCtx.hasErrors()) {
        semCtx.printDiagnostics();
        return 1;
    }

    return 0;
}
