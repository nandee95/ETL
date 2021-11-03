# tests for libetl

if(NOT DEFINED ETL_TESTS_DIR)
    set(ETL_TESTS_DIR ${CMAKE_CURRENT_LIST_DIR}/tests)
endif()

list(APPEND ETL_TEST_CMN ${ETL_TESTS_DIR}/ContainerTester.cpp)
list(APPEND ETL_TEST_CMN ${ETL_TESTS_DIR}/testMain.cpp)

list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testArray.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testVector.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testList.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testSet.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMap.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testMultiMap.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testFifo.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testPool.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testBufStr.cpp)
list(APPEND ETL_TEST_SRCS ${ETL_TESTS_DIR}/testSpan.cpp)

add_executable(testEtl ${ETL_TEST_SRCS} ${ETL_TEST_CMN})
target_link_libraries(testEtl PRIVATE libetl)
target_include_directories(testEtl PRIVATE ${ETL_TESTS_DIR})

add_executable(etlInfo ${ETL_TESTS_DIR}/info.cpp ${ETL_TEST_CMN})
target_link_libraries(etlInfo PRIVATE libetl)
target_include_directories(etlInfo PRIVATE ${ETL_TESTS_DIR})