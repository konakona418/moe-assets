//
// Created on 2025/4/2.
//

#include "args.h"

namespace moe_assets {
    ArgParser::ArgParser(const int argc, char** argv) : argc(argc), argv(argv) {
    }

    std::optional<ParseResult> ArgParser::parse() const {
        std::string input_path;
        std::string output_path;

        if (argc < 2) {
            return std::nullopt;
        }

        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "-i") == 0) {
                if (i + 1 < argc) {
                    input_path = argv[i + 1];
                } else {
                    return std::nullopt;
                }
            } else if (strcmp(argv[i], "-o") == 0) {
                if (i + 1 < argc) {
                    output_path = argv[i + 1];
                }
            }
        }
        return ParseResult(input_path, output_path);
    }
}
