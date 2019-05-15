/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019 Balazs Toth.

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

#include <etl/Span.h>

#include "ContainerTester.h"

using ETL_NAMESPACE::Test::ContainerTester;

namespace {

constexpr char C_ARRAY[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
constexpr std::size_t SIZE = sizeof(C_ARRAY);

constexpr std::initializer_list<char> INIT_LIST = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
constexpr std::array<char, SIZE> ARRAY = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

}


TEST_CASE("Etl::Span<> basic test", "[span][etl][basic]") {

    using Etl::Span;

    Span<const char> span(C_ARRAY);

    static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");

    REQUIRE_FALSE(span.empty());
    REQUIRE(span.size() == SIZE);

    REQUIRE(span.front() == '0');
    REQUIRE(span.back() == '9');
    REQUIRE(*(span.begin() + 2) == '2');
    REQUIRE(span[4] == '4');
    REQUIRE(span[7] == '7');
}


TEST_CASE("Etl::Span<> element constness tests", "[span][etl][basic]") {

    using Etl::Span;

    SECTION("Span<T> from T") {

        std::array<char, SIZE> data = ARRAY;
        Span<char, SIZE> span(data);

        REQUIRE(span[6] == '6');
    }

    SECTION("Span<const T> from T") {

        std::array<char, SIZE> data = ARRAY;
        Span<const char, SIZE> span(data);

        REQUIRE(span[6] == '6');
    }

    SECTION("Span<const T> from const T") {

        const std::array<char, SIZE> data = ARRAY;
        Span<const char, SIZE> span(data);

        REQUIRE(span[6] == '6');
    }
}


TEST_CASE("Etl::Span<> constexpr tests", "[span][etl][basic]") {

    using Etl::Span;

    SECTION("Span<>()") {

        constexpr Etl::Span<char> span;

        static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");
        static_assert(span.size() == 0U, "Span<>: Invalid size()");
        static_assert(span.data() == nullptr, "Span<>: Invalid data()");
        static_assert(span.begin() == span.end(), "Span<>: Invalid iterators");

        CHECK(true);
    }

    SECTION("Span<>(ptr, count)") {

        constexpr Etl::Span<const char> span(&C_ARRAY[0], SIZE);

        static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");

        static_assert(span.size() == SIZE, "Span<>: Invalid size()");
        static_assert(span.data() != nullptr, "Span<>: Invalid data()");

        static_assert(span.front() == '0', "Span<>: Invalid front()");
        static_assert(span.back() == '9', "Span<>: Invalid back()");

        static_assert(span.begin() != span.end(), "Span<>: Invalid iterators");
        static_assert(*span.begin() == '0', "Span<>: Invalid begin()");
        static_assert(*(span.end() - 1) == '9', "Span<>: Invalid end()");

        CHECK(true);
    }


    SECTION("Span<>(ptr, ptr)") {

        constexpr Etl::Span<const char> span(&C_ARRAY[0], &C_ARRAY[0] + SIZE);

        static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");

        static_assert(span.size() == SIZE, "Span<>: Invalid size()");
        static_assert(span.data() != nullptr, "Span<>: Invalid data()");

        static_assert(span.front() == '0', "Span<>: Invalid front()");
        static_assert(span.back() == '9', "Span<>: Invalid back()");

        static_assert(span.begin() != span.end(), "Span<>: Invalid iterators");
        static_assert(*span.begin() == '0', "Span<>: Invalid begin()");
        static_assert(*(span.end() - 1) == '9', "Span<>: Invalid end()");

        CHECK(true);
    }

    SECTION("Span<>(\"\")") {

        constexpr Etl::Span<const char> span(C_ARRAY);

        static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");

        static_assert(span.size() == SIZE, "Span<>: Invalid size()");
        static_assert(span.data() != nullptr, "Span<>: Invalid data()");

        static_assert(span.front() == '0', "Span<>: Invalid front()");
        static_assert(span.back() == '9', "Span<>: Invalid back()");

        static_assert(span.begin() != span.end(), "Span<>: Invalid iterators");
        static_assert(*span.begin() == '0', "Span<>: Invalid begin()");
        static_assert(*(span.end() - 1) == '9', "Span<>: Invalid end()");

        CHECK(true);
    }

    SECTION("Span<>(std::array<>)") {

        constexpr Etl::Span<const char> span(ARRAY);

        static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");

        static_assert(span.size() == SIZE, "Span<>: Invalid size()");
        static_assert(span.data() != nullptr, "Span<>: Invalid data()");

        static_assert(span.front() == '0', "Span<>: Invalid front()");
        static_assert(span.back() == '9', "Span<>: Invalid back()");

        static_assert(span.begin() != span.end(), "Span<>: Invalid iterators");
        static_assert(*span.begin() == '0', "Span<>: Invalid begin()");
        static_assert(*(span.end() - 1) == '9', "Span<>: Invalid end()");

        CHECK(true);
    }

    SECTION("Span<>(Span<>)") {

        constexpr Etl::Span<const char> spanRef(ARRAY);
        constexpr Etl::Span<const char> span(spanRef);

        static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");

        static_assert(span.size() == SIZE, "Span<>: Invalid size()");
        static_assert(span.data() != nullptr, "Span<>: Invalid data()");

        static_assert(span.front() == '0', "Span<>: Invalid front()");
        static_assert(span.back() == '9', "Span<>: Invalid back()");

        static_assert(span.begin() != span.end(), "Span<>: Invalid iterators");
        static_assert(*span.begin() == '0', "Span<>: Invalid begin()");
        static_assert(*(span.end() - 1) == '9', "Span<>: Invalid end()");

        CHECK(true);
    }

    SECTION("constexpr Span<> features") {

        constexpr Etl::Span<const char> span(ARRAY);

        static_assert(span.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");

        static_assert(span.size() == SIZE, "Span<>: Invalid size()");
        static_assert(span.data() != nullptr, "Span<>: Invalid data()");

        static_assert(span.front() == '0', "Span<>: Invalid front()");
        static_assert(span.back() == '9', "Span<>: Invalid back()");

        static_assert(span.begin() != span.end(), "Span<>: Invalid iterators");
        static_assert(*span.begin() == '0', "Span<>: Invalid begin()");
        static_assert(*(span.end() - 1) == '9', "Span<>: Invalid end()");

        SECTION("first<>()") {

            constexpr auto firstSub = span.first<6>();

            static_assert(firstSub.extent == 6, "Span<>: static extent '6' expected");
            static_assert(firstSub.size() == 6, "Span<>: Invalid size()");
            static_assert(firstSub.data() != nullptr, "Span<>: Invalid data()");
            static_assert(firstSub.front() == '0', "Span<>: Invalid front()");

            CHECK(true);
        }

        SECTION("first()") {

            constexpr auto firstSub = span.first(6);

            static_assert(firstSub.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");
            static_assert(firstSub.size() == 6, "Span<>: Invalid size()");
            static_assert(firstSub.data() != nullptr, "Span<>: Invalid data()");
            static_assert(firstSub.front() == '0', "Span<>: Invalid front()");

            CHECK(true);
        }

        SECTION("last<>()") {

            constexpr auto lastSub = span.last<6>();

            static_assert(lastSub.extent == 6, "Span<>: static extent '6' expected");
            static_assert(lastSub.size() == 6, "Span<>: Invalid size()");
            static_assert(lastSub.data() != nullptr, "Span<>: Invalid data()");
            static_assert(lastSub.front() == '4', "Span<>: Invalid front()");

            CHECK(true);
        }

        SECTION("last()") {

            constexpr auto lastSub = span.last(6);

            static_assert(lastSub.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");
            static_assert(lastSub.size() == 6, "Span<>: Invalid size()");
            static_assert(lastSub.data() != nullptr, "Span<>: Invalid data()");
            static_assert(lastSub.front() == '4', "Span<>: Invalid front()");

            CHECK(true);
        }

        SECTION("subspan<>()") {

            constexpr auto sub1 = span.subspan<3, 5>();

            static_assert(sub1.extent == 5, "Span<>: static extent '5' expected");
            static_assert(sub1.size() == 5, "Span<>: Invalid size()");
            static_assert(sub1.data() != nullptr, "Span<>: Invalid data()");
            static_assert(sub1.front() == '3', "Span<>: Invalid front()");
            static_assert(sub1.back() == '7', "Span<>: Invalid back()");

            constexpr auto sub2 = span.subspan<3>();

            static_assert(sub2.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");
            static_assert(sub2.size() == 7, "Span<>: Invalid size()");
            static_assert(sub2.data() != nullptr, "Span<>: Invalid data()");
            static_assert(sub2.front() == '3', "Span<>: Invalid front()");
            static_assert(sub2.back() == '9', "Span<>: Invalid back()");

            CHECK(true);
        }

        SECTION("subspan()") {

            constexpr auto sub = span.subspan(3, 5);

            static_assert(sub.extent == Etl::dynamic_extent, "Span<>: dynamic_extent expected");
            static_assert(sub.size() == 5, "Span<>: Invalid size()");
            static_assert(sub.data() != nullptr, "Span<>: Invalid data()");
            static_assert(sub.front() == '3', "Span<>: Invalid front()");
            static_assert(sub.back() == '7', "Span<>: Invalid back()");

            CHECK(true);
        }
    }
}
