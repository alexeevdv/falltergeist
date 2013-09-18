/*
 * Copyright 2012-2013 Falltergeist Developers.
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

#include "../Engine/CrossPlatform.h"
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <string.h>



namespace Falltergeist
{
namespace CrossPlatform
{

void debug(const char * message, unsigned char level)
{
    std::cout << message;
}

const char * findDataPath()
{
    //debug("* Searching for Fallout data files\n");

    // Сначала ищем в текущей папке
    {
        //debug(" - Searching in current directory: ");
        char buffer[512];
        char * cwd = getcwd(buffer, sizeof(buffer));
        std::string path(cwd);
        path.append("/master.dat");
        std::ifstream stream(path.c_str());
        //debug(cwd);
        if (stream)
        {
            //debug(" - [OK]\n");
            return cwd;
        }
        else
        {
            //debug(" - [FAIL]\n");
        }
    }
    // Потом ищем в домашней папке .falltergeist
    {
        //debug(" - Searching in home directory: ");
        char * cwd = getenv("HOME");
        std::string path(cwd);
        path.append("/.falltergeist/master.dat");
        std::ifstream stream(path.c_str());
        //debug(cwd);
        //debug("/.falltergeist");
        if (stream)
        {
            path.clear();
            path.append(cwd);
            path.append("/.falltergeist");
            //debug(" - [OK]\n");
            return (char *) path.c_str();
        }
        else
        {
            //debug(" - [FAIL]\n");
        }
    }

    // Потом ищем подключенные диски и файлы в их корне
    // @TODO
    debug("[CRITICAL] Fallout data files were not found\n", 0);
    return 0;
}

std::vector<std::string> * findDataFiles()
{
    debug("* Getting list of data files\n");
    std::vector<std::string> * files = new std::vector<std::string>;
    files->push_back(""); // for master.dat
    files->push_back(""); // for critter.dat

    DIR * pxDir = opendir(findDataPath());
    struct dirent * pxItem = NULL;
    if(pxDir != NULL)
    {
        while((pxItem = readdir(pxDir)))
        {
            std::string filename(pxItem->d_name);
            std::transform(filename.begin(),filename.end(),filename.begin(), ::tolower);
            if (filename.length() > 4)
            {
                std::string ext = filename.substr(filename.size()-4,4);
                if (ext.compare(".dat") == 0)
                {
                    if (filename.compare("master.dat") == 0) files->at(0).append("master.dat");
                    if (filename.compare("critter.dat") == 0) files->at(1).append("critter.dat");
                    if (filename.length() == 12 && filename.substr(0,5).compare("patch") == 0) files->push_back(filename);
                }
            }
        }
        closedir(pxDir);
    }
    else
    {
        std::cout << "Unable to open specified directory." << std::endl;
    }

    if (files->at(0).compare("master.dat") != 0)
    {
        debug("[CRITICAL] master.dat not found!\n");
        return 0;
    }
    if (files->at(1).compare("critter.dat") != 0)
    {
        debug("[CRITICAL] critter.dat not found!\n");
        return 0;
    }
    std::cout << "Found: " << files->size() << std::endl;
    return files;
}

const char * findFileAlias(char * path, char * filename)
{
    DIR * dir = opendir(path);
    if (!dir) return 0;
    struct dirent * entry;
    //complex filename
    if (char * chr = strchr(filename, '/'))
    {
        int pos = chr - filename;
        std::string sfilename(filename);
        std::string spath(path);
        std::string folder(sfilename.substr(0, pos));
        // check whether there is a folder
        while ((entry = readdir(dir)))
        {
            std::string dirname(entry->d_name);
            std::transform(dirname.begin(),dirname.end(),dirname.begin(), ::tolower);
            if (dirname.compare(folder) == 0 && entry->d_type == DT_DIR)
            {
                spath.append("/").append(entry->d_name);
                const char * alias = findFileAlias((char *) spath.c_str(), (char *) sfilename.substr(pos+1, sfilename.length()).c_str());
                if (alias)
                {
                    std::string newname(entry->d_name);
                    newname.append("/");
                    newname.append(alias);
                    closedir(dir);
                    return newname.c_str();
                }
            }
        }
    }
    else
    {
        while ((entry = readdir(dir)))
        {
            std::string fname(entry->d_name);
            std::transform(fname.begin(),fname.end(),fname.begin(), ::tolower);
            if (fname.compare(filename) == 0 && entry->d_type != DT_DIR)
            {
                return entry->d_name;
            }
        }
    }
    closedir(dir);
    return 0;
}


}
}
