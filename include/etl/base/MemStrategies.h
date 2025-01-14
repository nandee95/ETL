/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2021 Balazs Toth.

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

#ifndef __ETL_MEMSTARTEGIES_H__
#define __ETL_MEMSTARTEGIES_H__

#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <limits>
#include <memory>

namespace ETL_NAMESPACE {


template<class C>
class AMemStrategy {

  public:  // types

    using value_type = typename C::value_type;
    using size_type = typename C::size_type;

  public:  // functions

    virtual ~AMemStrategy() = default;

    virtual size_type getMaxCapacity() const noexcept = 0;
    virtual void reserveExactly(C& cont, size_type length) = 0;
    virtual void reserve(C& cont, size_type length) = 0;
    virtual void shrinkToFit(C& cont) noexcept = 0;
    virtual void resize(C& cont, size_type length) = 0;
    virtual void resize(C& cont, size_type length, const value_type& ref) = 0;
    virtual void cleanup(C& cont) noexcept = 0;

    const virtual void* handle() const noexcept = 0;
};


/// Mem strategy with static size, allocated externally.
template<class C>
class StaticSized : public AMemStrategy<C> {

  public:  // types

    using value_type = typename C::value_type;
    using size_type = typename C::size_type;

  private:  // variables

    void* const data;
    const size_type capacity;

  public:  // functions

    // No defult constructor;

    StaticSized(void* d, size_type c) :
        data(d),
        capacity(c) {};

    size_type getMaxCapacity() const noexcept final {
        return capacity;
    }

    void reserveExactly(C& cont, size_type length) final {
        setupData(cont, length);
    }

    void reserve(C& cont, size_type length) final {
        setupData(cont, length);
    }

    void shrinkToFit(C& cont) noexcept final {
        setupData(cont, capacity);
    }

    void cleanup(C& cont) noexcept final {
        cont.clear();
    }

    void resize(C& cont, size_type length) final {
        resizeWithInserter(
            cont, length, [](typename C::iterator pos) { C::ops().placeDefaultTo(pos); });
    }

    void resize(C& cont, size_type length, const value_type& ref) final {
        resizeWithInserter(
            cont, length, [&ref](typename C::iterator pos) { C::ops().placeValueTo(pos, ref); });
    }

    const void* handle() const noexcept final {
        return data;
    }

  private:

    template<class INS>
    void resizeWithInserter(C& cont, size_type length, INS inserter);
    void setupData(C& cont, size_type length);
};


template<class C>
template<class INS>
void StaticSized<C>::resizeWithInserter(C& cont, size_type length, INS inserter) {

    using iterator = typename C::iterator;

    if (length <= capacity) {

        setupData(cont, length);

        if (length > cont.size()) {

            iterator newEnd = cont.data() + length;

            for (iterator it = cont.end(); it < newEnd; ++it) {
                inserter(it);
            }

        } else if (length < cont.size()) {

            iterator newEnd = cont.data() + length;
            C::ops().destruct(newEnd, cont.end());
        }

        cont.getProxy().setSize(length);
    }
}

template<class C>
void StaticSized<C>::setupData(C& cont, size_type length) {

    if (length <= capacity) {
        cont.getProxy().setData(data);
        cont.getProxy().setCapacity(capacity);
    } else {
        // TODO throw;
    }
}


/// Mem strategy with dynamic size, allocated with Allocator
template<class C, class A, bool UA = false>
class DynamicSized : public AMemStrategy<C> {

  public:  // types

    using value_type = typename C::value_type;
    using size_type = typename C::size_type;

    using Allocator = A;

    static constexpr bool UNIQUE_ALLOCATOR = UA;

  private:

    A allocator;

  public:  // functions

    size_type getMaxCapacity() const noexcept final {
        return std::numeric_limits<size_type>::max();
    }

    void reserveExactly(C& cont, size_type length) final;

    void reserve(C& cont, size_type length) final {
        auto cap = cont.capacity();
        if (length > cap) {
            auto goal = std::max(length, 2U * cap);
            reserveExactly(cont, getRoundedLength(goal));
        }
    }

    void shrinkToFit(C& cont) noexcept final;

    void resize(C& cont, size_type length) final {
        resizeWithInserter(
            cont, length, [](typename C::iterator pos) { C::ops().placeDefaultTo(pos); });
    }

    void resize(C& cont, size_type length, const value_type& ref) final {
        resizeWithInserter(
            cont, length, [&ref](typename C::iterator pos) { C::ops().placeValueTo(pos, ref); });
    }

    void cleanup(C& cont) noexcept final {
        cont.clear();
        deallocate(cont);
    }

    const void* handle() const noexcept final {
        return getHandle();
    }

  private:

    template<bool T = UNIQUE_ALLOCATOR>
    enable_if_t<T, const void*> getHandle() const noexcept {
        return &allocator;
    }

    template<bool T = UNIQUE_ALLOCATOR>
    enable_if_t<!T, const void*> getHandle() const noexcept {
        static const int h {};
        return &h;
    }

    template<class INS>
    void resizeWithInserter(C& cont, size_type length, INS inserter);

    void reallocateAndCopyFor(C& cont, size_type len);
    void allocate(C& cont, size_type len);

    void deallocate(C& cont) noexcept {
        allocator.deallocate(cont.data(), cont.capacity());
    }

    static size_type getRoundedLength(size_type length) noexcept {
        static constexpr size_type ROUND_STEP = 8;
        return (length + (ROUND_STEP - 1)) & ~(ROUND_STEP - 1);
    }
};


template<class C, class A, bool UA>
void DynamicSized<C, A, UA>::reserveExactly(C& cont, size_type length) {

    if (length > cont.capacity()) {
        reallocateAndCopyFor(cont, length);
    }
}


template<class C, class A, bool UA>
void DynamicSized<C, A, UA>::shrinkToFit(C& cont) noexcept {

    if (cont.capacity() > cont.size()) {
        reallocateAndCopyFor(cont, cont.size());
    }
}


template<class C, class A, bool UA>
template<class INS>
void DynamicSized<C, A, UA>::resizeWithInserter(C& cont, size_type length, INS inserter) {

    using iterator = typename C::iterator;

    if (length > cont.size()) {

        if (length > cont.capacity()) {
            reallocateAndCopyFor(cont, getRoundedLength(length));
        }

        iterator newEnd = cont.data() + length;

        for (iterator it = cont.end(); it < newEnd; ++it) {
            inserter(it);
        }

    } else if (length < cont.size()) {

        iterator newEnd = cont.data() + length;
        C::ops().destruct(newEnd, cont.end());
    }

    cont.getProxy().setSize(length);
}


template<class C, class A, bool UA>
void DynamicSized<C, A, UA>::reallocateAndCopyFor(C& cont, size_type len) {

    auto oldData = cont.data();
    auto oldEnd = cont.end();
    auto oldCapacity = cont.capacity();

    allocate(cont, len);

    if (oldData != nullptr) {

        size_type numToCopy = (len < cont.size()) ? len : cont.size();

        if ((cont.data() != nullptr) && (numToCopy > 0)) {

            auto dataAlias = cont.data();
            C::ops().moveWithPlacement(oldData, dataAlias, numToCopy);
        }

        C::ops().destruct(oldData, oldEnd);
        allocator.deallocate(oldData, oldCapacity);
    }
}


template<class C, class A, bool UA>
void DynamicSized<C, A, UA>::allocate(C& cont, size_type len) {

    if (len > 0) {
        cont.getProxy().setData(allocator.allocate(len));
    } else {
        cont.getProxy().setData(nullptr);
    }

    if (cont.data() != nullptr) {
        cont.getProxy().setCapacity(len);
    } else {
        cont.getProxy().setCapacity(0);
    }
}

}  // namespace ETL_NAMESPACE

#endif  // __ETL_MEMSTARTEGIES_H__
