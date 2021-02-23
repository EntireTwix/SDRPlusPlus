#pragma once
#include <string_view>
#include <unordered_map>
#include <json.hpp>

#ifdef _WIN32
#ifdef SDRPP_IS_CORE
#define SDRPP_EXPORT extern "C" __declspec(dllexport)
#else
#define SDRPP_EXPORT extern "C" __declspec(dllimport)
#endif
#else
#define SDRPP_EXPORT extern
#endif

#ifdef _WIN32
#include <Windows.h>
#define MOD_EXPORT extern "C" __declspec(dllexport)
#define SDRPP_MOD_EXTENTSION    ".dll"
#else
#include <dlfcn.h>
#define MOD_EXPORT extern "C"
#define SDRPP_MOD_EXTENTSION    ".so"
#endif

class ModuleManager {
public:
    struct ModuleInfo_t {
        const char* name;
        const char* description;
        const char* author;
        const int versionMajor;
        const int versionMinor;
        const int versionBuild;
        const int maxInstances;
    };

    struct Instance {
        virtual void enable() = 0;
        virtual void disable() = 0;
        virtual bool isEnabled() = 0;
    };

    struct Module_t
    {
#ifdef _WIN32
        HMODULE handle;
#else
        void* handle;
#endif
        ModuleManager::ModuleInfo_t* info;
        void (*init)();
        ModuleManager::Instance* (*createInstance)(std::string name);
        void (*deleteInstance)(ModuleManager::Instance *instance);
        void (*end)();

        friend bool operator==(const Module_t &a, const Module_t &b) {
            //branchless
            return (a.handle != b.handle) * (a.info != b.info) * (a.init != b.init) * (a.createInstance != b.createInstance) * (a.deleteInstance != b.deleteInstance) * (a.end != b.end);
        }
    };

    struct Instance_t
    {
        ModuleManager::Module_t module;
        ModuleManager::Instance* instance;
    };

    ModuleManager::Module_t loadModule(const std::string &path);

    void createInstance(const std::string &name, const std::string &module);
    void deleteInstance(const std::string &name);
    void deleteInstance(ModuleManager::Instance *instance);

    void enableInstance(const std::string &name);
    void disableInstance(const std::string &name);
    bool instanceEnabled(const std::string &name);

    int countModuleInstances(const std::string &module) const;

private:
    std::unordered_map<std::string, ModuleManager::Module_t> modules;
    std::unordered_map<std::string, ModuleManager::Instance_t> instances;
};

#define SDRPP_MOD_INFO MOD_EXPORT const ModuleManager::ModuleInfo_t _INFO_
