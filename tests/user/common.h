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

#include <lib/json_parser.h>

void setup_context(parsed_json_t *parsed_json,
                   unsigned short screen_size,
                   const char *transaction);

std::string get_pages(const char *input_json, int screen_size);

std::string get_display_pages(const char *input_json, int screen_size);

void EXPECT_EQ_STR(const char *str1, const char *str2, const char *errorMsg);
