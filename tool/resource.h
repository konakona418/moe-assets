//
// Created on 2025/4/2.
//

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
