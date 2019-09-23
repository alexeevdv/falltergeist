#pragma once

#if defined(_WIN32) || defined(WIN32)

#include "../Platform/IPlatform.h"

namespace Falltergeist
{
    namespace Platform
    {
        class Windows : public IPlatform
        {
            public:
                std::string name() override;
                std::vector<std::string> compactDiscDrivePaths() override;
                uint32_t microtime() override;
            private:
                std::string homeDirectory();
        };
    }
}
#endif
