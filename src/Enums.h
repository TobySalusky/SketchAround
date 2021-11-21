//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_ENUMS_H
#define SENIORRESEARCH_ENUMS_H


class Enums {
public:
// Enums
    enum DrawMode {
        MODE_PLOT, MODE_GRAPH_Y, MODE_GRAPH_Z
    };
    enum TransformationType {
        TRANSFORM_DRAG, TRANSFORM_ROTATE, TRANSFORM_SCALE, TRANSFORM_SMEAR
    };
    enum LineType {
        POLYLINE, PIECEWISE
    };
    enum KeyframeBlendMode {
        LINEAR, SINE
    };
};


#endif //SENIORRESEARCH_ENUMS_H
