#pragma once

#include <list>
#include <memory>
#include <string>
#include <vector>
#include "Platform/IPlatform.h"

namespace Falltergeist
{
    class CrossPlatform
    {
        public:
            CrossPlatform();
            ~CrossPlatform();

            static std::string getVersion();
            static std::string findFalloutDataPath();
            static std::vector<std::string> findFalloutDataFiles();
            static std::string findFalltergeistDataPath();
            static std::string getConfigPath();

            // Create directory at given path with parent directories as needed.
            // Throw runtime_error on failure.
            static void createDirectory(std::string path);

            static bool fileExists(std::string file);

            static uint32_t microtime();

        protected:

            static std::unique_ptr<Platform::IPlatform> _platform;
            static std::string _falloutDataPath;
            static std::string _falltergeistDataPath;
            static std::vector<std::string> _dataFiles;
            static std::string _version;
            const static std::vector<std::string> _necessaryDatFiles;
            static std::string getExecutableDirectory();
    };
}
