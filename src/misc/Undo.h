//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_UNDO_H
#define SENIORRESEARCH_UNDO_H

#include <utility>
#include <vector>



class Undo {
public:
    virtual void Apply() = 0;

    virtual ~Undo() = default;
};

#endif //SENIORRESEARCH_UNDO_H
