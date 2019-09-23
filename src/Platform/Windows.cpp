#if defined(_WIN32) || defined(WIN32)

#include <shlobj.h>
#include <windows.h>
#include "../Platform/Windows.h"

namespace Falltergeist
{
    namespace Platform
    {
        std::string Windows::name()
        {
            return "Windows";
        }

        std::vector<std::string> Windows::compactDiscDrivePaths()
        {
            std::vector<std::string> results;
            char buf[256];
            GetLogicalDriveStringsA(sizeof(buf), buf);

            for(char * s = buf; *s; s += strlen(s) + 1) {
                if (GetDriveTypeA(s) == DRIVE_CDROM) {
                    results.push_back(std::string(s));
                }
            }
            return results;
        }

        uint32_t Windows::microtime()
        {
            auto now = std::chrono::high_resolution_clock::now();
            return static_cast<uint32_t>(std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count());
        }

        std::string Windows::homeDirectory()
        {
            char cwd[256];
            LPITEMIDLIST pidl;
            SHGetSpecialFolderLocation(NULL, CSIDL_PROFILE  ,&pidl);
            SHGetPathFromIDList(pidl, cwd);
            return std::string(cwd);
        }

        bool Windows::createDirectory()
        {
            DWORD attrs = GetFileAttributes(dir);

            // Assume path exists
            if (attrs != INVALID_FILE_ATTRIBUTES)
            {
                // Directory already exists
                if (attrs & FILE_ATTRIBUTE_DIRECTORY) return false;

                throw std::runtime_error("Path `" + std::string(dir) + "' already exists and is not a directory");
            }
            else
            {
                if (CreateDirectory(dir,NULL) != 0) return true;

                DWORD errorId = GetLastError();

                // FIXME: Use FormatMessage to get error string
                throw std::runtime_error("CreateDirectory failed with code: " + std::to_string(errorId));
            }
        }

        bool Windows::fileExists(std::string path)
        {
            DWORD attrs = GetFileAttributes(path.c_str());
            return attrs != INVALID_FILE_ATTRIBUTES;
        }

        std::string Windows::applicationDataPath()
        {
            char path[256];
            SHGetFolderPath(nullptr, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path);
            return std::string(path) + "/falltergeist";
        }

        std::vector<std::string> Windows::listDirectory(std::string path)
        {
            path = path + "*";
            std::vector<std::string> files;
            HANDLE hFind = INVALID_HANDLE_VALUE;
            WIN32_FIND_DATA ffd;
            hFind = FindFirstFile(path.c_str(), &ffd);

            if (INVALID_HANDLE_VALUE == hFind) {
                return files;
            }

            do {
                if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    continue;
                }
                std::string filename(ffd.cFileName);
                files.push_back(filename);
            } while (FindNextFile(hFind, &ffd) != 0);
            return files;
        }
    }
}
#endif

