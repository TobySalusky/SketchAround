//
// Created by Tobiathan on 9/19/21.
//

#include "Util.h"
#include <fstream>


std::string Util::ReadFile(const char *path) {
    std::string content;
    std::ifstream fileStream(path, std::ios::in);

    if (!fileStream.is_open()) {
        LOG("Failed to read %s! File doesn't exist!", path);
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

glm::vec2 Util::Polar(float mag, float angle) {
    return {sin(angle) * mag, cos(angle) * mag};
}

glm::vec2 Util::AveragePos(const std::vector<glm::vec2> &points)  {
    glm::vec2 pointSum = std::accumulate(points.begin(),  points.end(), glm::vec2(0, 0), [](glm::vec2 p1, glm::vec2 p2) {
        return p1 + p2;
    });
    return pointSum / (float) points.size();
}
