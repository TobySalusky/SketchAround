//
// Created by Tobiathan on 9/19/21.
//

#include "Util.h"
#include <fstream>


std::string Util::ReadFile(const char *path) {
    std:: string content;
    std::ifstream fileStream(path, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read %s! File doesn't exist!", path);
    }

    std::string line;
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + '\n');
    }
    fileStream.close();

    return content;
}