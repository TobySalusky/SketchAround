//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_UNDO_H
#define SENIORRESEARCH_UNDO_H

#include <vector>
#include "../generation/ModelObject.h"

class Undo {
public:
    struct State {
        std::vector<ModelObject*>& modelObjects;
    };

    explicit Undo(void (*funcPtr)(State)) {
        this->funcPtr = funcPtr;
    }

    void Apply(State state);

private:
    void (*funcPtr)(State);
};


#endif //SENIORRESEARCH_UNDO_H
