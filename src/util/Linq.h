//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_LINQ_H
#define SENIORRESEARCH_LINQ_H

#include <vector>
#include <functional>

#include "../util/Includes.h"

class Linq {
public:
    template<class T, class K>
    static std::vector<K> Select(const std::vector<T>& initVec, std::function<K(T)> func) {
        std::vector<K> newVec {};
        newVec.reserve(initVec.size());

        for (const T& val : initVec) {
            newVec.push_back(func(val));
        }
        return newVec;
    }
};


#endif //SENIORRESEARCH_LINQ_H
