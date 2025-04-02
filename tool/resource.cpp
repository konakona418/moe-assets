//
// Created on 2025/4/2.
//

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

