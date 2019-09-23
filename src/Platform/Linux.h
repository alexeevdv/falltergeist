#pragma once

#if defined(__linux__)

#include "../Platform/IPlatform.h"

namespace Falltergeist
{
    namespace Platform
    {
        class Linux : public IPlatform
        {
            public:
                std::string name() override;
                std::vector<std::string> compactDiscDrivePaths() override;
                uint32_t microtime() override;
                bool fileExists(std::string path) override;
                bool createDirectory(std::string path) override;
                std::vector<std::string> listDirectory(std::string path) override;
                std::string applicationConfigDirectory() override;
                std::string applicationDataDirectory() override;

                private:
                std::string homeDirectory();
        };
    }
}

#endif
