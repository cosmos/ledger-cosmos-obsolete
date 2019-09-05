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
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <jsmn.h>
#include <lib/json_parser.h>
#include <lib/tx_parser.h>
#include <lib/tx_display.h>
#include "common.h"

namespace {
    TEST(TxDisplay, Indexing) {
        auto transaction = R"({"account_number":"V1","chain_id":"V2","fee":{"amount":[],"gas":"V3"},"memo":"V4","msgs":[],"sequence":"V5"})";

        parsed_json_t parsed_json;
        auto err = parse_tx(&parsed_json, transaction, 100);
        ASSERT_STREQ(nullptr, err);

        auto num_pages = tx_display_numItems();
        EXPECT_EQ(6, num_pages) << "Invalid number of pages";

        auto display_cache = tx_display_cache();
        EXPECT_EQ(6, display_cache->numItems) << "Invalid number of pages";

        EXPECT_EQ(4, display_cache->subroot_start_token[0]) << "Invalid subroot chain_id";
        EXPECT_EQ(2, display_cache->subroot_start_token[1]) << "Invalid subroot account_number";
        EXPECT_EQ(16, display_cache->subroot_start_token[2]) << "Invalid subroot sequence";
        EXPECT_EQ(6, display_cache->subroot_start_token[3]) << "Invalid subroot fee";
        EXPECT_EQ(12, display_cache->subroot_start_token[4]) << "Invalid subroot memo";
        EXPECT_EQ(14, display_cache->subroot_start_token[5]) << "Invalid subroot msgs";
    }

    TEST(TxDisplay, GetItems) {
        auto transaction = R"({"account_number":"V1","chain_id":"V2","fee":{"amount":[],"gas":"V3"},"memo":"V4","msgs":[],"sequence":"V5"})";

        parsed_json_t parsed_json;
        auto err = parse_tx(&parsed_json, transaction, 100);
        ASSERT_STREQ(nullptr, err);
        tx_display_index_root();

        char key[100];
        char val[100];

        INIT_QUERY(key, sizeof(key), val, sizeof(val), 0)
        auto num_chunks = tx_display_get_item(0);
        EXPECT_EQ(1, num_chunks) << "Invalid number of chunks";
        EXPECT_EQ_STR(key, "chain_id", "Wrong key")
        EXPECT_EQ_STR(val, "V2", "Wrong value")

        INIT_QUERY(key, sizeof(key), val, sizeof(val), 0)
        num_chunks = tx_display_get_item(1);
        EXPECT_EQ(1, num_chunks) << "Invalid number of chunks";
        EXPECT_EQ_STR(key, "account_number", "Wrong key");
        EXPECT_EQ_STR(val, "V1", "Wrong value");
    }


    TEST(TxDisplay, OutOfBoundsSmall) {
        auto transaction = R"({"chain_id":"chain-1","fee":{"amount":[{"amount":5,"denom":"photon"}],"gas":10000},"msgs":[{"inputs":[{"address":"696E707574","coins":[{"amount":10,"denom":"atom"}]}],"outputs":[{"address":"6F7574707574","coins":"abcdefghijklmnopqr"}]}],"sequence":1})";

        parsed_json_t parsed_json;
        auto err = parse_tx(&parsed_json, transaction, 100);
        ASSERT_STREQ(nullptr, err);

        tx_display_index_root();

        char key[100];
        char val[100];

        int16_t found;
        // We should find it.. but later tx_display should fail
        INIT_QUERY_CONTEXT(key, sizeof(key), val, sizeof(val), 0, 4);
        tx_ctx.query.item_index = 5;
        found = tx_traverse(0);
        EXPECT_EQ(1, found) << "Item not found";

        //
        INIT_QUERY(key, sizeof(key), val, sizeof(val), 5)
        int16_t num_chunk = tx_display_get_item(8);
        EXPECT_EQ(-1, num_chunk) << "Item not found";

        INIT_QUERY(key, sizeof(key), val, sizeof(val), 5)
        num_chunk = tx_display_get_item(1);
        EXPECT_EQ(-1, num_chunk) << "Item not found";
    }
}
