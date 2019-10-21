#*******************************************************************************
#*   (c) 2018 ZondaX GmbH
#*
#*  Licensed under the Apache License, Version 2.0 (the "License");
#*  you may not use this file except in compliance with the License.
#*  You may obtain a copy of the License at
#*
#*      http://www.apache.org/licenses/LICENSE-2.0
#*
#*  Unless required by applicable law or agreed to in writing, software
#*  distributed under the License is distributed on an "AS IS" BASIS,
#*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*  See the License for the specific language governing permissions and
#*  limitations under the License.
#********************************************************************************
cmake_minimum_required(VERSION 3.0)
project(ledger-cosmos VERSION 0.0.0)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(cmake/gtest)

string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address -fno-omit-frame-pointer")
string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fsanitize=address -fno-omit-frame-pointer")

#string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=undefined")
#string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fsanitize=undefined")

##############################################################
##############################################################
####### User App
##############################################################
##############################################################
# User static libs
file(GLOB_RECURSE USER_JSMN_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/deps/jsmn/src/jsmn.c
        )

file(GLOB_RECURSE USER_LIB_SRC
        src/ledger-user/src/lib/parser.c
        src/ledger-user/src/lib/parser_impl.c

        src/ledger-user/src/lib/json/json_parser.c
        src/ledger-user/src/lib/json/tx_parser.c
        src/ledger-user/src/lib/json/tx_display.c
        src/ledger-user/src/lib/json/tx_validate.c
        )

file(GLOB_RECURSE TESTS_USER_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/user/*.cpp)

file(GLOB_RECURSE FUZZING_USER_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/fuzzing/fuzzingMain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/user/util/common.cpp
        )

add_library(user_json_parser STATIC ${USER_LIB_SRC} ${USER_JSMN_SRC})
target_include_directories(user_json_parser PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/deps/jsmn/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/src/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/deps/ledger-zxlib/include
        )

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/json)

add_executable(test_user ${TESTS_USER_SRC})
target_include_directories(test_user PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/deps/jsmn/src
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
        )
target_link_libraries(test_user gtest_main user_json_parser nlohmann_json::nlohmann_json)
add_test(gtest ${PROJECT_BINARY_DIR}/test_user)

add_executable(fuzzing_stub ${FUZZING_USER_SRC})
target_include_directories(fuzzing_stub PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/deps/jsmn/src
        )
target_link_libraries(fuzzing_stub user_json_parser)


###############################################################
###############################################################
######## Validator App
###############################################################
###############################################################
file(GLOB_RECURSE VAL_LIB_SRC
        src/ledger-val/src/lib/vote.c
        src/ledger-val/src/lib/vote_fsm.c
        src/ledger-val/src/lib/vote_buffer.c
        src/ledger-val/src/lib/vote_parser.c
        src/ledger-val/deps/ledger-zxlib/src/buffering.c
        )
file(GLOB_RECURSE TESTS_VAL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tests/val/*.cpp)

add_library(val_lib STATIC ${VAL_LIB_SRC})
target_include_directories(val_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-val/deps/ledger-zxlib/include
        )

add_executable(test_val ${TESTS_VAL_SRC})
target_include_directories(test_val PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-val/src
        )
target_link_libraries(test_val gtest_main val_lib)

add_test(gtest ${PROJECT_BINARY_DIR}/test_val)

###############################################################
# Force tests to depend from app compiling
###############################################################

set(DISABLE_DOCKER_BUILDS OFF CACHE BOOL "Disables Docker Builds")

if (NOT DISABLE_DOCKER_BUILDS)
    add_custom_target(ledger_user_app
            COMMAND make build_cosmos
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
    add_dependencies(test_user ledger_user_app)
    add_custom_target(ledger_val_app
            COMMAND make build_tendermint
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
    add_dependencies(test_val ledger_val_app)
endif ()
