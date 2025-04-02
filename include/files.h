//
// Created on 2025/4/2.
//

#ifndef FILES_H
#define FILES_H

#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <regex>
#include <unordered_map>

namespace moe_assets {
    using filepath = std::pair<std::string, std::string>;

    inline bool is_end_with_slash(const std::string& path) {
        return path.back() == '/' || path.back() == '\\';
    }

    inline std::string concat_path(const std::string& base_path, const std::string& name) {
        if (is_end_with_slash(base_path)) {
            return base_path + name;
        }
        return base_path + "/" + name;
    }

    template <typename... Args>
    std::string concat_path(const std::string& base_path, const Args&... args) {
        std::string result = base_path;
        std::vector<std::string> args_vec{args...};
        for (const auto& arg: args_vec) {
            result = concat_path(result, arg);
        }
        return result;
    }

    inline void write_file(const std::string& base_path, const std::string& name, const std::string& content) {
        std::ofstream file(concat_path(base_path, name), std::ios::out);
        file << content;
        file.close();
    }

    inline void create_folder(const std::string& base_path, const std::string& name) {
        std::filesystem::create_directory(concat_path(base_path, name));
    }

    inline void clear_folder(const std::string& base_path, const std::string& name) {
        std::filesystem::remove_all(concat_path(base_path, name));
    }

    inline bool exists(const std::string& base_path, const std::string& name) {
        return std::filesystem::exists(concat_path(base_path, name));
    }

    class FileSystemIterator {
        std::string path;

        std::vector<filepath> files;

        [[nodiscard]] std::string as_relative(const std::string& path) const;

        static std::string replace_slash(const std::string& path);

        std::vector<filepath> iter_sub_dir(const std::string& path);

    public:
        explicit FileSystemIterator(const std::string& path);

        std::vector<filepath> get_files();
    };

#ifdef _WIN32
    class ResourceFileGenerator {
        std::stringstream ss;

        std::unordered_map<std::string, size_t> file_map;

    public:
        explicit ResourceFileGenerator(const std::vector<filepath>& files);

        std::string get() const;

        std::unordered_map<std::string, size_t> get_file_map() const;
    };

    class ResourceFileRegisterGenerator {
        std::stringstream ss;

    public:
        explicit ResourceFileRegisterGenerator(const std::unordered_map<std::string, size_t>& file_map);

        std::string get() const;

        static std::string get_header();

        static std::string get_source();
    };
#endif
}


#endif //FILES_H
