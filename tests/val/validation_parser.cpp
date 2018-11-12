/*******************************************************************************
*   (c) 2018 ZondaX GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "json_parser.h"
#include "validation_parser.h"

#include "gtest/gtest.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <jsmn.h>
#include <string>

namespace {

TEST(ValidationParserTest, ParseRoundMin) {

    const char* transaction = R"({"height":0,"other":{"Hello world"},"round":0})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int8_t round = validation_parser_get_msg_round(&parsed_json, transaction, NULL);
    EXPECT_EQ(round, 0) << "Round not parsed correctly";
}

TEST(ValidationParserTest, ParseHeightMin) {

    const char* transaction = R"({"height":0,"other":{"Hello world"},"round":100})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int64_t height = validation_parser_get_msg_height(&parsed_json, transaction, NULL);
    EXPECT_EQ(height, 0) << "Height not parsed correctly";
}

TEST(ValidationParserTest, ParseRoundDecimal) {
    const char* transaction = R"({"height":200,"other":{"Hello world"},"round":100})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int8_t round = validation_parser_get_msg_round(&parsed_json, transaction, NULL);
    EXPECT_EQ(round, 100) << "Round not parsed correctly";
}

TEST(ValidationParserTest, ParseHeightDecimal) {
    const char* transaction = R"({"height":200,"other":{"Hello world"},"round":100})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int64_t height = validation_parser_get_msg_height(&parsed_json, transaction, NULL);
    EXPECT_EQ(height, 200) << "Height not parsed correctly";
}

TEST(ValidationParserTest, ParseRoundMax) {

    const char *transaction = R"({"round":127,"height":200,"other":{"Hello world"}})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int8_t round = validation_parser_get_msg_round(&parsed_json, transaction, NULL);
    EXPECT_EQ(round, std::numeric_limits<int8_t>::max()) << "Round not parsed correctly";
}

TEST(ValidationParserTest, ParseHeightMax) {

    const char* transaction = R"({"round":100,"height":9223372036854775807,"other":{"Hello world"}})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int64_t height = validation_parser_get_msg_height(&parsed_json, transaction, NULL);
    EXPECT_EQ(height, std::numeric_limits<int64_t>::max()) << "Height not parsed correctly";
}

TEST(ValidationParserTest, Parse_NegativeHeight) {

    const char* transaction = R"({"height":-200,"other":{"Hello world"},"round":100})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int64_t height = validation_parser_get_msg_height(&parsed_json, transaction, NULL);
    EXPECT_EQ(-200, height) << "Height not parsed correctly";
}

TEST(ValidationParserTest, Parse_NegativeRound) {

    const char* transaction = R"({"round":-100,"height":200,"other":{"Hello world"}})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    int8_t round = validation_parser_get_msg_round(&parsed_json, transaction, NULL);
    EXPECT_EQ(-100, round) << "Round not parsed correctly";
}

TEST(ValidationParserTest, ValidateJson_HexHeight) {

    const char* transaction = R"({"height":0xFF,"other":{"Hello world"},"round":100})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    const char* error = json_validate(&parsed_json, transaction);
    EXPECT_STREQ("Could not parse height", error) << "Json validation not working correctly";
}

TEST(ValidationParserTest, ValidateJson_HexRound) {

    const char* transaction = R"({"height":200,"other":{"Hello world"},"round":0xAA})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    const char* error = json_validate(&parsed_json, transaction);
    EXPECT_STREQ("Could not parse round", error) << "Json validation not working correctly";
}

TEST(ValidationParserTest, ValidateJson_Correct) {

    const char* transaction = R"({"height":200,"other":{"Hello world"},"round":100})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    const char* error = json_validate(&parsed_json, transaction);
    EXPECT_EQ(NULL, error) << "Json validation not working correctly";
}

TEST(ValidationParserTest, ValidateJson_NotSorted) {

    const char* transaction = R"({"height":200,"round":100,"other":{"Hello world"}})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    const char* error = json_validate(&parsed_json, transaction);
    EXPECT_STREQ("Dictionaries are not sorted", error) << "Json validation should fail";
}

TEST(ValidationParserTest, ValidateJson_Whitespaces) {

    const char* transaction = R"({"height": 200,"other":{"Hello world"},"round": 100})";
    parsed_json_t parsed_json;
    json_parse(&parsed_json, transaction);
    const char* error = json_validate(&parsed_json, transaction);
    EXPECT_STREQ("Contains whitespace in the corpus", error) << "Json validation should fail";
}
}