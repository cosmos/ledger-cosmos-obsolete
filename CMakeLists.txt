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
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
add_subdirectory(cmake/gtest)

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
        src/ledger-user/src/lib/json_parser.c
        src/ledger-user/src/lib/transaction_parser.c
        )
file(GLOB_RECURSE TESTS_USER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tests/user/*.cpp)

add_library(user_json_parser STATIC ${USER_LIB_SRC} ${USER_JSMN_SRC})
target_include_directories(user_json_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/deps/jsmn/src)

add_executable(test_user ${TESTS_USER_SRC})
target_include_directories(test_user PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger-user/deps/jsmn/src
)
target_link_libraries(test_user gtest_main user_json_parser)

add_test(gtest ${PROJECT_BINARY_DIR}/test_user)

###############################################################
###############################################################
######## Validator App
###############################################################
###############################################################
file(GLOB_RECURSE VAL_LIB_SRC
        src/ledger-val/src/lib/vote_parser.c
        src/ledger-val/src/lib/vote_fsm.c
        src/ledger-val/src/lib/vote.c
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

set(DISABLE_DOCKER_BUILDS OFF  CACHE BOOL "Disables Docker Builds")

if (NOT DISABLE_DOCKER_BUILDS)
    add_custom_target(ledger_val_app
            COMMAND make build_tendermint
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_target(ledger_user_app
            COMMAND make build_cosmos
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )

    add_dependencies(test_val ledger_val_app)
    add_dependencies(test_user ledger_user_app)
endif()
