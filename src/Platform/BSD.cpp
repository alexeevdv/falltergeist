#if defined(BSD)

#include <sys/mount.h>
#include <sys/ucred.h>
#include "../Logger.h"
#include "../Platform/BSD.h"

namespace Falltergeist
{
    namespace Platform
    {
        std::string BSD::name()
        {
            return "BSD";
        }

        std::vector<std::string> BSD::compactDiscDrivePaths()
        {
            std::vector<std::string> result;
            #if defined (BSD)
                struct statfs *mntbuf;
                int mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
                for ( int i = 0; i < mntsize; i++ ) {
                    std::string directory = ((struct statfs *)&mntbuf[i])->f_mntonname;
                    std::string type = ((struct statfs *)&mntbuf[i])->f_fstypename;
                    if (type == "cd9660") {
                        result.push_back(directory);
                    }
                }
            #endif
            return result;
        }

        uint32_t BSD::microtime()
        {
            Logger::error() << "Microtime is not supported" << std::endl;
            return 0;
        }
    }
}
#endif
