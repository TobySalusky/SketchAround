//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_UNDO_H
#define SENIORRESEARCH_UNDO_H

#include <utility>

#include "../generation/ModelObject.h"

class Undo {
public:

    explicit Undo(std::function<void()> func) {
        this->func = std::move(func);
    }

    void Apply();

private:
    std::function<void()> func;
};


#endif //SENIORRESEARCH_UNDO_H
