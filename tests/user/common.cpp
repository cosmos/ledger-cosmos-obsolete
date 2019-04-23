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
#include "common.h"

#include <lib/tx_display.h>
#include <gtest/gtest.h>
#include "common.h"

void setup_context(parsed_json_t *parsed_json,
                   unsigned short screen_size,
                   const char *transaction) {
    parsing_context_t context;
    context.parsed_tx = parsed_json;
    context.max_chars_per_key_line = screen_size;
    context.max_chars_per_value_line = screen_size;
    context.tx = transaction;
    set_parsing_context(context);
}

const char *parse_tx(parsed_json_t *parsed_json, const char *tx, uint16_t screen_size) {
    const char *err = json_parse(parsed_json, tx);
    setup_context(parsed_json, screen_size, tx);
    return err;
}

std::string get_pages(const char *input_json, int screen_size) {
    parsed_json_t parsed_json;
    const char *err = parse_tx(&parsed_json, input_json, screen_size);
    if (err != nullptr) {
        return std::string(err);
    }
    setup_context(&parsed_json, screen_size, input_json);

    tx_display_index_root();

    std::ostringstream out;
    char key[screen_size];
    char val[screen_size];
    int num_pages = 1000;

    out << std::endl;
    for (int16_t idx = 0; idx < num_pages; idx++) {
        int16_t num_chunks = 1;
        int16_t chunk_index = 0;

        while (chunk_index < num_chunks) {
            INIT_QUERY_CONTEXT(key, sizeof(key), val, sizeof(val), chunk_index, 2);
            tx_ctx.query.item_index = idx;
            num_chunks = tx_traverse(0);

            if (num_chunks > 0) {
                out << "[" << idx << "] "
                    << chunk_index + 1 << "/"
                    << num_chunks << " | "
                    << key << " : "
                    << val << std::endl;
            } else {
                out << "-----------";
                return out.str();
            }
            chunk_index++;
        }
    }

    return out.str();
}

std::string get_display_pages(const char *input_json, int screen_size) {
    parsed_json_t parsed_json;
    const char *err = parse_tx(&parsed_json, input_json, screen_size);
    if (err != nullptr) {
        return std::string(err);
    }
    setup_context(&parsed_json, screen_size, input_json);

    tx_display_index_root();

    std::ostringstream out;
    char key[screen_size];
    char value[screen_size];

    auto num_pages = tx_display_num_pages();
    auto display_cache = tx_display_cache();

    out << std::endl;
    for (int i = 0; i < sizeof(display_cache->num_subpages); i++) {
        out << "[" << i << "]" << (int) display_cache->num_subpages[i] << std::endl;
    }

    // Get raw items
    for (int16_t page_idx = 0; page_idx < num_pages + 1; page_idx++) {
        int16_t num_chunks = 1;
        int16_t chunk_index = 0;
        while (chunk_index < num_chunks) {
            INIT_QUERY(key, sizeof(key), value, sizeof(value), chunk_index)

            num_chunks = tx_display_get_item(page_idx);
            if (num_chunks > 0) {
                out << "[" << page_idx << "] ";
                out << chunk_index + 1 << "/" << num_chunks << " | ";
                out << key;
                if (strlen(value) > 0) {
                    out << " : " << value;
                } else {
                    out << " :";
                }
                out << std::endl;
            } else {
                out << "----------- " << num_pages;
                return out.str();
            }
            chunk_index++;


        }
    }

    return out.str();
}
