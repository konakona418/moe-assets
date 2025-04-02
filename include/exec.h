//
// Created on 2025/4/2.
//

#ifndef EXEC_H
#define EXEC_H

#include <string>
#include <vector>

namespace moe_assets {
    class TaskExecuter {
        std::string executable_name;
        std::vector<std::string> args;

    public:
        explicit TaskExecuter(const std::string& executable_name);

        TaskExecuter& add_arg(const std::string& arg);

        TaskExecuter& add_arg(const std::string& opt, const std::string& arg);

        [[nodiscard]] int exec() const;
    };

    TaskExecuter exec(const std::string& executable_name);
}


#endif //EXEC_H
