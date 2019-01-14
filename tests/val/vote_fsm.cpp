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
#include "lib/vote_fsm.h"

namespace {

    TEST(VoteParserTest, NotInitialized) {
        vote_t v;
        vote_state_t s;

        EXPECT_EQ(0, s.isInitialized);
        EXPECT_EQ(0, try_state_transition(&v, &s));
    }

////////////

    TEST(VoteParserTest, SameHeightRound_SameType) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PROPOSAL;
        v.Height = 0;
        v.Round = 0;

        EXPECT_EQ(0, try_state_transition(&v, &s));
    }

    TEST(VoteParserTest, SameHeightRound_NextType) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PREVOTE;
        v.Height = 0;
        v.Round = 0;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

    TEST(VoteParserTest, SameHeightRound_NextType2) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PRECOMMIT;
        v.Height = 0;
        v.Round = 0;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

///////////

    TEST(VoteParserTest, NextRound_SameType) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PROPOSAL;
        v.Height = 0;
        v.Round = 1;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

    TEST(VoteParserTest, NextRound_NextType) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PREVOTE;
        v.Height = 0;
        v.Round = 1;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

    TEST(VoteParserTest, NextRound_NextType2) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PRECOMMIT;
        v.Height = 0;
        v.Round = 1;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

///////////

    TEST(VoteParserTest, NextHeight_SameType) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PROPOSAL;
        v.Height = 1;
        v.Round = 0;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

    TEST(VoteParserTest, NextHeight_NextType) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PREVOTE;
        v.Height = 1;
        v.Round = 0;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

    TEST(VoteParserTest, NextHeight_NextType2) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PROPOSAL;
        s.vote.Height = 0;
        s.vote.Round = 0;

        v.Type = TYPE_PRECOMMIT;
        v.Height = 1;
        v.Round = 0;

        EXPECT_EQ(1, try_state_transition(&v, &s));
    }

    TEST(VoteParserTest, DeviceTest1) {
        vote_t v;
        vote_state_t s;

        s.isInitialized = 1;
        s.vote.Type = TYPE_PREVOTE;
        s.vote.Height = 9;
        s.vote.Round = 0;

        v.Type = TYPE_PREVOTE;
        v.Height = 1;
        v.Round = 0;

        EXPECT_EQ(0, try_state_transition(&v, &s));
    }

}