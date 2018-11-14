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

#include "gtest/gtest.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <string>
#include "lib/vote_parser.h"

namespace {

// https://github.com/tendermint/tendermint/blob/develop/docs/spec/blockchain/encoding.md#signed-messages
// https://github.com/tendermint/tendermint/blob/master/types/vote_test.go

TEST(VoteParserTest, Empty) {
    std::vector<uint8_t> vote_data{};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_unexpected_buffer_end, error);
}

TEST(VoteParserTest, Invalid) {
    std::vector<uint8_t> vote_data{0xFF, 0xFF};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_unexpected_type_value, error);
}

TEST(VoteParserTest, AllDefaults) {
    std::vector<uint8_t> vote_data{0x22, 0xb, 0x8, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff, 0xff, 0x1};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_unexpected_type_value, error);
}

TEST(VoteParserTest, DefaultData) {
    std::vector<uint8_t> vote_data{
        0x8, 0x1,
        0x22, 0xb, 0x8, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff, 0xff, 0x1};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_ok, error);

    EXPECT_EQ(1, vote.Type) << "Type not parsed correctly";
    EXPECT_EQ(0, vote.Round) << "Round not parsed correctly";
    EXPECT_EQ(0, vote.Height) << "Height not parsed correctly";
}

TEST(VoteParserTest, BasicPreVote) {
    std::vector<uint8_t> vote_data{
        0x8,                                    // (field_number << 3) | wire_type
        0x1,                                    // PrevoteType
        0x11,                                   // (field_number << 3) | wire_type
        0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // height
        0x19,                                   // (field_number << 3) | wire_type
        0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // round
        0x22, // (field_number << 3) | wire_type
        // remaining fields (timestamp):
        0xb, 0x8, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff, 0xff, 0x1};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_ok, error);

    EXPECT_EQ(1, vote.Type) << "Type not parsed correctly";
    EXPECT_EQ(1, vote.Height) << "Round not parsed correctly";
    EXPECT_EQ(1, vote.Round) << "Height not parsed correctly";
}

TEST(VoteParserTest, BasicPrecommit) {
    std::vector<uint8_t> vote_data{
        0x8,                                    // (field_number << 3) | wire_type
        0x2,                                    // PrecommitType
        0x11,                                   // (field_number << 3) | wire_type
        0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // height
        0x19,                                   // (field_number << 3) | wire_type
        0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // round
        0x22, // (field_number << 3) | wire_type
        // remaining fields (timestamp):
        0xb, 0x8, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff, 0xff, 0x1};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_ok, error);

    EXPECT_EQ(2, vote.Type) << "Type not parsed correctly";
    EXPECT_EQ(5, vote.Height) << "Round not parsed correctly";
    EXPECT_EQ(7, vote.Round) << "Height not parsed correctly";
}

TEST(VoteParserTest, MissingRound) {
    std::vector<uint8_t> vote_data{
        0x8,                                    // (field_number << 3) | wire_type
        0x2,                                    // PrecommitType
        0x11,                                   // (field_number << 3) | wire_type
        0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // height
        0x22, // (field_number << 3) | wire_type
        // remaining fields (timestamp):
        0xb, 0x8, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff, 0xff, 0x1};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_ok, error);

    EXPECT_EQ(2, vote.Type) << "Type not parsed correctly";
    EXPECT_EQ(5, vote.Height) << "Round not parsed correctly";
    EXPECT_EQ(0, vote.Round) << "Height not parsed correctly";
}

TEST(VoteParserTest, MissingHeight) {
    std::vector<uint8_t> vote_data{
        0x8,                                    // (field_number << 3) | wire_type
        0x2,                                    // PrecommitType
        0x19,                                   // (field_number << 3) | wire_type
        0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // round
        0x22, // (field_number << 3) | wire_type
        // remaining fields (timestamp):
        0xb, 0x8, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff, 0xff, 0x1};

    vote_t vote;
    parse_error_t error = vote_amino_parse(vote_data.data(),
                                           vote_data.size(),
                                           &vote);

    EXPECT_EQ(parse_ok, error);

    EXPECT_EQ(2, vote.Type) << "Type not parsed correctly";
    EXPECT_EQ(0, vote.Height) << "Round not parsed correctly";
    EXPECT_EQ(7, vote.Round) << "Height not parsed correctly";
}

}