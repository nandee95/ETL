﻿/**
\file
\date 2016.01.02. 20:07:16
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
\endparblock
*/

#ifndef __ETL_AFIFOITERATOR_H__
#define __ETL_AFIFOITERATOR_H__

#include "etlSupport.h"

#include "Base/FifoIndexing.h"


namespace ETL_NAMESPACE {


class AFifoIterator {

// variables
protected:

    uint32_t ix;
    const FifoIndexing* fifo;

// functions
public:

    bool operator==(const AFifoIterator &other) const {
        return ((ix == other.ix) && (fifo == other.fifo));
    }

    inline bool operator!=(const AFifoIterator &other) const {
        return (!operator==(other));
    }

    AFifoIterator &operator++() {
        ++ix;
        return *this;
    }

    const AFifoIterator operator++(int) {
        AFifoIterator old = *this;
        this->operator++();
        return old;
    }

    AFifoIterator &operator--() {
        --ix;
        return *this;
    }

    const AFifoIterator operator--(int) {
        AFifoIterator old = *this;
        this->operator--();
        return old;
    }

protected:

    AFifoIterator(const AFifoIterator &other) :
        ix(other.ix),
        fifo(other.fifo) {};

    AFifoIterator &operator=(const AFifoIterator &other) {
        ix = other.ix;
        fifo = other.fifo;
        return *this;
    }

    AFifoIterator(const FifoIndexing* indexing, uint32_t index) :
        ix(index),
        fifo(indexing) {};

    const FifoIndexing* getFifoIndexing() const {
        return fifo;
    }

    uint32_t getIndexFromFront(uint32_t ix) const {
        return fifo->getIndexFromFront(ix);
    }

};


template<class T>
class FifoIterator : public AFifoIterator {

// variables
private:

    T* data;

// functions
public:

    FifoIterator<T>(const FifoIterator<T> &other) :
        AFifoIterator(other),
        data(other.data) {};

    FifoIterator<T> &operator=(const FifoIterator<T> &other) {
        AFifoIterator::operator=(other);
        data = other.data;
        return *this;
    };

    T &operator*() const {
        return *(get());
    };

    T* operator->() const {
        return get();
    };

protected:

    FifoIterator<T>(T* data, const FifoIndexing* indexing, uint32_t index) :
        AFifoIterator(indexing, index),
        data(data) {};

    inline T* get() const {
        return data + getIndexFromFront(ix);
    }

};

}

#endif /* __ETL_AFIFOITERATOR_H__ */

