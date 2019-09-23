#pragma once

#include <string>
#include <vector>

namespace Falltergeist
{
    namespace Platform
    {
        class IPlatform
        {
            public:
                virtual std::string name() = 0;
                virtual std::vector<std::string> compactDiscDrivePaths() = 0;
                virtual uint32_t microtime() = 0;
                virtual bool createDirectory(std::string path) = 0;
                virtual bool fileExists(std::string path) = 0;
                virtual std::string applicationConfigDirectory() = 0;
                virtual std::string applicationDataDirectory() = 0;
                virtual std::vector<std::string> listDirectory(std::string path) = 0;
        };
    }
}
