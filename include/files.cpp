//
// Created on 2025/4/2.
//

#include "files.h"

const char* RES_INCLUDE_SOURCE = R"(
#include "resource.h"

#include <memory>
#include <utility>


namespace moefx::resource {
#ifdef _WIN32

    RcResource::RcResource(void* ctx) {
        this->id = *static_cast<int *>(ctx);
    };

    std::vector<uint8_t> RcResource::get_data() {
        const HMODULE hModule = GetModuleHandle(nullptr);
        HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(this->id), RT_RCDATA);
        if (!hRes) {
            return {};
        }

        const HGLOBAL hMem = LoadResource(hModule, hRes);
        if (!hMem) {
            return {};
        }

        const void* pData = LockResource(hMem);
        if (!pData) {
            return {};
        }

        const DWORD size = SizeofResource(hModule, hRes);
        if (size == 0) {
            return {};
        }

        return {static_cast<const uint8_t *>(pData), static_cast<const uint8_t *>(pData) + size};
    };

#endif

    LazyLoadResource::LazyLoadResource(void* ctx): inner(ctx) {
    }

    std::vector<uint8_t> LazyLoadResource::get_data() {
        if (!loaded) {
            data = inner.get_data();
            loaded = true;
        }
        return data;
    }

    LazyLoadStringResource::LazyLoadStringResource(void* ctx): inner(ctx) {
    }

    std::string LazyLoadStringResource::get_data() {
        if (!loaded) {
            auto data = inner.get_data();
            this->data = std::string(data.begin(), data.end());
            loaded = true;
        }
        return data;
    }

    ResourceRegistry& ResourceRegistry::instance() {
        static ResourceRegistry instance;
        return instance;
    }

    void ResourceRegistry::register_resource(const std::string& name, void* ctx) {
        registry[name] = std::make_shared<RcResource>(ctx);
    }

    void ResourceRegistry::register_resource(const std::string& name, std::shared_ptr<BinaryResource> resource) {
        registry[name] = std::move(resource);
    }

    std::shared_ptr<BinaryResource> ResourceRegistry::get_resource(const std::string& name) {
        return registry[name];
    }
}
)";

const char* RES_INCLUDE_HEADER = R"(
#ifndef RESOURCE_H
#define RESOURCE_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace moefx::resource {
    class BinaryResource {
    public:
        virtual ~BinaryResource() = default;

        virtual std::vector<uint8_t> get_data() = 0;
    };

    class StringResource {
    public:
        virtual ~StringResource() = default;

        virtual std::string get_data() = 0;
    };

#ifdef _WIN32

#include <windows.h>

    class RcResource : public BinaryResource {
        int id;

    public:
        explicit RcResource(void* ctx);

        std::vector<uint8_t> get_data() override;
    };


    using ResourceInner = RcResource;
#endif

    class LazyLoadResource : public BinaryResource {
        std::vector<uint8_t> data;
        bool loaded = false;

        ResourceInner inner;

    public:
        explicit LazyLoadResource(void* ctx);

        std::vector<uint8_t> get_data() override;
    };

    class LazyLoadStringResource : public StringResource {
        std::string data;
        bool loaded = false;

        ResourceInner inner;

    public:
        explicit LazyLoadStringResource(void* ctx);

        std::string get_data() override;
    };

    template <typename T>
    void* void_cast(T&& x) {
        return reinterpret_cast<void *>(std::addressof(x));
    }

    class ResourceRegistry {
        std::unordered_map<std::string, std::shared_ptr<BinaryResource>> registry;

    public:
        ResourceRegistry() = default;

        ResourceRegistry(const ResourceRegistry&) = delete;

        ResourceRegistry(ResourceRegistry&&) = delete;

        static ResourceRegistry& instance();

        void register_resource(const std::string& name, void* ctx);

        void register_resource(const std::string& name, std::shared_ptr<BinaryResource> resource);

        std::shared_ptr<BinaryResource> get_resource(const std::string& name);
    };
}

#endif //RESOURCE_H
)";

namespace moe_assets {
    std::string FileSystemIterator::as_relative(const std::string& path) const {
        return replace_slash(std::filesystem::relative(path, this->path).string());
    }

    std::string FileSystemIterator::replace_slash(const std::string& path) {
        return std::regex_replace(path, std::regex("\\\\"), "/");
    }

    std::vector<filepath> FileSystemIterator::iter_sub_dir(const std::string& path) {
        std::vector<filepath> result;
        for (const auto& entry: std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string file_path = replace_slash(entry.path().string());
                result.emplace_back(as_relative(file_path), file_path);
            } else {
                std::vector<filepath> sub_files = iter_sub_dir(entry.path().string());
                result.insert(result.end(), sub_files.begin(), sub_files.end());
            }
        }
        return result;
    }

    FileSystemIterator::FileSystemIterator(const std::string& path) : path(path) {
        for (const auto& entry: std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string file_path = replace_slash(entry.path().string());
                files.emplace_back(as_relative(file_path), file_path);
            } else {
                std::vector<filepath> sub_files = iter_sub_dir(entry.path().string());
                files.insert(files.end(), sub_files.begin(), sub_files.end());
            }
        }
    }

    std::vector<filepath> FileSystemIterator::get_files() {
        return files;
    }

#ifdef _WIN32
    ResourceFileGenerator::ResourceFileGenerator(const std::vector<filepath>& files) {
        size_t file_count = 100;
        for (const auto& [fst, snd]: files) {
            ss << file_count << " ";
            ss << "RCDATA" << " ";
            ss << snd << "\n";

            file_map[fst] = file_count;

            file_count++;
        }
    }

    std::string ResourceFileGenerator::get() const {
        return ss.str();
    }

    std::unordered_map<std::string, size_t> ResourceFileGenerator::get_file_map() const {
        return file_map;
    }

    ResourceFileRegisterGenerator::ResourceFileRegisterGenerator(
        const std::unordered_map<std::string, size_t>& file_map) {
        ss << "// This file is generated by moe-assets." << "\n";
        ss << "// Do not modify this file." << "\n\n";
        ss << "#include \"resource.h\"" << "\n";
        ss << "inline void _regist_all_resources() {" << "\n";
        ss << "    " << "auto& instance = moefx::resource::ResourceRegistry::instance();" << "\n";
        for (const auto& [key, value]: file_map) {
            ss << "    " << "instance.register_resource(\""
                    << key << "\", moefx::resource::void_cast(" << value << "));" << "\n";
        }
        ss << "}";
    }

    std::string ResourceFileRegisterGenerator::get() const {
        return ss.str();
    }

    std::string ResourceFileRegisterGenerator::get_header() {
        return {RES_INCLUDE_HEADER};
    }

    std::string ResourceFileRegisterGenerator::get_source() {
        return {RES_INCLUDE_SOURCE};
    }

#endif
}
