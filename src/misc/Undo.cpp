//
// Created by Tobiathan on 11/7/21.
//

#include "Undo.h"

void Undo::Apply(Undo::State state) {
    funcPtr(state);
}
