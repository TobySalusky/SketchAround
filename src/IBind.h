//
// Created by Tobiathan on 9/15/21.
//

#ifndef TEST_IBIND_H
#define TEST_IBIND_H


class IBind
{
public:
    IBind() = default;
    virtual ~IBind() = default;
    virtual void Bind() = 0;    // "= 0" part makes this method pure virtual, and
    // also makes this class abstract.
    virtual void UnBind() = 0;
};


#endif //TEST_IBIND_H
