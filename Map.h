﻿/**
\file
\date 2015.05.26.
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

#ifndef __ETL_MAP_H__
#define __ETL_MAP_H__

#include "etlSupport.h"

#include "Base/Sorted.h"
#include "Base/MapItem.h"
#include "PoolAllocator.h"

#include <memory>
#include <utility>

namespace ETL_NAMESPACE {


template<typename K, class E, template<class> class A = std::allocator>
class Map : public Sorted<ListTemplate<MapItem<K, E>, A> > {

  public:   // types

    typedef MapItem<K, E> ItemType;
    typedef ListTemplate<ItemType, A> ContainerType;
    typedef typename ContainerType::Allocator Allocator;
    typedef Sorted<ContainerType> MapBase;
    typedef typename MapBase::Iterator Iterator;
    typedef typename MapBase::ConstIterator ConstIterator;
    typedef Matcher<ItemType> ItemMatcher;
    typedef typename ItemType::KeyMatcher KeyMatcher;
    typedef typename ItemType::ElementMatcher ElementMatcher;

  public:   // functions

    Map() {};

    Map(const Map& other) {
        copyElementsFrom(other);
    }

    Map& operator=(const Map& other) {
        MapBase::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Map(const std::initializer_list<std::pair<K, E>>& initList);

#endif

    inline std::pair<Iterator, bool> insert(const K& k, const E& e) {
        return MapBase::insertUnique(ItemType(k, e));
    }

    std::pair<Iterator, bool> insertOrAssign(const K& k, const E& e);

    void erase(const K& k);

    Iterator erase(Iterator pos) {
        return MapBase::erase(pos);
    }

    Iterator find(const K& k) const;

    Iterator find(const ItemMatcher& matchCall) const {
        return MapBase::find(matchCall);
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const ItemMatcher& matchCall) const {
        return MapBase::find(startPos, endPos, matchCall);
    }

    Iterator find(const KeyMatcher& matchCall) const {
        return MapBase::find(typename ItemType::KeyMatcherForwarder(matchCall));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const KeyMatcher& matchCall) const {
        return MapBase::find(startPos, endPos, typename ItemType::KeyMatcherForwarder(matchCall));
    }

    Iterator find(const ElementMatcher& matchCall) const {
        return MapBase::find(typename ItemType::ElementMatcherForwarder(matchCall));
    }

    Iterator find(ConstIterator startPos, ConstIterator endPos, const ElementMatcher& matchCall) const {
        return MapBase::find(startPos, endPos, typename ItemType::ElementMatcherForwarder(matchCall));
    }

    Iterator getItem(const K& k) const;

    E& getElement(const K& k) const {
        return getItem(k)->getElement();
    }

    void copyElementsFrom(const Map<K, E, A>& other);

    E& operator[](const K& k) {
        return Map<K, E, A>::getElement(k);
    }

    const E& operator[](const K& k) const {
        return Map<K, E, A>::getElement(k);
    }

#if ETL_USE_CPP11

    template<typename... Args>
    inline std::pair<Iterator, bool> emplace(const K& k, Args&& ... args);

#endif

};


template<typename K, class E, template<class> class A>
std::pair<typename Map<K, E, A>::Iterator, bool> Map<K, E, A>::insertOrAssign(const K& k, const E& e) {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if (found.second == false) {
#if ETL_USE_CPP11
        found.first = MapBase::emplaceTo(found.first, k, e);
#else
        found.first = MapBase::insertTo(found.first, ItemType(k, e));
#endif
    } else {
        --found.first;
        found.first->setElement(e);
    }

    found.second = !found.second;
    return found;
}


template<typename K, class E, template<class> class A>
void Map<K, E, A>::erase(const K& k) {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if (found.second == true) {
        MapBase::erase(--found.first);
    }
}


template<typename K, class E, template<class> class A>
typename Map<K, E, A>::Iterator  Map<K, E, A>::find(const K& k) const {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if (found.second == true) {
        return --found.first;
    } else {
        return MapBase::end();
    }
}


template<typename K, class E, template<class> class A>
typename Map<K, E, A>::Iterator Map<K, E, A>::getItem(const K& k) const {

    std::pair<Iterator, bool> found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if (found.second == false) {
#if ETL_USE_CPP11
        found.first = MapBase::emplaceTo(found.first, std::move(ItemType(k)));
#else
        found.first = MapBase::insertTo(found.first, ItemType(k));
#endif
    } else {
        --found.first;
    }

    return found.first;
}


template<typename K, class E, template<class> class A>
void Map<K, E, A>::copyElementsFrom(const Map<K, E, A>& other) {

    Iterator endIt = other.end();
    for (Iterator it = other.begin(); it != endIt; ++it) {
        insertOrAssign(it->getKey(), it->getElement());
    }
}

#if ETL_USE_CPP11

template<typename K, class E, template<class> class A>
Map<K, E, A>::Map(const std::initializer_list<std::pair<K, E>>& initList) {

    for (auto& item : initList) {
        insert(item.first, item.second);
    }
}


template<typename K, class E, template<class> class A>
template<typename... Args>
std::pair<typename Map<K, E, A>::Iterator, bool> Map<K, E, A>::emplace(const K& k, Args&& ... args) {

    auto found = MapBase::findSortedPosition(&ItemType::getKey, k);

    if (found.second == false) {
        found.first = MapBase::emplaceTo(found.first, k, std::forward<Args>(args)...);
    }

    found.second = !found.second;
    return found;
}

#endif


namespace Pooled {


template<class K, class E, uint32_t N>
class Map : public ETL_NAMESPACE::Map<K, E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> {

  public:   // types

    typedef ETL_NAMESPACE::Map<K, E, ETL_NAMESPACE::PoolHelper<N>::template Allocator> MapBase;
    typedef typename MapBase::Iterator Iterator;
    typedef typename MapBase::ConstIterator ConstIterator;

  public:   // functions

    Map() {};

    Map(const MapBase& other) :
        MapBase(other) {};

    Map& operator=(const MapBase& other) {
        MapBase::clear();
        copyElementsFrom(other);
        return *this;
    }

#if ETL_USE_CPP11

    Map(const std::initializer_list<std::pair<K, E>>& initList) :
        MapBase(initList) {};

#endif

};

}

}

#endif /* __ETL_MAP_H__ */
