//
// Created on 2025/4/2.
//

#ifndef ARGS_H
#define ARGS_H

#include <optional>
#include <cstring>
#include <string>

namespace moe_assets {
    struct ParseResult {
        std::string input_path;
        std::string output_path;

        ParseResult() = default;

        ParseResult(std::string input_path, std::string output_path)
            : input_path(std::move(input_path)), output_path(std::move(output_path)) {
        }
    };

    class ArgParser {
        int argc;
        char** argv;

    public:
        ArgParser(int argc, char** argv);

        [[nodiscard]] std::optional<ParseResult> parse() const;
    };
}


#endif //ARGS_H
