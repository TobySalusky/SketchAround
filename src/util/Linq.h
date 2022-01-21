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
    static std::vector<K> Select(std::vector<T>& initVec, std::function<K(T&)> func) {
        std::vector<K> newVec {};
        newVec.reserve(initVec.size());

        for (T& val : initVec) {
            newVec.push_back(func(val));
        }
        return newVec;
    }

    template<class T, class K>
    static std::vector<K> Select(const std::vector<T>& initVec, std::function<K(T)> func) {
        std::vector<K> newVec {};
        newVec.reserve(initVec.size());

        for (const T& val : initVec) {
            newVec.push_back(func(val));
        }
        return newVec;
    }

    template<class T, class K>
    static std::vector<K> Select(const std::vector<T>& initVec, std::function<K(T, int)> func) {
        std::vector<K> newVec {};
        newVec.reserve(initVec.size());

        for (int i = 0; i < initVec.size(); i++) {
            newVec.push_back(func(initVec[i], i));
        }
        return newVec;
    }

//    template<class T>
//    static std::vector<std::tuple<T, int>> Enumerate(std::vector<T> vec) {
//        Select<T, std::tuple<T, int>>(vec, [](T val, int i) {
//            return std::make_tuple(val, i);
//        });
//    }
};


#endif //SENIORRESEARCH_LINQ_H
