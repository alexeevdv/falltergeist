#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <cctype>

#include "CrossPlatform.h"
#include "Exception.h"
#include "Logger.h"
#if defined(_WIN32) || defined(WIN32)
    #include "Platform/Windows.h"
#elif defined(__linux__)
    #include "Platform/Linux.h"
#elif defined(__APPLE__)
    #include "Platform/Apple.h"
#elif defined(BSD)
    #include "Platform/BSD.h"
#else
    #include "Platform/Unknown.h"
#endif
#include "Version.h"

#include <SDL.h>

namespace Falltergeist
{
    // static members initialization
    std::string CrossPlatform::_version;
    std::string CrossPlatform::_falloutDataPath;
    std::string CrossPlatform::_falltergeistDataPath;
    std::vector<std::string> CrossPlatform::_dataFiles;
    const std::vector<std::string> CrossPlatform::_necessaryDatFiles = {"master.dat", "critter.dat"};

    #if defined(_WIN32) || defined(WIN32)
        std::unique_ptr<Platform::IPlatform> CrossPlatform::_platform = std::make_unique<Platform::Windows>();
    #elif defined(__linux__)
        std::unique_ptr<Platform::IPlatform> CrossPlatform::_platform = std::make_unique<Platform::Linux>();
    #elif defined(__APPLE__)
        std::unique_ptr<Platform::IPlatform> CrossPlatform::_platform = std::make_unique<Platform::Apple>();
    #elif defined(BSD)
        std::unique_ptr<Platform::IPlatform> CrossPlatform::_platform = std::make_unique<Platform::BSD>();
    #else
        std::unique_ptr<Platform::IPlatform> CrossPlatform::_platform = std::make_unique<Platform::Unknown>();
    #endif

    CrossPlatform::CrossPlatform()
    {
    }

    CrossPlatform::~CrossPlatform()
    {
    }

    std::string CrossPlatform::getVersion()
    {
        if (_version.length() > 0) {
            return _version;
        }

        _version = "Falltergeist ";
        _version += PROJECT_VERSION;
        _version += " (" + _platform->name() + ")";
        return _version;
    }

    std::string CrossPlatform::getExecutableDirectory()
    {
        char* buffer = SDL_GetBasePath();
        if (buffer == nullptr) {
            Logger::warning() << "SDL_GetBasePath() not able to obtain a path on this platform" << std::endl;
            return "./";
        }
        std::string path(buffer);
        SDL_free(buffer);
        return path;
    }

    // This method is trying to find out where are the DAT files located
    std::string CrossPlatform::findFalloutDataPath()
    {
        if (_falloutDataPath.length() > 0) {
            return _falloutDataPath;
        }
        Logger::info() << "Looking for Fallout data files" << std::endl;
        std::vector<std::string> directories;
        directories.push_back(getExecutableDirectory());

        directories.push_back(_platform->applicationDataDirectory());

        try {
            std::vector<std::string> cdDrives = _platform->compactDiscDrivePaths();
            directories.insert(directories.end(), cdDrives.begin(), cdDrives.end());
        } catch (const Exception& e) {
            Logger::error() << e.what() << std::endl;
        }

        for (auto &directory : directories) {
            if (std::all_of(
                _necessaryDatFiles.begin(),
                _necessaryDatFiles.end(),
                [directory](const std::string& file) {
                    std::ifstream stream(directory + "/" + file);
                    if (stream) {
                        Logger::info() << "Searching in directory: " << directory << " " << file << " [FOUND]" << std::endl;
                        return true;
                    } else {
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
        if (_falltergeistDataPath.length() > 0) {
            return _falltergeistDataPath;
        }
        Logger::info() << "Looking for Falltergeist data files" << std::endl;
        std::vector<std::string> directories;
        directories.push_back(getExecutableDirectory());
        directories.push_back(_platform->applicationConfigDirectory());

        for (auto &directory : directories) {
            std::ifstream stream(directory + "/data/movies.lst");
            if (stream) {
                Logger::info() << "Searching in directory: " << directory << "/data/movies.lst [FOUND]" << std::endl;
                _falltergeistDataPath = directory;
                return _falltergeistDataPath;
            } else {
                Logger::info() << "Searching in directory: " << directory << "/data/movies.lst [NOT FOUND]" << std::endl;
            }
        }

        throw Exception("Falltergeist data files are not found!");
    }

    // this method looks for available dat files
    std::vector<std::string> CrossPlatform::findFalloutDataFiles()
    {
        if (_dataFiles.size()) {
            return _dataFiles;
        }

        _dataFiles = _necessaryDatFiles;

        std::vector<std::string> files = _platform->listDirectory(CrossPlatform::findFalloutDataPath());
        for (auto filename : files) {
            Logger::error() << filename << std::endl;
            std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
            // exclude . and ..
            if (filename.length() > 4) {
                std::string ext = filename.substr(filename.size() - 4, 4);
                if (ext == ".dat") {
                    if (filename.length() == 12 && filename.substr(0, 5) == "patch") {
                        _dataFiles.insert(_dataFiles.begin(), filename);
                    }
                }
            }
        }

        return _dataFiles;
    }

    void CrossPlatform::createDirectory(std::string path)
    {
        // Start index of 1 means that we skip root directory.
        for (auto dirDelim = path.find_first_of('/', 1U); dirDelim != std::string::npos;
             dirDelim = path.find_first_of('/', dirDelim + 1)) {
            path[dirDelim] = '\0';
            _platform->createDirectory(path);
            path[dirDelim] = '/';
        }
        _platform->createDirectory(path);
    }

    bool CrossPlatform::fileExists(std::string file)
    {
        return _platform->fileExists(file);
    }

    std::string CrossPlatform::getConfigPath()
    {
        return _platform->applicationDataDirectory();
    }

    uint32_t CrossPlatform::microtime()
    {
        return _platform->microtime();
    }
}
