#pragma once

#if defined (__APPLE__)

#include "../Platform/IPlatform.h"

namespace Falltergeist
{
    namespace Platform
    {
        class Apple : public IPlatform
        {
            public:
                std::string name() override;
                std::vector<std::string> compactDiscDrivePaths() override;
                uint32_t microtime() override;
                bool createDirectory(std::string path) override;
                bool fileExists(std::string path) override;
                std::string applicationConfigDirectory() override;
                std::string applicationDataDirectory() override;
                std::vector<std::string> listDirectory(std::string path) override;

            private:
                std::string homeDirectory();
        };
    }
}
#endif
