#pragma once

#if defined(BSD)

#include "../Platform/IPlatform.h"

namespace Falltergeist
{
    namespace Platform
    {
        class BSD : public IPlatform
        {
            public:
                std::string name() override;
                std::vector<std::string> compactDiscDrivePaths() override;
                uint32_t microtime() override;
        };
    }
}
#endif
