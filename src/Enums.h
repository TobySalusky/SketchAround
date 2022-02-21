//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_ENUMS_H
#define SENIORRESEARCH_ENUMS_H

#include <vector>

class Enums {
public:
// Enums

    enum Direction {
        HORIZONTAL, VERTICAL
    };
    enum DrawMode {
        MODE_PLOT, MODE_GRAPH_Y, MODE_GRAPH_Z, MODE_CROSS_SECTION
    };
    enum TransformationType {
        TRANSFORM_DRAG, TRANSFORM_ROTATE, TRANSFORM_SCALE, TRANSFORM_SMEAR
    };
    enum LineType {
        POLYLINE, PIECEWISE
    };
    enum BlendType {
        CUSTOM, LINEAR, SINE, ELASTIC_IN, ELASTIC_OUT, ELASTIC_INOUT
    };
    enum ModelObjectType {
        LATHE, CROSS_SECTIONAL
    };
    enum TransformAxisLock {
        LOCK_NONE, LOCK_X, LOCK_Y
    };
    enum EditingTool {
        TOOL_BRUSH,
        TOOL_ERASE,
        TOOL_SMUDGE,
        TOOL_RECTANGLE,
        TOOL_ELLIPSE,
    };

    static std::vector<EditingTool> editingTools;
};


#endif //SENIORRESEARCH_ENUMS_H
