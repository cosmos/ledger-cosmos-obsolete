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
#include <lib/transaction_parser.h>
#include "common.h"

namespace {
// Test parsing real Cosmos transactions

    TEST(TransactionParserTest, Transaction_Page_Count) {

        auto transaction = R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_json;
        const char *err = json_parse(&parsed_json, transaction);
        ASSERT_STREQ(nullptr, err);

        constexpr int screen_size = 100;
        setup_context(&parsed_json, screen_size, transaction);

        EXPECT_EQ(9, transaction_get_display_pages()) << "Wrong number of pages";
    }

    TEST(TransactionParserTest, Transaction_Page_Count_MultipleMsgs) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]},{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]},{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]},{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_json;
        const char *err = json_parse(&parsed_json, transaction);
        ASSERT_STREQ(nullptr, err);

        constexpr int screen_size = 100;
        setup_context(&parsed_json, screen_size, transaction);

        EXPECT_EQ(21, transaction_get_display_pages()) << "Wrong number of pages";
    }

    TEST(TransactionParserTest, ParseTransaction_OutOfBounds) {

        auto transaction =
                R"({"chain_id":"test-chain-1","fee":{"amount":[{"amount":5,"denom":"photon"}],"gas":10000},"msgs":[{"inputs":[{"address":"696E707574","coins":[{"amount":10,"denom":"atom"}]}],"outputs":[{"address":"6F7574707574","coins":"LONGJUMPLIFELOVEDOVE"}]}],"sequence":1})";
        parsed_json_t parsed_json;
        const char *err = json_parse(&parsed_json, transaction);
        ASSERT_STREQ(nullptr, err);

        constexpr int screen_size = 5;
        setup_context(&parsed_json, screen_size, transaction);

        char key[32];
        char value[screen_size];

        // String: LONGJUMPLIFELOVEDOVE

        int16_t num_chunk = transaction_get_display_key_value(key, sizeof(key),
                                                              value, sizeof(value),
                                                              8, -1);

        EXPECT_EQ(num_chunk, 5) << "Wrong number of chunks";
        EXPECT_EQ_STR(value, "", "Wrong value");

        num_chunk = transaction_get_display_key_value(key, sizeof(key),
                                                      value, sizeof(value),
                                                      8,
                                                      10);

        EXPECT_EQ(num_chunk, 5) << "Wrong number of chunks";
        EXPECT_EQ_STR(value, "", "Wrong value");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_CorrectFormat) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_TRUE(error_msg == nullptr) << "Validation failed, error: " << error_msg;
    }

    TEST(TransactionParserTest, TransactionJsonValidation_MissingAccountNumber) {

        auto transaction =
                R"({"chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Missing account_number",
                      "Validation should fail because account_number is missing");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_MissingChainId) {

        auto transaction =
                R"({"account_number":"0","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Missing chain_id", "Validation should fail because chain_id is missing");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_MissingFee) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fees":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Missing fee", "Validation should fail because fee is missing");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_MissingMsgs) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgsble":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Missing msgs", "Validation should fail because msgs is missing");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_MissingSequence) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}]})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Missing sequence", "Validation should fail because sequence is missing");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_Spaces_InTheMiddle) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1", "fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Contains whitespace in the corpus",
                      "Validation should fail because contains whitespace in the corpus");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_Spaces_AtTheFront) {

        auto transaction =
                R"({  "account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Contains whitespace in the corpus",
                      "Validation should fail because contains whitespace in the corpus");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_Spaces_AtTheEnd) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"  })";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Contains whitespace in the corpus",
                      "Validation should fail because contains whitespace in the corpus");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_Spaces_Lots) {

        auto transaction =
                R"({"account_number":"0",   "chain_id":"test-chain-1",    "fee":{"amount":    [{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Contains whitespace in the corpus",
                      "Validation should fail because contains whitespace in the corpus");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_AllowSpacesInString) {

        auto transaction =
                R"({"account_number":"0","chain_id":"    test-chain-1    ","fee":{"amount":[{"amount":"5","denom":"    photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_TRUE(error_msg == nullptr) << "Validation failed, error: " << error_msg;
    }

    TEST(TransactionParserTest, TransactionJsonValidation_SortedDictionary) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_TRUE(error_msg == nullptr) << "Validation failed, error: " << error_msg;
    }

    TEST(TransactionParserTest, TransactionJsonValidation_NotSortedDictionary_FirstElement) {

        auto transaction =
                R"({"chain_id":"test-chain-1","account_number":"0","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Dictionaries are not sorted",
                      "Validation should fail because dictionaries are not sorted");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_NotSortedDictionary_MiddleElement) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","memo":"testmemo","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Dictionaries are not sorted",
                      "Validation should fail because dictionaries are not sorted");
    }

    TEST(TransactionParserTest, TransactionJsonValidation_NotSortedDictionary_LastElement) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","sequence":"1","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}]})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Dictionaries are not sorted",
                      "Validation should fail because dictionaries are not sorted");
    }

// This json has been taken directly from goclient which uses cosmos to serialize a simple tx
// This test is currently failing the validation.
// We are reviewing the validation code and cosmos serialization to find the culprit.

    TEST(TransactionParserTest, TransactionJsonValidation_CosmosExample) {

        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_TRUE(error_msg == nullptr) << "Validation failed, error: " << error_msg;
    }

    TEST(TransactionParserTest, TransactionJsonValidation_GaiaCLIissue) {

        auto transaction = R"({"account_number":"811","chain_id":"cosmoshub-1","fee":{"amount":[],"gas":"5000000"},"memo":"","msgs":[{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj","value":{"amount":"8000000000","denom":"uatom"}}}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ(nullptr, err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_TRUE(error_msg == nullptr) << "Validation failed, error: " << error_msg;
    }

    TEST(TransactionParserTest, TransactionJsonValidation_GaiaCLIissueBigTX) {

        auto transaction = R"({"account_number":"811","chain_id":"cosmoshub-1","fee":{"amount":[],"gas":"5000000"},"memo":"","msgs":[{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj","value":{"amount":"8000000000","denom":"uatom"}}},{"type":"cosmos-sdk/MsgDelegate","value":{"delegator_address":"cosmos13vfzpfmg6jgzfk4rke9glzpngrzucjtanq9awx","validator_address":"cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj","value":{"amount":"8000000000","denom":"uatom"}}}],"sequence":"1"})";

        parsed_json_t parsed_transaction;
        const char *err = json_parse(&parsed_transaction, transaction);
        ASSERT_STREQ("NOMEM: JSON string contains too many tokens", err);

        const char *error_msg = json_validate(&parsed_transaction, transaction);
        EXPECT_EQ_STR(error_msg, "JSON Missing chain_id", "Validation failed, error");
    }
}