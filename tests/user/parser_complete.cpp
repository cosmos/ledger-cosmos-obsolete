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
    TEST(ParserIterator, EnumerateBasic1) {
        auto transaction = R"({"A":"1","B":"2","C":"3"})";

        auto expected_data = R"(
[0] 1/1 | A : 1
[1] 1/1 | B : 2
[2] 1/1 | C : 3
-----------)";

        auto expected_display = R"(
-----------)";

        auto pages = get_pages(transaction, 100);
        auto display_pages = get_display_pages(transaction, 100);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, EnumerateBasic2) {
        auto transaction = R"({"inputs":[{"address":"696E707574","coins":[{"amount":10,"denom":"atom"}]}],)" \
                           R"("outputs":[{"address":"6F7574707574","coins":[{"amount":11,"denom":"uatom"}]}]})";

        auto expected_data = R"(
[0] 1/1 | inputs/address : 696E707574
[1] 1/1 | inputs/coins : [{"amount":10,"denom":"atom"}]
[2] 1/1 | outputs/address : 6F7574707574
[3] 1/1 | outputs/coins : [{"amount":11,"denom":"uatom"}]
-----------)";

        auto expected_display = R"(
-----------)";

        auto pages = get_pages(transaction, 100);
        auto display_pages = get_display_pages(transaction, 100);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, Enumerate1) {
        auto transaction = R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]}],"sequence":"1"})";

        auto expected_data = R"(
[0] 1/1 | account_number : 0
[1] 1/1 | chain_id : test-chain-1
[2] 1/1 | fee/amount : [{"amount":"5","denom":"photon"}]
[3] 1/1 | fee/gas : 10000
[4] 1/1 | memo : testmemo
[5] 1/1 | msgs/inputs : [{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}]
[6] 1/1 | msgs/outputs : [{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]
[7] 1/1 | sequence : 1
-----------)";

        auto expected_display = R"(
[0] 1/1 | chain_id : test-chain-1
[1] 1/1 | account_number : 0
[2] 1/1 | sequence : 1
[3] 1/1 | fee : {"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"}
[4] 1/1 | memo : testmemo
[5] 1/1 | msgs_1/inputs/address : cosmosaccaddr1d9h8qat5e4ehc5
[6] 1/1 | msgs_1/inputs/coins : [{"amount":"10","denom":"atom"}]
[7] 1/1 | msgs_1/outputs/address : cosmosaccaddr1da6hgur4wse3jx32
[8] 1/1 | msgs_1/outputs/coins : [{"amount":"10","denom":"atom"}]
-----------)";

        auto pages = get_pages(transaction, 100);
        auto display_pages = get_display_pages(transaction, 100);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, Enumerate2) {
        auto transaction =
                R"({"account_number":"0","chain_id":"test-chain-1","fee":{"amount":[{"amount":"5","denom":"photon"}],"gas":"10000"},"memo":"testmemo","msgs":[{"inputs":[{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins":[{"amount":"10","denom":"atom"}]}],"outputs":[{"address":"cosmosaccaddr1da6hgur4wse3jx32","coins":[{"amount":"10","denom":"atom"}]}]},{"inputs":[{"address":"test2","coins":[{"amount":"20","denom":"bitcoin"}]}],"outputs":[{"address":"test3","coins":[{"amount":"50","denom":"ripple"}]}]}],"sequence":"1"})";

        auto expected_data = R"(
[0] 1/1 | account_number : 0
[1] 1/1 | chain_id : test-chain-1
[2] 1/1 | fee/amount : [{"amount":"5","denom":"photon"}]
[3] 1/1 | fee/gas : 10000
[4] 1/1 | memo : testmemo
[5] 1/2 | msgs/inputs : [{"address":"cosmosaccaddr1d9h8qat5e4ehc5","coins
[5] 2/2 | msgs/inputs : ":[{"amount":"10","denom":"atom"}]}]
[6] 1/2 | msgs/outputs : [{"address":"cosmosaccaddr1da6hgur4wse3jx32","coi
[6] 2/2 | msgs/outputs : ns":[{"amount":"10","denom":"atom"}]}]
[7] 1/2 | msgs/outputs/inputs : [{"address":"test2","coins":[{"amount":"20","deno
[7] 2/2 | msgs/outputs/inputs : m":"bitcoin"}]}]
[8] 1/2 | msgs/outputs/outputs : [{"address":"test3","coins":[{"amount":"50","deno
[8] 2/2 | msgs/outputs/outputs : m":"ripple"}]}]
[9] 1/1 | sequence : 1
-----------)";

        auto expected_display = R"(
[0] 1/1 | chain_id : test-chain-1
[1] 1/1 | account_number : 0
[2] 1/1 | sequence : 1
[3] 1/2 | fee : {"amount":[{"amount":"5","denom":"photon"}],"gas"
[3] 2/2 | fee : :"10000"}
[4] 1/1 | memo : testmemo
[5] 1/1 | msgs_1/inputs/address : cosmosaccaddr1d9h8qat5e4ehc5
[6] 1/1 | msgs_1/inputs/coins : [{"amount":"10","denom":"atom"}]
[7] 1/1 | msgs_1/outputs/address : cosmosaccaddr1da6hgur4wse3jx32
[8] 1/1 | msgs_1/outputs/coins : [{"amount":"10","denom":"atom"}]
[9] 1/1 | msgs_2/inputs/address : test2
[10] 1/1 | msgs_2/inputs/coins : [{"amount":"20","denom":"bitcoin"}]
[11] 1/1 | msgs_2/outputs/address : test3
[12] 1/1 | msgs_2/outputs/coins : [{"amount":"50","denom":"ripple"}]
-----------)";

        auto pages = get_pages(transaction, 50);
        auto display_pages = get_display_pages(transaction, 50);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, ValueCharacterShortToFitScreen) {
        auto transaction = R"({"chain_id":"1234567890AB","fee":"1234"})";

        auto expected_data = R"(
[0] 1/3 | chai : 1234
[0] 2/3 | chai : 5678
[0] 3/3 | chai : 90AB
[1] 1/1 | fee : 1234
-----------)";

        auto expected_display = R"(
[0] 1/3 | chain_id : 1234
[0] 2/3 | chain_id : 5678
[0] 3/3 | chain_id : 90AB
-----------)";

        auto pages = get_pages(transaction, 5);
        auto display_pages = get_display_pages(transaction, 5);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, ValueCharacterShortToFitScreen2) {
        auto transaction = R"({"chain_id":"1234567890ABC","fee":"12345"})";

        auto expected_data = R"(
[0] 1/4 | chai : 1234
[0] 2/4 | chai : 5678
[0] 3/4 | chai : 90AB
[0] 4/4 | chai : C
[1] 1/2 | fee : 1234
[1] 2/2 | fee : 5
-----------)";

        auto expected_display = R"(
[0] 1/4 | chain_id : 1234
[0] 2/4 | chain_id : 5678
[0] 3/4 | chain_id : 90AB
[0] 4/4 | chain_id : C
-----------)";

        auto pages = get_pages(transaction, 5);
        auto display_pages = get_display_pages(transaction, 5);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }


    TEST(ParserIterator, VeryLongValue) {
        auto transaction =
                R"({"chain_id":"test-chain-1","fee":{"amount":[{"amount":5,"denom":"photon"}],"gas":10000},"msgs":[{"inputs":[{"address":"696E707574","coins":[{"amount":10,"denom":"atom"}]}],"outputs":[{"address":"6F7574707574","coins":"LONGJUMPLIFELOVEDOVE"}]}],"sequence":1})";

        auto expected_data = R"(
[0] 1/3 | chai : test
[0] 2/3 | chai : -cha
[0] 3/3 | chai : in-1
[1] 1/8 | fee/ : [{"a
[1] 2/8 | fee/ : moun
[1] 3/8 | fee/ : t":5
[1] 4/8 | fee/ : ,"de
[1] 5/8 | fee/ : nom"
[1] 6/8 | fee/ : :"ph
[1] 7/8 | fee/ : oton
[1] 8/8 | fee/ : "}]
[2] 1/2 | fee/ : 1000
[2] 2/2 | fee/ : 0
[3] 1/17 | msgs : [{"a
[3] 2/17 | msgs : ddre
[3] 3/17 | msgs : ss":
[3] 4/17 | msgs : "696
[3] 5/17 | msgs : E707
[3] 6/17 | msgs : 574"
[3] 7/17 | msgs : ,"co
[3] 8/17 | msgs : ins"
[3] 9/17 | msgs : :[{"
[3] 10/17 | msgs : amou
[3] 11/17 | msgs : nt":
[3] 12/17 | msgs : 10,"
[3] 13/17 | msgs : deno
[3] 14/17 | msgs : m":"
[3] 15/17 | msgs : atom
[3] 16/17 | msgs : "}]}
[3] 17/17 | msgs : ]
[4] 1/15 | msgs : [{"a
[4] 2/15 | msgs : ddre
[4] 3/15 | msgs : ss":
[4] 4/15 | msgs : "6F7
[4] 5/15 | msgs : 5747
[4] 6/15 | msgs : 0757
[4] 7/15 | msgs : 4","
[4] 8/15 | msgs : coin
[4] 9/15 | msgs : s":"
[4] 10/15 | msgs : LONG
[4] 11/15 | msgs : JUMP
[4] 12/15 | msgs : LIFE
[4] 13/15 | msgs : LOVE
[4] 14/15 | msgs : DOVE
[4] 15/15 | msgs : "}]
[5] 1/1 | sequ : 1
-----------)";

        auto expected_display = R"(
[0] 1/3 | chain_id : test
[0] 2/3 | chain_id : -cha
[0] 3/3 | chain_id : in-1
-----------)";

        auto pages = get_pages(transaction, 5);
        auto display_pages = get_display_pages(transaction, 5);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, StackOverflow) {
        auto transaction = R"({"1":[[[[[[[[[[[[{"2":"4"}]]]]]]]]]]]]})";

        auto expected_data = R"(
[0] 1/1 | 1 : [[[[[[[[[[{"2":"4"}]]]]]]]]]]
-----------)";

        auto expected_display = R"(
-----------)";

        auto pages = get_pages(transaction, 100);
        auto display_pages = get_display_pages(transaction, 100);
        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

}