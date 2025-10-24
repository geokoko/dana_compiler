#include <iostream>
#include <cstdio>
#include <string>
#include "./frontend/parser/parser.tab.hh"

extern FILE* yyin;

int main(int argc, char** argv) {
	const char* fname = argc > 1 ? argv[1] : "input.dana";
	if (argc == 1) {
		std::cout << "No input file specified. Using default: " << fname << "\n";
		return 0;
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

    dana::parser parser;
    int res = parser.parse();

	std::fclose(yyin);

    if (res == 0) {
        std::cout << "Parsing finished: success (parse() returned 0)\n";
    } else {
        std::cerr << "Parsing finished: failure (parse() returned " << res << ")\n";
    }

    return res;
}

