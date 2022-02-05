//
// Created by Tobiathan on 2/5/22.
//

#ifndef SENIORRESEARCH_OBJEXPORTER_H
#define SENIORRESEARCH_OBJEXPORTER_H

#include <string>
#include <vector>
#include "../generation/ModelObject.h"


class ObjExporter {
public:
    static std::string GenerateFileContents(const std::vector<ModelObject*>& modelObjects);
private:

};


#endif //SENIORRESEARCH_OBJEXPORTER_H
