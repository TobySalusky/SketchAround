//
// Created by Tobiathan on 2/5/22.
//

#include "ObjExporter.h"

// TODO: use buffer (since large files may cause issues)
std::string ObjExporter::GenerateFileContents(const std::vector<std::shared_ptr<ModelObject>>& modelObjects) {
    std::string vertexContents, faceContents;

    int startVertexIndex = 1;

    for (const auto& modelObject : modelObjects) {
        TopologyCorrector topologyData;
        const auto& [vertices, indices] = modelObject->GenMeshTuple(&topologyData);
        std::vector<std::vector<unsigned int>> faces;

        const glm::mat4 modelMat = modelObject->GenModelMat();
        for (const Vec3& vert : vertices) {
            const Vec3 realVert = modelMat * Vec4(vert, 1.0f);
            vertexContents += "v " + std::to_string(realVert.x) + " " + std::to_string(realVert.y) + " " + std::to_string(realVert.z) + "\n";
        }

        { // TODO: extract to helper method
            if (topologyData.wrapStartPoints > 0) {
                std::vector<unsigned int> startFace;
                startFace.reserve(topologyData.wrapStartPoints);
                for (int i = 0; i < topologyData.wrapStartPoints; i++) {
                    startFace.emplace_back(topologyData.wrapStartPoints - i - 1);
                }
                faces.emplace_back(startFace);
            }

            for (int i = 0; i < topologyData.sideQuadCount; i ++) {
                unsigned int startQuadIndex = i * 6;
                faces.emplace_back(std::vector<unsigned int>{indices[startQuadIndex], indices[startQuadIndex + 2], indices[startQuadIndex + 5], indices[startQuadIndex + 1]});
            }

            if (topologyData.wrapEndPoints > 0) {
                std::vector<unsigned int> endFace;
                endFace.reserve(topologyData.wrapEndPoints);
                for (int i = 0; i < topologyData.wrapEndPoints; i++) {
                    endFace.emplace_back(vertices.size() - topologyData.wrapEndPoints + i);
                }
                faces.emplace_back(endFace);
            }
        }

        std::string faceLine;
        for (const std::vector<unsigned int>& face : faces) {
            faceLine = "f";
            for (unsigned int index : face) {
                faceLine += " " + std::to_string(index + startVertexIndex);
            }
            faceContents += faceLine + "\n";
        }

        startVertexIndex += (int) vertices.size();
    }

    return "# Generated using SeniorResearch\n\n" + vertexContents + "\n" + faceContents; // TODO: change title of program
}
