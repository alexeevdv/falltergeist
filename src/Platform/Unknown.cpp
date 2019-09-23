#include "../Logger.h"
#include "../Platform/Unknown.h"

namespace Falltergeist
{
    namespace Platform
    {
        std::string Unknown::name()
        {
            return "Unknown";
        }

        std::vector<std::string> Unknown::compactDiscDrivePaths()
        {
            Logger::warning() << "CD-ROM detection is not supported" << std::endl;
            return std::vector<std::string>();
        }

        uint32_t Unknown::microtime()
        {
            Logger::error() << "Microtime is not supported" << std::endl;
            return 0;
        }
    }
}
