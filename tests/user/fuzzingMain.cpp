#include <iostream>
#include <lib/parser_common.h>
#include <lib/parser.h>
#include "util/common.h"

#ifndef __AFL_LOOP
#define __AFL_LOOP 0
#endif

void parse(std::istream &istream) {
    parser_context_t ctx;
    parser_error_t err;

    std::string input;
    istream >> input;

    err = parser_parse(&ctx, (const uint8_t *) input.c_str(), input.length());
    if (err != parser_ok)
        return;

    auto output = dumpUI(&ctx, 40, 40);

    for (auto line : output) {
        std::cout << line << std::endl;
    }
}

int main(int argc, char** argv) {
    while (__AFL_LOOP(1000)) {
        parse(std::cin);
    }
    return 0;
}
