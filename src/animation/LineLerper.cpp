//
// Created by Tobiathan on 11/3/21.
//

#include "LineLerper.h"
#include "../util/Util.h"
#include "../generation/Sampler.h"

std::vector<glm::vec2> LineLerper::Lerp(const std::vector<glm::vec2>& vec1, const std::vector<glm::vec2>& vec2, float t) {

    std::vector<glm::vec2> vec;
    vec.reserve(vec1.size());

    for (int i = 0; i < vec1.size(); i++) {
        vec.emplace_back(Util::Lerp(vec1[i].x, vec2[i].x, t), Util::Lerp(vec1[i].y, vec2[i].y, t));
    }

    return vec;
}

std::vector<glm::vec2> LineLerper::MorphPolyLine(const std::vector<glm::vec2>& line1, const std::vector<glm::vec2>& line2, float t, int sampleCount) {
    return Lerp(Sampler::SampleTo(line1, sampleCount), Sampler::SampleTo(line2, sampleCount), t);
}

