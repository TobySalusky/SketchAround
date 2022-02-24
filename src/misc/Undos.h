//
// Created by Tobiathan on 11/23/21.
//

#ifndef SENIORRESEARCH_UNDOS_H
#define SENIORRESEARCH_UNDOS_H

#include <vector>
#include "Undo.h"

class Undos {
public:
    static void Add(Undo* undo) {
        undos.emplace_back(undo);
    }

    static void UseLast() {
        if (undos.empty()) return;
        printf("using undo...\n");
        undos.back()->Apply();
        undos.pop_back();
    }
private:
    static std::vector<UndoPtr> undos;
};

#endif //SENIORRESEARCH_UNDOS_H
