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

#include <lib/transaction_parser.h>
#include <gtest/gtest.h>
#include "common.h"

void setup_context(
        parsed_json_t *parsed_json,
        unsigned short screen_size,
        const char *transaction) {
    parsing_context_t context;
    context.parsed_tx = parsed_json;
    context.max_chars_per_key_line = screen_size;
    context.max_chars_per_value_line = screen_size;
    context.tx = transaction;
    set_parsing_context(context);
    set_copy_delegate([](void *d, const void *s, size_t size) { memcpy(d, s, size); });
}

std::string get_pages(const char *input_json, int screen_size) {
    std::ostringstream out;

    parsed_json_t parsed_json;
    const char *err = json_parse(&parsed_json, input_json);

    if (!err) {
        char key[screen_size];
        char value[screen_size];
        setup_context(&parsed_json, screen_size, input_json);

        int num_pages = 1000;

        out << std::endl;
        for (int16_t page_idx = 0; page_idx < num_pages; page_idx++) {
            int16_t num_chunks = 1;
            int16_t chunk_index = 0;
            while (chunk_index < num_chunks) {
                num_chunks = display_arbitrary_item(page_idx, // page index
                                                    key, sizeof(key),
                                                    value, sizeof(value),
                                                    0, // token index of the root element
                                                    chunk_index);
                if (num_chunks > 0) {
                    out << "[" << page_idx << "] " << chunk_index + 1 << "/" << num_chunks << " | " << key << " : "
                        << value << std::endl;
                } else {
                    out << "-----------";
                    return out.str();
                }
                chunk_index++;
            }
        }
    }

    return out.str();
}

std::string get_display_pages(const char *input_json, int screen_size) {
    std::ostringstream out;

    parsed_json_t parsed_json;
    const char *err = json_parse(&parsed_json, input_json);

    if (!err) {
        char key[screen_size];
        char value[screen_size];
        setup_context(&parsed_json, screen_size, input_json);

        int num_pages = 1000;

        out << std::endl;
        for (int16_t page_idx = 0; page_idx < num_pages; page_idx++) {
            int16_t num_chunks = 1;
            int16_t chunk_index = 0;
            while (chunk_index < num_chunks) {
                num_chunks = transaction_get_display_key_value(key, sizeof(key),
                                                               value, sizeof(value),
                                                               page_idx, chunk_index);
                if (num_chunks > 0) {
                    out << "[" << page_idx << "] " << chunk_index + 1 << "/" << num_chunks << " | " << key << " : "
                        << value << std::endl;
                } else {
                    out << "-----------";
                    return out.str();
                }
                chunk_index++;
            }
        }
    }

    return out.str();
}


void EXPECT_EQ_STR(const char *str1, const char *str2, const char *errorMsg) {
    if (str1 != nullptr & str2 != nullptr) {
        EXPECT_TRUE(strcmp(str1, str2) == 0) << errorMsg << ", expected: " << str2 << ", received: " << str1;
    } else {
        FAIL() << "One of the strings is null";
    }
}
