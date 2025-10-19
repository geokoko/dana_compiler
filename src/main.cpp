#include <iostream>
#include "./frontend/parser/parser.tab.hh"

int main(int argc, char** argv) {
    dana::parser parser;
    int res = parser.parse();

    if (res == 0) {
        std::cout << "Parsing finished: success (parse() returned 0)\n";
    } else {
        std::cerr << "Parsing finished: failure (parse() returned " << res << ")\n";
    }

    return res;
}

