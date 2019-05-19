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
    TEST(ParserIterator, MinimalTx_RawPages) {
        auto transaction = R"({"account_number":"V1","chain_id":"V2","fee":{"amount":[{"a":"b"}, {"c":"d"}, {"x":"y"}],"gas":"V3"},"memo":"V4","msgs":[],"sequence":"V5"})";

        auto expected_data = R"(
[0] 1/1 | account_number : V1
[1] 1/1 | chain_id : V2
[2] 1/1 | fee/amount : [{"a":"b"}, {"c":"d"}, {"x":"y"}]
[3] 1/1 | fee/gas : V3
[4] 1/1 | memo : V4
[5] 1/1 | sequence : V5
-----------)";
        auto pages = get_pages(transaction, 100);
        std::cout << pages << std::endl;
        ASSERT_EQ(expected_data, pages);
    }

    TEST(ParserIterator, MinimalTx_DisplayPages) {
        auto transaction = R"({"account_number":"V1","chain_id":"V2","fee":{"amount":[{"a":"b"}, {"c":"d"}, {"x":"y"}],"gas":"V3"},"memo":"V4","msgs":[{"m1":"z1"}, {"m2":"z2"}, {"m3":"z3"}],"sequence":"V5"})";
        auto expected_display = R"(
[0]1
[1]1
[2]1
[3]2
[4]1
[5]3
[0] 1/1 | chain_id : V2
[1] 1/1 | account_number : V1
[2] 1/1 | sequence : V5
[3] 1/1 | fee/amount : [{"a":"b"}, {"c":"d"}, {"x":"y"}]
[4] 1/1 | fee/gas : V3
[5] 1/1 | memo : V4
[6] 1/1 | msgs/m1 : z1
[7] 1/1 | msgs/m2 : z2
[8] 1/1 | msgs/m3 : z3
----------- 9)";
        auto display_pages = get_display_pages(transaction, 100);
        std::cout << display_pages << std::endl;
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, EnumerateBasic1) {
        auto transaction = R"({"A":"1","B":"2","C":"3"})";

        auto expected_data = R"(
[0] 1/1 | A : 1
[1] 1/1 | B : 2
[2] 1/1 | C : 3
-----------)";

        auto expected_display = R"(
[0]0
[1]0
[2]0
[3]0
[4]0
[5]0
----------- 0)";

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
[0]0
[1]0
[2]0
[3]0
[4]0
[5]0
----------- 0)";

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
[0]1
[1]1
[2]1
[3]2
[4]1
[5]4
[0] 1/1 | chain_id : test-chain-1
[1] 1/1 | account_number : 0
[2] 1/1 | sequence : 1
[3] 1/1 | fee/amount : [{"amount":"5","denom":"photon"}]
[4] 1/1 | fee/gas : 10000
[5] 1/1 | memo : testmemo
[6] 1/1 | msgs/inputs/address : cosmosaccaddr1d9h8qat5e4ehc5
[7] 1/1 | msgs/inputs/coins : [{"amount":"10","denom":"atom"}]
[8] 1/1 | msgs/outputs/address : cosmosaccaddr1da6hgur4wse3jx32
[9] 1/1 | msgs/outputs/coins : [{"amount":"10","denom":"atom"}]
----------- 10)";

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
[7] 1/2 | msgs/inputs : [{"address":"test2","coins":[{"amount":"20","deno
[7] 2/2 | msgs/inputs : m":"bitcoin"}]}]
[8] 1/2 | msgs/outputs : [{"address":"test3","coins":[{"amount":"50","deno
[8] 2/2 | msgs/outputs : m":"ripple"}]}]
[9] 1/1 | sequence : 1
-----------)";

        auto expected_display = R"(
[0]1
[1]1
[2]1
[3]2
[4]1
[5]8
[0] 1/1 | chain_id : test-chain-1
[1] 1/1 | account_number : 0
[2] 1/1 | sequence : 1
[3] 1/1 | fee/amount : [{"amount":"5","denom":"photon"}]
[4] 1/1 | fee/gas : 10000
[5] 1/1 | memo : testmemo
[6] 1/1 | msgs/inputs/address : cosmosaccaddr1d9h8qat5e4ehc5
[7] 1/1 | msgs/inputs/coins : [{"amount":"10","denom":"atom"}]
[8] 1/1 | msgs/outputs/address : cosmosaccaddr1da6hgur4wse3jx32
[9] 1/1 | msgs/outputs/coins : [{"amount":"10","denom":"atom"}]
[10] 1/1 | msgs/inputs/address : test2
[11] 1/1 | msgs/inputs/coins : [{"amount":"20","denom":"bitcoin"}]
[12] 1/1 | msgs/outputs/address : test3
[13] 1/1 | msgs/outputs/coins : [{"amount":"50","denom":"ripple"}]
----------- 14)";

        auto pages = get_pages(transaction, 50);
        auto display_pages = get_display_pages(transaction, 50);

        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;

        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, Enumerate3) {
        auto transaction =
                R"({"account_number":"2","chain_id":"local-testnet","fee":{"amount":[],"gas":"500000"},"memo":"abc","msgs":[{"description":"test","initial_deposit":[{"amount":"1","denom":"stake"}],"proposal_type":"Text","proposer":"cosmos101234567890abcdefghijklmnopqrstuvwxyz0","title":"test"}],"sequence":"0"})";

        auto expected_data = R"(
[0] 1/1 | account_number : 2
[1] 1/1 | chain_id : local-testnet
[2] 1/1 | fee/amount : []
[3] 1/1 | fee/gas : 500000
[4] 1/1 | memo : abc
[5] 1/1 | msgs/description : test
[6] 1/1 | msgs/initial_deposit : [{"amount":"1","denom":"stake"}]
[7] 1/1 | msgs/proposal_type : Text
[8] 1/1 | msgs/proposer : cosmos101234567890abcdefghijklmnopqrstuvwxyz0
[9] 1/1 | msgs/title : test
[10] 1/1 | sequence : 0
-----------)";

        auto expected_display = R"(
[0]1
[1]1
[2]1
[3]2
[4]1
[5]6
[0] 1/1 | chain_id : local-testnet
[1] 1/1 | account_number : 2
[2] 1/1 | sequence : 0
[3] 1/1 | fee/amount : []
[4] 1/1 | fee/gas : 500000
[5] 1/1 | memo : abc
[6] 1/1 | msgs/description : test
[7] 1/1 | msgs/initial_deposit/amount : 1
[8] 1/1 | msgs/initial_deposit/denom : stake
[9] 1/1 | msgs/proposal_type : Text
[10] 1/1 | msgs/proposer : cosmos101234567890abcdefghijklmnopqrstuvwxyz0
[11] 1/1 | msgs/title : test
----------- 12)";

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
[0]1
[1]0
[2]0
[3]0
[4]0
[5]0
[0] 1/3 | chai : 1234
[0] 2/3 | chai : 5678
[0] 3/3 | chai : 90AB
----------- 1)";

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
[0]1
[1]0
[2]0
[3]0
[4]0
[5]0
[0] 1/4 | chai : 1234
[0] 2/4 | chai : 5678
[0] 3/4 | chai : 90AB
[0] 4/4 | chai : C
----------- 1)";

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
[0]1
[1]0
[2]0
[3]0
[4]0
[5]0
[0] 1/3 | chai : test
[0] 2/3 | chai : -cha
[0] 3/3 | chai : in-1
----------- 1)";

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
[0] 1/1 | 1 : [[[[[[[{"2":"4"}]]]]]]]
-----------)";

        auto expected_display = R"(
[0]0
[1]0
[2]0
[3]0
[4]0
[5]0
----------- 0)";

        auto pages = get_pages(transaction, 100);
        auto display_pages = get_display_pages(transaction, 100);

        std::cout << pages << std::endl;
        std::cout << display_pages << std::endl;

        ASSERT_EQ(expected_data, pages);
        ASSERT_EQ(expected_display, display_pages);
    }

    TEST(ParserIterator, BigTX) {
        auto transaction = R"({"account_number":"108","chain_id":"cosmoshub-2","fee":{"amount":[{"amount":"600","denom":"uatom"}],"gas":"200000"},"memo":"","msgs":[{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1qwl879nx9t6kef4supyazayf7vjhennyh568ys"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1x88j7vp2xnw3zec8ur3g4waxycyz7m0mahdv3p"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1ttfytaf43nkytzp8hkfjfgjc693ky4t3y2n2ku"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1wdrypwex63geqswmcy5qynv4w3z3dyef2qmyna"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper102ruvpv2srmunfffxavttxnhezln6fnc54at8c"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1sxx9mszve0gaedz5ld7qdkjkfv8z992ax69k08"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1ssm0d433seakyak8kcf93yefhknjleeds4y3em"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper13sduv92y3xdhy3rpmhakrc3v7t37e7ps9l0kpv"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper15urq2dtp9qce4fyc85m6upwm9xul3049e02707"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper14lultfckehtszvzw4ehu0apvsr77afvyju5zzy"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1k9a0cs97vul8w2vwknlfmpez6prv8klv03lv3d"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1kj0h4kn4z5xvedu2nd9c4a9a559wvpuvu0h6qn"}},{"type":"cosmos-sdk/MsgWithdrawDelegationReward","value":{"delegator_address":"cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh","validator_address":"cosmosvaloper1hjct6q7npsspsg3dgvzk3sdf89spmlpfdn6m9d"}}],"sequence":"106"})";

        auto expected_display = R"(
[0]1
[1]1
[2]1
[3]2
[4]1
[5]48
[0] 1/1 | Chain ID : cosmoshub-2
[1] 1/1 | Account : 108
[2] 1/1 | Sequence : 106
[3] 1/1 | Fee : [{"amount":"600","denom":"uatom"}]
[4] 1/1 | Gas : 200000
[5] 1/1 | Memo :
[6] 1/1 | Type : Withdraw Reward
[7] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[8] 1/1 | Validator : cosmosvaloper1qwl879nx9t6kef4supyazayf7vjhennyh568ys
[9] 1/1 | Type : Withdraw Reward
[10] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[11] 1/1 | Validator : cosmosvaloper1x88j7vp2xnw3zec8ur3g4waxycyz7m0mahdv3p
[12] 1/1 | Type : Withdraw Reward
[13] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[14] 1/1 | Validator : cosmosvaloper1grgelyng2v6v3t8z87wu3sxgt9m5s03xfytvz7
[15] 1/1 | Type : Withdraw Reward
[16] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[17] 1/1 | Validator : cosmosvaloper1ttfytaf43nkytzp8hkfjfgjc693ky4t3y2n2ku
[18] 1/1 | Type : Withdraw Reward
[19] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[20] 1/1 | Validator : cosmosvaloper1wdrypwex63geqswmcy5qynv4w3z3dyef2qmyna
[21] 1/1 | Type : Withdraw Reward
[22] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[23] 1/1 | Validator : cosmosvaloper102ruvpv2srmunfffxavttxnhezln6fnc54at8c
[24] 1/1 | Type : Withdraw Reward
[25] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[26] 1/1 | Validator : cosmosvaloper10e4vsut6suau8tk9m6dnrm0slgd6npe3jx5xpv
[27] 1/1 | Type : Withdraw Reward
[28] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[29] 1/1 | Validator : cosmosvaloper1sxx9mszve0gaedz5ld7qdkjkfv8z992ax69k08
[30] 1/1 | Type : Withdraw Reward
[31] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[32] 1/1 | Validator : cosmosvaloper1ssm0d433seakyak8kcf93yefhknjleeds4y3em
[33] 1/1 | Type : Withdraw Reward
[34] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[35] 1/1 | Validator : cosmosvaloper13sduv92y3xdhy3rpmhakrc3v7t37e7ps9l0kpv
[36] 1/1 | Type : Withdraw Reward
[37] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[38] 1/1 | Validator : cosmosvaloper15urq2dtp9qce4fyc85m6upwm9xul3049e02707
[39] 1/1 | Type : Withdraw Reward
[40] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[41] 1/1 | Validator : cosmosvaloper14kn0kk33szpwus9nh8n87fjel8djx0y070ymmj
[42] 1/1 | Type : Withdraw Reward
[43] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[44] 1/1 | Validator : cosmosvaloper14lultfckehtszvzw4ehu0apvsr77afvyju5zzy
[45] 1/1 | Type : Withdraw Reward
[46] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[47] 1/1 | Validator : cosmosvaloper1k9a0cs97vul8w2vwknlfmpez6prv8klv03lv3d
[48] 1/1 | Type : Withdraw Reward
[49] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[50] 1/1 | Validator : cosmosvaloper1kj0h4kn4z5xvedu2nd9c4a9a559wvpuvu0h6qn
[51] 1/1 | Type : Withdraw Reward
[52] 1/1 | Delegator : cosmos14lultfckehtszvzw4ehu0apvsr77afvyhgqhwh
[53] 1/1 | Validator : cosmosvaloper1hjct6q7npsspsg3dgvzk3sdf89spmlpfdn6m9d
----------- 54)";

        auto pages = get_pages(transaction, 100);
        auto display_pages = get_display_pages(transaction, 100, true);

        std::cout << display_pages << std::endl;

        ASSERT_EQ(expected_display, display_pages);
    }

}
