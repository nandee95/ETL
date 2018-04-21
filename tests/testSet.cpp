/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2017 Balazs Toth.

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

#include "catch.hpp"

#include <ETL/Set.h>

#include "ContainerTester.h"
#include "DummyAllocator.h"

using ETL_NAMESPACE::Test::ContainerTester;
using ETL_NAMESPACE::Test::DummyAllocator;


TEST_CASE("Etl::Set<> basic test", "[set][etl][basic]") {

    typedef Etl::Set<ContainerTester> SetType;
    SetType set;

    REQUIRE(set.isEmpty());
    REQUIRE(set.getSize() == 0);

    ContainerTester a(4);
    set.insert(a);

    REQUIRE_FALSE(set.isEmpty());
    REQUIRE(set.getSize() == 1);
    SetType::Iterator it = set.begin();
    REQUIRE(*it == a);

    set.insert(ContainerTester(5));

    REQUIRE(set.getSize() == 2);
    ++it;
    REQUIRE(*it == ContainerTester(5));

    ContainerTester b(4);
    set.insert(b);

    REQUIRE(set.getSize() == 2);

    set.erase(ContainerTester(5));

    REQUIRE(set.getSize() == 1);
}


TEST_CASE("Etl::Set<> insert() test", "[set][etl]") {

    typedef Etl::Set<int> SetType;
    typedef std::pair<SetType::Iterator, bool> ResultType;

    SetType set;

    ResultType res = set.insert(1);

    REQUIRE(res.second == true);
    REQUIRE(res.first != set.end());
    REQUIRE(set.getSize() == 1);

    SECTION("first element") {

        REQUIRE(*(res.first) == 1);
    }

    SECTION("second element") {

        res = set.insert(2);

        REQUIRE(res.second == true);
        REQUIRE(set.getSize() == 2);
        REQUIRE(*(res.first) == 2);
    }

    SECTION("insert() of existing shall fail") {

        res = set.insert(1);

        REQUIRE(res.second == false);
        REQUIRE(set.getSize() == 1);
    }
}


TEST_CASE("Etl::Set<> erase tests", "[set][etl]") {

    typedef Etl::Set<int> SetType;

    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    CHECK(set.getSize() == 4);

    SECTION("erase(Element)") {

        set.erase(2);

        REQUIRE(set.getSize() == 3);
        REQUIRE(set.find(2) == set.end());
    }

    SECTION("erase(Iterator)") {

        SetType::Iterator it = set.find(2);
        CHECK(it != set.end());

        it = set.erase(it);

        REQUIRE(set.getSize() == 3);
        REQUIRE(set.find(2) == set.end());
        REQUIRE(it == set.find(3));
    }
}


TEST_CASE("Etl::Set<> iteration tests", "[set][etl]") {

    typedef Etl::Set<int> SetType;

    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    CHECK(set.getSize() == 4);

    SECTION("forward") {

        SetType::Iterator it = set.begin();

        REQUIRE(*it == 1);

        ++it;

        REQUIRE(*it == 2);
    }

    SECTION("backward") {

        SetType::Iterator it = set.end();

        --it;

        REQUIRE(*it == 4);

        --it;

        REQUIRE(*it == 3);
    }
}


TEST_CASE("Etl::Set<> element order", "[set][etl]") {

    typedef Etl::Set<int> SetType;

    SetType set;

    set.insert(3);
    set.insert(1);
    set.insert(2);
    set.insert(4);

    CHECK(set.getSize() == 4);

    SetType::Iterator it = set.begin();

    REQUIRE(*it == 1);

    ++it;
    REQUIRE(*it == 2);

    ++it;
    REQUIRE(*it == 3);

    ++it;
    REQUIRE(*it == 4);

    ++it;
    REQUIRE(it == set.end());
}


TEST_CASE("Etl::Set<> copy", "[set][etl]") {

    typedef Etl::Set<int> SetType;

    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    SetType set2;

    set2.insert(1);
    set2.insert(5);

    CHECK(set.getSize() == 4);
    CHECK(set2.getSize() == 2);

    SECTION("copy assignment") {

        set2 = set;

        REQUIRE(set2.getSize() == 4);
        REQUIRE(set2.find(1) != set2.end());
        REQUIRE(set2.find(4) != set2.end());
    }

    SECTION("copy constructor") {

        SetType set3 = set;

        REQUIRE(set3.getSize() == 4);
        REQUIRE(set3.find(1) != set3.end());
        REQUIRE(set3.find(4) != set3.end());
    }

    SECTION("copyElementsFrom()") {

        set2.copyElementsFrom(set);

        REQUIRE(set2.getSize() == 5);
        REQUIRE(set2.find(1) != set2.end());
        REQUIRE(set2.find(4) != set2.end());
        REQUIRE(set2.find(5) != set2.end());
    }

    SECTION("swap()") {

        set.swap(set2);

        REQUIRE(set2.getSize() == 4);
        REQUIRE(set.getSize() == 2);

        REQUIRE(set.find(1) != set.end());
        REQUIRE(set.find(5) != set.end());

        REQUIRE(set2.find(1) != set2.end());
        REQUIRE(set2.find(2) != set2.end());
        REQUIRE(set2.find(3) != set2.end());
        REQUIRE(set2.find(4) != set2.end());
    }
}


TEST_CASE("Etl::Set<> search tests", "[set][etl]") {

    typedef Etl::Set<int> SetType;
    SetType set;

    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);

    CHECK(set.getSize() == 4);

    SECTION("find(Key)") {

        SetType::Iterator it = set.find(3);

        REQUIRE(it != set.end());
        REQUIRE(*it == 3);
    }

    SECTION("find(Key) non-existing") {

        SetType::Iterator it = set.find(7);

        REQUIRE(it == set.end());
    }
}


TEST_CASE("Etl::Set<> allocator test", "[set][etl]") {

    typedef ContainerTester ItemType;
    typedef Etl::Set<ItemType, DummyAllocator> SetType;
    typedef SetType::Allocator AllocatorType;

    AllocatorType::reset();
    CHECK(AllocatorType::getAllocCount() == 0);
    CHECK(AllocatorType::getDeleteCount() == 0);

    SetType set;
    set.insert(ContainerTester(5));

    SetType::Iterator it = set.begin();
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(0)->item));

    set.insert(ContainerTester(6));
    ++it;
    REQUIRE(it.operator->() == &(AllocatorType::ptrOfAllocation(1)->item));

    CHECK(AllocatorType::getDeleteCount() == 0);

    set.erase(ContainerTester(5));
    REQUIRE(AllocatorType::getDeleteCount() == 1);
}


TEST_CASE("Etl::Pooled::Set<> test", "[set][etl]") {

    static const uint32_t NUM = 16;
    typedef ContainerTester ItemType;
    typedef Etl::Pooled::Set<ItemType, NUM> SetType;

    SetType set;

    SECTION("Basic allocation") {

        set.insert(ContainerTester(5));

        SetType::Iterator it = set.begin();
        REQUIRE(it.operator->() != NULL);

        set.insert(ContainerTester(6));
        SetType::Iterator it2 = it;
        ++it2;
        REQUIRE(it2.operator->() != NULL);
        REQUIRE(it2.operator->() != it.operator->());
    }

    SECTION("Allocate all") {

        for (uint32_t i = 0; i < NUM; ++i) {
            set.insert(ContainerTester(i));
        }

        CHECK(set.getSize() == NUM);

        std::pair<SetType::Iterator, bool> res = set.insert(ContainerTester(NUM));
        REQUIRE(set.getSize() == NUM);
        REQUIRE(res.first == set.end());
        REQUIRE(res.second == false);
    }
}


TEST_CASE("Etl::Set<> test cleanup", "[set][etl]") {

    typedef Etl::Set<ContainerTester, DummyAllocator> SetType;

    CHECK(ContainerTester::getObjectCount() == 0);
    CHECK(SetType::Allocator::getDeleteCount() == SetType::Allocator::getAllocCount());
}

