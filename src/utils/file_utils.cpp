#include <iostream>
#include <string>
#include <vector>

#include "utils/file_utils.h"

#ifdef _WIN32
#include <windows.h>

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> getObjFiles(const std::string& folderPath) {
    std::vector<std::string> objFiles;
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((folderPath + "\\*").c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open directory: " << folderPath << std::endl;
        return objFiles;
    }
    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::string fileName = findFileData.cFileName;
            if (endsWith(fileName, ".obj")) {
                objFiles.push_back(fileName);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
    return objFiles;
}

#elif __linux__ || __APPLE__
#include <dirent.h>
#include <sys/types.h>

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> getObjFiles(const std::string& folderPath) {
    std::vector<std::string> objFiles;
    DIR* dir = opendir(folderPath.c_str());
    if (dir == nullptr) {
        std::cerr << "Failed to open directory: " << folderPath << std::endl;
        return objFiles;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip directories (entry->d_type == DT_DIR)
        if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
            std::string fileName = entry->d_name;
            if (endsWith(fileName, ".obj")) {
                objFiles.push_back(fileName);
            }
        }
    }
    closedir(dir);
    return objFiles;
}
#else
#error "Unsupported platform"
#endif

