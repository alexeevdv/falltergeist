/*
 * Copyright 2012-2015 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

// C++ standard includes

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <stdexcept>
#include <SDL.h>

#if defined(__unix__) || defined(__APPLE__)
    #include <sys/param.h>
#endif

#if defined(__linux__)
    #include <mntent.h>
#endif

#if defined (__APPLE__) || defined(BSD)
    #include <sys/ucred.h>
    #include <sys/mount.h>
#endif

#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <shlobj.h>
#elif defined(__unix__) || defined(__APPLE__)
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

// Falltergeist includes
#include "CrossPlatform.h"
#include "Exception.h"
#include "Logger.h"

// Third party includes

namespace Falltergeist
{

// static members initialization
std::string CrossPlatform::_version;
std::string CrossPlatform::_falloutDataPath;
std::string CrossPlatform::_falltergeistDataPath;
std::vector<std::string> *CrossPlatform::_dataFiles = 0;
const std::vector<std::string> CrossPlatform::necessaryDatFiles = {"master.dat", "critter.dat"};


CrossPlatform::CrossPlatform()
{
}

CrossPlatform::~CrossPlatform()
{
}

std::string CrossPlatform::getVersion()
{
    if (_version.length() > 0) return _version;

    _version = "Falltergeist 0.2.0";
#if defined(_WIN32) || defined(WIN32)
    _version += " (Windows)";
#elif defined(__ANDROID__)
    _version += " (Android)";
#elif defined(__linux__)
    _version += " (Linux)";
#elif defined(__APPLE__)
    _version += " (Apple)";
#elif defined(BSD)
    _version += " (BSD)";
#endif
    return _version;
}

std::string CrossPlatform::getHomeDirectory()
{
#if defined(_WIN32) || defined(WIN32)
    char cwd[256];
    LPITEMIDLIST pidl;
    SHGetSpecialFolderLocation(NULL, CSIDL_PROFILE  ,&pidl);
    SHGetPathFromIDList(pidl, cwd);
#elif defined (__ANDROID__)
    char *cwd = "";
#else
    char *cwd = getenv("HOME");
#endif
    return std::string(cwd);
}

std::string CrossPlatform::getExecutableDirectory()
{
#if defined(__ANDROID__)
    std::string path("/");
#else
    char* buffer=SDL_GetBasePath();
    std::string path(buffer);
    SDL_free(buffer);
#endif
    return path;
}

std::vector<std::string> CrossPlatform::getCdDrivePaths()
{
    std::vector<std::string> result;
#if defined(_WIN32) || defined(WIN32)
    // Looking for data files on CD-ROM drives
    char buf[256];
    GetLogicalDriveStringsA(sizeof(buf), buf);

    for(char * s = buf; *s; s += strlen(s) + 1)
    {
        if (GetDriveTypeA(s) == DRIVE_CDROM)
        {
            result.push_back(std::string(s));
        }
    }

#elif defined(__linux__) && !defined(__ANDROID__)
    FILE *mtab = setmntent("/etc/mtab", "r");
    struct mntent *m;
    struct mntent mnt;
    char strings[4096];
    while ((m = getmntent_r(mtab, &mnt, strings, sizeof(strings))))
    {
        std::string directory = mnt.mnt_dir;
        std::string type = mnt.mnt_type;
        if (type == "iso9660")
        {
            result.push_back(directory);
        }
    }
    endmntent(mtab);
#elif defined (BSD)
    struct statfs *mntbuf;
    int mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
    for ( int i = 0; i < mntsize; i++ )
    {
        std::string directory = ((struct statfs *)&mntbuf[i])->f_mntonname;
        std::string type = ((struct statfs *)&mntbuf[i])->f_fstypename;
        if (type == "cd9660")
        {
            result.push_back(directory);
        }
    }
#else
    throw Exception("CD-ROM detection not supported");
#endif
    return result;
}

// This method is trying to find out where are the DAT files located
std::string CrossPlatform::findFalloutDataPath()
{
    if (_falloutDataPath.length() > 0)
        return _falloutDataPath;
    Logger::info() << "Looking for Fallout data files" << std::endl;
    std::vector<std::string> directories;
    directories.push_back(getExecutableDirectory());

    for (auto &directory : getDataPaths())
    {
        directories.push_back(directory);
    }

    try
    {
        std::vector<std::string> cdDrives = getCdDrivePaths();
        directories.insert(directories.end(), cdDrives.begin(), cdDrives.end());
    }
    catch (Exception e)
    {
        Logger::error() << e.what() << std::endl;
    }

    for (auto &directory : directories)
    {
        if (std::all_of(
                necessaryDatFiles.begin(),
                necessaryDatFiles.end(),
                [directory](std::string file) {
                    std::ifstream stream(directory + "/" + file);
                    if (stream)
                    {
                        Logger::info() << "Searching in directory: " << directory << " " << file << " [FOUND]" << std::endl;
                        return true;
                    }
                    else
                    {
                        Logger::info() << "Searching in directory: " << directory << " " << file << " [NOT FOUND]" << std::endl;
                        return false;
                    }
                })
                )
        {
            _falloutDataPath = directory;
            return _falloutDataPath;
        }
    }

    throw Exception("Fallout data files are not found!");
}

std::string CrossPlatform::findFalltergeistDataPath()
{
    if (_falltergeistDataPath.length() > 0)
        return _falltergeistDataPath;
    Logger::info() << "Looking for Falltergeist data files" << std::endl;
    std::vector<std::string> directories;
    directories.push_back(getExecutableDirectory());

    for (auto &directory : getDataPaths())
    {
        directories.push_back(directory);
    }

    for (auto &directory : directories)
    {
        std::ifstream stream(directory + "/data/dialogs.lst");
        if (stream)
        {
            Logger::info() << "Searching in directory: " << directory << " data/dialogs.lst [FOUND]" << std::endl;
            _falltergeistDataPath = directory;
            return _falltergeistDataPath;
        }
        else
        {
            Logger::info() << "Searching in directory: " << directory << " data/dialogs.lst [NOT FOUND]" << std::endl;
        }
    }

    throw Exception("Falltergeist data files are not found!");
}

// this method looks for available dat files
std::vector<std::string> *CrossPlatform::findFalloutDataFiles()
{
    if (_dataFiles)
        return _dataFiles;

    // looking for all available dat files in directory
    DIR *pxDir = opendir(CrossPlatform::findFalloutDataPath().c_str());
    if (!pxDir)
    {
        throw Exception("Can't open data directory: " + CrossPlatform::findFalloutDataPath());
    }
    _dataFiles = new std::vector<std::string>(necessaryDatFiles);
    struct dirent *pxItem = 0;
    while ((pxItem = readdir(pxDir)))
    {
        std::string filename(pxItem->d_name);
        if (filename.length() > 4) // exclude . and ..
        {
            std::string ext = filename.substr(filename.size() - 4, 4);
            if (ext == ".dat")
            {
                if (filename.length() == 12 && filename.substr(0, 5) == "patch")
                    _dataFiles->insert(_dataFiles->begin(),filename);
            }
        }
    }
    closedir(pxDir);

    return _dataFiles;
}

bool CrossPlatform::_createDirectory(const char *dir)
{
#if defined(__unix__) || defined(__APPLE__) // Linux, OS X, BSD
    struct stat st;

    if (stat(dir, &st) == 0)
    {
        // Directory already exists
        if (S_ISDIR(st.st_mode))
            return false;

        throw std::runtime_error("Path `" + std::string(dir) + "' already exists and is not a directory");
    }
    else
    {
        if (mkdir(dir, S_IRWXU) == 0)
            return true;

        throw std::runtime_error(strerror(errno));
    }
#elif defined(_WIN32) || defined(WIN32) // Windows
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
#else
#error Platform not supported: CrossPlatform::createDirectoryIfNotExists not implemented
#endif
    return false;
}

char *CrossPlatform::_copyString(const char *str)
{
#if defined(__unix__) || defined(__APPLE__)
    return strdup(str);
#else
    char *ret = (char*)malloc((strlen(str) + 1) * sizeof(*str));
    if (ret != nullptr) strcpy(ret, str);
    return ret;
#endif
}

void CrossPlatform::createDirectory(const std::string &path)
{
    char *dir = _copyString(path.c_str());

    try
    {
        for (char *dirDelim = strchr(dir, '/'); dirDelim != nullptr; dirDelim = strchr(dirDelim + 1, '/'))
        {
            *dirDelim = '\0';
            // Skip root directory
            if (dir != dirDelim) _createDirectory(dir);
            *dirDelim = '/';
        }
        _createDirectory(dir);
    }
    catch (const std::runtime_error &)
    {
        free(dir);
        throw;
    }

    free(dir);
}

std::string CrossPlatform::getConfigPath()
{
#if defined(__unix__) && !defined(__ANDROID__)
    char *maybeConfigHome = getenv("XDG_CONFIG_HOME");
    if (maybeConfigHome == nullptr || *maybeConfigHome == '\0')
        return getHomeDirectory() + "/.config/falltergeist";
    return std::string(maybeConfigHome) + "/falltergeist";
#elif defined(__APPLE__)
    return getHomeDirectory() + "/Library/Application Support/falltergeist";
#elif defined(_WIN32) || defined(WIN32)
    char path[256];
    SHGetFolderPath(nullptr, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path);
    return std::string(path) + "/falltergeist";
#elif defined(__ANDROID__)
    return std::string(SDL_AndroidGetInternalStoragePath());
#else
#error Platform not supported: CrossPlatform::getConfigPath not implemented
#endif
}

std::vector<std::string> CrossPlatform::getDataPaths()
{
    std::vector<std::string> _dataPaths;
#if defined(__unix__) && !defined(__ANDROID__)
    char *maybeDataHome = getenv("XDG_DATA_HOME");
    if (maybeDataHome == nullptr || *maybeDataHome == '\0')
        _dataPaths.push_back(getHomeDirectory() + "/.local/share/falltergeist");
    else
        _dataPaths.push_back(std::string(maybeDataHome) + "/falltergiest");

    std::string sharedir = getExecutableDirectory()+"/../share/falltergeist";

    DIR *pxDir = opendir(sharedir.c_str());
    if(pxDir)
    {
        _dataPaths.push_back(sharedir.c_str());
    }
    closedir(pxDir);
#elif defined(__ANDROID__)
    _dataPaths.push_back("/sdcard/falltergeist");
    _dataPaths.push_back("/mnt/extSdCard/falltergeist");
    _dataPaths.push_back("/mnt/sdcard/falltergeist");
    _dataPaths.push_back("/storage/sdcard0/falltergeist");
    _dataPaths.push_back("/storage/sdcard1/falltergeist");
    _dataPaths.push_back("/storage/extSdCard/falltergeist");
#else
    _dataPaths.push_back(getConfigPath());
#endif
    return _dataPaths;
}

}
