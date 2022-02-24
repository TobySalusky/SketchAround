//
// Created by Tobiathan on 2/23/22.
//

#ifndef SENIORRESEARCH_MULTIUNDO_H
#define SENIORRESEARCH_MULTIUNDO_H

#include "Undo.h"

class MultiUndo : public Undo {
public:
    explicit MultiUndo(const std::vector<Undo*>& undos) {
        this->undos.reserve(undos.size());
        for (Undo* undoPtr : undos) {
            this->undos.emplace_back(UndoPtr(undoPtr));
        }
    }

    void Apply() override {
        for (const UndoPtr& undo : undos) {
            undo->Apply();
        }
    }
    std::vector<UndoPtr> undos;
};


#endif //SENIORRESEARCH_MULTIUNDO_H
