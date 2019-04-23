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

#include <gtest/gtest.h>
#include <lib/json_parser.h>

#define EXPECT_EQ_STR(_STR1, _STR2, _ERROR_MESSAGE) { if (_STR1 != nullptr & _STR2 != nullptr) \
EXPECT_TRUE(!strcmp(_STR1, _STR2)) << _ERROR_MESSAGE << ", expected: " << _STR2 << ", received: " << _STR1; \
else FAIL() << "One of the strings is null"; }

void setup_context(parsed_json_t *parsed_json,
                   unsigned short screen_size,
                   const char *transaction);

const char *parse_tx(parsed_json_t *parsed_json, const char *tx, uint16_t screen_size);

std::string get_pages(const char *input_json, int screen_size);

std::string get_display_pages(const char *input_json, int screen_size);
