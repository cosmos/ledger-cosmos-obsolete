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
        vote_reset();
        vote_state_reset();

        EXPECT_EQ(0, vote_state.isInitialized);
        EXPECT_EQ(0, try_state_transition());
    }

////////////

    TEST(VoteParserTest, SameHeightRound_SameType) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PROPOSAL;
        vote.Height = 0;
        vote.Round = 0;

        EXPECT_EQ(0, try_state_transition());
    }

    TEST(VoteParserTest, SameHeightRound_NextType) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PREVOTE;
        vote.Height = 0;
        vote.Round = 0;

        EXPECT_EQ(1, try_state_transition());
    }

    TEST(VoteParserTest, SameHeightRound_NextType2) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PRECOMMIT;
        vote.Height = 0;
        vote.Round = 0;

        EXPECT_EQ(1, try_state_transition());
    }

///////////

    TEST(VoteParserTest, NextRound_SameType) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PROPOSAL;
        vote.Height = 0;
        vote.Round = 1;

        EXPECT_EQ(1, try_state_transition());
    }

    TEST(VoteParserTest, NextRound_NextType) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PREVOTE;
        vote.Height = 0;
        vote.Round = 1;

        EXPECT_EQ(1, try_state_transition());
    }

    TEST(VoteParserTest, NextRound_NextType2) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PRECOMMIT;
        vote.Height = 0;
        vote.Round = 1;

        EXPECT_EQ(1, try_state_transition());
    }

///////////

    TEST(VoteParserTest, NextHeight_SameType) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PROPOSAL;
        vote.Height = 1;
        vote.Round = 0;

        EXPECT_EQ(1, try_state_transition());
    }

    TEST(VoteParserTest, NextHeight_NextType) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PREVOTE;
        vote.Height = 1;
        vote.Round = 0;

        EXPECT_EQ(1, try_state_transition());
    }

    TEST(VoteParserTest, NextHeight_NextType2) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PROPOSAL;
        vote_state.vote.Height = 0;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PRECOMMIT;
        vote.Height = 1;
        vote.Round = 0;

        EXPECT_EQ(1, try_state_transition());
    }

    TEST(VoteParserTest, DeviceTest1) {
        vote_reset();
        vote_state_reset();

        vote_state.isInitialized = 1;
        vote_state.vote.Type = TYPE_PREVOTE;
        vote_state.vote.Height = 9;
        vote_state.vote.Round = 0;

        vote.Type = TYPE_PREVOTE;
        vote.Height = 1;
        vote.Round = 0;

        EXPECT_EQ(0, try_state_transition());
    }

}
