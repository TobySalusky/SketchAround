//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_UNDO_H
#define SENIORRESEARCH_UNDO_H

#include <utility>
#include <vector>
#include "UndoersInfo.h"



class Undo {
public:
    virtual void Apply() = 0;

    virtual ~Undo() = default;

    static void InitializeUndoers(const UndoersInfo& info);
};

typedef std::unique_ptr<Undo> UndoPtr;

#endif //SENIORRESEARCH_UNDO_H
