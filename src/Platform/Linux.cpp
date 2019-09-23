#if defined(__linux__)

#include <dirent.h>
#include <mntent.h>
#include <stdexcept>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

#include "../Exception.h"
#include "../Platform/Linux.h"

namespace Falltergeist
{
    namespace Platform
    {
        std::string Linux::name()
        {
            return "Linux";
        }

        std::vector<std::string> Linux::compactDiscDrivePaths()
        {
            std::vector<std::string> results;
            FILE *mtab = setmntent("/etc/mtab", "r");
            struct mntent *m;
            struct mntent mnt;
            char strings[4096];
            while ((m = getmntent_r(mtab, &mnt, strings, sizeof(strings)))) {
                std::string directory = mnt.mnt_dir;
                std::string type = mnt.mnt_type;
                if (type == "iso9660") {
                    results.push_back(directory);
                }
            }
            endmntent(mtab);
            return results;
        }

        uint32_t Linux::microtime()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return ts.tv_nsec;
        }

        std::string Linux::applicationConfigDirectory()
        {
            char *maybeConfigHome = getenv("XDG_CONFIG_HOME");
            if (maybeConfigHome == nullptr || *maybeConfigHome == '\0') {
                return homeDirectory() + "/.config/falltergeist";
            }
            return std::string(maybeConfigHome) + "/falltergeist";
        }

        std::string Linux::applicationDataDirectory()
        {
            char *maybeDataHome = getenv("XDG_DATA_HOME");
            if (maybeDataHome == nullptr || *maybeDataHome == '\0') {
                return homeDirectory() + "/.local/share/falltergeist";
            }
            return std::string(maybeDataHome) + "/falltergeist";
        }

        std::string Linux::homeDirectory()
        {
            char *cwd = getenv("HOME");
            return std::string(cwd);
        }

        bool Linux::createDirectory(std::string path)
        {
            struct stat st;

            if (stat(path.c_str(), &st) == 0) {
                // Directory already exists
                if (S_ISDIR(st.st_mode)) {
                    return false;
                }

                throw std::runtime_error("Path `" + path + "` already exists and is not a directory");
            }

            if (mkdir(path.c_str(), S_IRWXU) == 0) {
                return true;
            }

            throw std::runtime_error(strerror(errno));
        }

        bool Linux::fileExists(std::string path)
        {
            struct stat st;
            return stat(path.c_str(), &st) == 0;
        }

        std::vector<std::string> Linux::listDirectory(std::string path)
        {
            std::vector<std::string> files;
            // looking for all available dat files in directory
            DIR *pxDir = opendir(path.c_str());
            if (!pxDir) {
                throw Exception("Can't open data directory: " + path);
            }

            struct dirent *pxItem = 0;
            while ((pxItem = readdir(pxDir))) {
                std::string filename(pxItem->d_name);
                files.push_back(filename);
            }
            closedir(pxDir);
            return files;
        }
    }
}
#endif

