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

    TEST(VoteParserTest, ReadVarints) {
        size_t value;
        uint32_t pos = 0;

        std::vector<uint8_t> varint_data{
                0x34,
                0xac, 0x02,
                0xee, 0xe7, 0x82, 0x83, 0x0b,
                0x80};

        auto err = get_varint(varint_data.data(), varint_data.size(), &value, pos, &pos);
        EXPECT_EQ(0x34, value);
        EXPECT_EQ(1, pos);
        EXPECT_EQ(parse_ok, err);

        err = get_varint(varint_data.data(), varint_data.size(), &value, pos, &pos);
        EXPECT_EQ(parse_ok, err);
        EXPECT_EQ(300, value);
        EXPECT_EQ(3, pos);

        err = get_varint(varint_data.data(), varint_data.size(), &value, pos, &pos);
        EXPECT_EQ(parse_ok, err);
        EXPECT_EQ(2959127534, value);
        EXPECT_EQ(8, pos);

        err = get_varint(varint_data.data(), varint_data.size(), &value, pos, &pos);
        EXPECT_EQ(parse_unexpected_buffer_end, err);
        EXPECT_EQ(0, value);
        EXPECT_EQ(9, pos);
    }


    TEST(VoteParserTest, Empty) {
        std::vector<uint8_t> vote_data{};

        vote_t vote;
        parse_error_t error = vote_amino_parse(vote_data.data(),
                                               vote_data.size(),
                                               &vote);

        EXPECT_EQ(parse_unexpected_buffer_end, error);
    }

    TEST(VoteParserTest, Invalid) {
        std::vector<uint8_t> vote_data{0x5F, 0xFF};

        vote_t vote;
        parse_error_t error = vote_amino_parse(vote_data.data(),
                                               vote_data.size(),
                                               &vote);

        EXPECT_EQ(parse_unexpected_buffer_size, error);
    }

    TEST(VoteParserTest, AllDefaults) {
        std::vector<uint8_t> vote_data{
                0x0d,
                0x22, 0xb, 0x8, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff, 0xff, 0x1};

        vote_t vote;
        parse_error_t error = vote_amino_parse(vote_data.data(),
                                               vote_data.size(),
                                               &vote);

        EXPECT_EQ(parse_unexpected_type_value, error);
    }

    TEST(VoteParserTest, DefaultData) {
        std::vector<uint8_t> vote_data{
                0x0F,
                0x8, TYPE_PREVOTE,
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
                0x21,
                0x8, TYPE_PREVOTE,                      // (field_number << 3) | wire_type + PrevoteType
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
        EXPECT_EQ(TYPE_PREVOTE, vote.Type) << "Type not parsed correctly";
        EXPECT_EQ(1, vote.Height) << "Round not parsed correctly";
        EXPECT_EQ(1, vote.Round) << "Height not parsed correctly";
    }

    TEST(VoteParserTest, BasicPrecommit) {
        std::vector<uint8_t> vote_data{
                0x21,
                0x8, TYPE_PRECOMMIT,                    // (field_number << 3) | wire_type + PrecommitType
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
        EXPECT_EQ(TYPE_PRECOMMIT, vote.Type) << "Type not parsed correctly";
        EXPECT_EQ(5, vote.Height) << "Round not parsed correctly";
        EXPECT_EQ(7, vote.Round) << "Height not parsed correctly";
    }

    TEST(VoteParserTest, MissingRound) {
        std::vector<uint8_t> vote_data{
                0x18,
                0x8, TYPE_PRECOMMIT,                    // (field_number << 3) | wire_type + PrecommitType
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
        EXPECT_EQ(TYPE_PRECOMMIT, vote.Type) << "Type not parsed correctly";
        EXPECT_EQ(5, vote.Height) << "Round not parsed correctly";
        EXPECT_EQ(0, vote.Round) << "Height not parsed correctly";
    }

    TEST(VoteParserTest, MissingHeight) {
        std::vector<uint8_t> vote_data{
                0x18,
                0x8, TYPE_PRECOMMIT,                    // (field_number << 3) | wire_type + PrecommitType
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
        EXPECT_EQ(TYPE_PRECOMMIT, vote.Type) << "Type not parsed correctly";
        EXPECT_EQ(0, vote.Height) << "Round not parsed correctly";
        EXPECT_EQ(7, vote.Round) << "Height not parsed correctly";
    }

    TEST(VoteParserTest, RealMessageFromSignatory) {
        std::vector<uint8_t> vote_data{116, 8, 32, 17, 1, 0, 0, 0, 0, 0, 0, 0, 33, 255, 255, 255, 255, 255, 255, 255,
                                       255, 42, 72, 10, 32, 130, 250, 74, 141, 138, 59, 64, 89, 2, 14, 37, 169, 26, 68,
                                       218, 149, 185, 25, 233, 110, 99, 175, 117, 39, 218, 42, 6, 66, 115, 118, 248,
                                       131, 18, 36, 10, 32, 35, 52, 252, 117, 251, 228, 106, 244, 94, 202, 53, 155, 96,
                                       99, 0, 168, 21, 197, 255, 187, 17, 129, 117, 111, 124, 207, 121, 29, 101, 96, 55,
                                       74, 16, 1, 50, 11, 8, 164, 152, 150, 227, 5, 16, 167, 135, 203, 41, 58, 7, 116,
                                       101, 115, 116, 110, 101, 116};

        vote_t vote;
        parse_error_t error = vote_amino_parse(vote_data.data(),
                                               vote_data.size(),
                                               &vote);

        EXPECT_EQ(parse_ok, error);
        EXPECT_EQ(TYPE_PROPOSAL, vote.Type) << "Type not parsed correctly";
        EXPECT_EQ(1, vote.Height) << "Round not parsed correctly";
        EXPECT_EQ(0, vote.Round) << "Height not parsed correctly";
    }

}