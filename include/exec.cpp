//
// Created on 2025/4/2.
//

#include "exec.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace moe_assets {
    TaskExecuter::TaskExecuter(const std::string& executable_name) {
        this->executable_name = executable_name;
    }

    TaskExecuter& TaskExecuter::add_arg(const std::string& arg) {
        args.emplace_back(arg);
        return *this;
    }

    TaskExecuter& TaskExecuter::add_arg(const std::string& opt, const std::string& arg) {
        args.emplace_back(opt);
        args.emplace_back(arg);
        return *this;
    }

    [[nodiscard]] int TaskExecuter::exec() const {
        std::ostringstream ss;
        for (const auto& arg: args) {
            ss << arg << " ";
        }
        return std::system((executable_name + " " + ss.str()).c_str());
    }

    TaskExecuter exec(const std::string& executable_name) {
        return TaskExecuter(executable_name);
    }
}
