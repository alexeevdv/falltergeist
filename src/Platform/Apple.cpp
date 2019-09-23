#if defined (__APPLE__)

#include <string>
#include <vector>
#include <mach/mach_time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/param.h>
#include <dirent.h>
#include <sys/mount.h>
#include <sys/ucred.h>
#include "../Logger.h"
#include "../Platform/Apple.h"

namespace Falltergeist
{
    namespace Platform
    {
        std::string Apple::name()
        {
            return "Apple";
        }

        std::vector<std::string> Apple::compactDiscDrivePaths()
        {
            Logger::warning() << "CD-ROM detection not supported" << std::endl;
            return std::vector<std::string>();
        }

        uint32_t Apple::microtime()
        {
            static mach_timebase_info_data_t timebase;
            if ( timebase.denom == 0 ) {
                (void) mach_timebase_info(&timebase);
            }

            uint64_t time = mach_absolute_time();
            return static_cast<uint32_t>(((double)time * (double)timebase.numer) / ((double)timebase.denom));
        }

        bool Falltergeist::Platform::Apple::createDirectory(std::string path)
        {
            Logger::error() << "Create directory is not supported" << std::endl;
            return false;
        }

        bool Falltergeist::Platform::Apple::fileExists(std::string path)
        {
            Logger::error() << "File exists is not supported" << std::endl;
            return false;
        }

        std::string Falltergeist::Platform::Apple::applicationConfigDirectory()
        {
            return homeDirectory() + "/Library/Application Support/falltergeist";
        }

        std::string Falltergeist::Platform::Apple::applicationDataDirectory()
        {
            return homeDirectory() + "/Library/Application Support/falltergeist";
        }

        std::vector <std::string> Falltergeist::Platform::Apple::listDirectory(std::string path)
        {
            Logger::error() << "Directory listing is not supported" << std::endl;
            return std::vector<std::string>();
        }

        std::string Apple::homeDirectory()
        {
            char *cwd = getenv("HOME");
            return std::string(cwd);
        }
    }
}
#endif
