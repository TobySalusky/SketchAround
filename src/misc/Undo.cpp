//
// Created by Tobiathan on 11/7/21.
//

#include "Undo.h"
#include "UndoTypes/LineStateUndo.h"

void Undo::InitializeUndoers(const UndoersInfo& info) {
    LineStateUndo::Initialize(info);
}