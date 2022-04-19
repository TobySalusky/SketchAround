//
// Created by Tobiathan on 2/23/22.
//

#ifndef SENIORRESEARCH_MULTIUNDO_H
#define SENIORRESEARCH_MULTIUNDO_H

#include "../Undo.h"

class MultiUndo : public Undo {
public:
    explicit MultiUndo(const std::vector<Undo*>& undos) {
        this->undos.reserve(undos.size());
        for (Undo* undoPtr : undos) {
            this->undos.push_back(std::unique_ptr<Undo>(undoPtr));
        }
    }

    void Apply() override {
        for (const auto& undo : undos) {
            undo->Apply();
        }
    }

private:
    std::vector<std::unique_ptr<Undo>> undos {};
};


#endif //SENIORRESEARCH_MULTIUNDO_H
