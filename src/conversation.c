#include "conversation.h"
#include "util.h"
#include "tomlc17.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


Conversation allConvos[512] = {};


int parse_convo_node(const toml_datum_t* const tomlNode, ConvoNode* const outNode) {
    const toml_datum_t nodeTextDatum = toml_get(*tomlNode, "text");
    const toml_datum_t nodeSetDatum = toml_get(*tomlNode, "setsNarrative");
    const toml_datum_t nodeUnsetDatum = toml_get(*tomlNode, "unsetsNarrative");
    if (nodeTextDatum.type != TOML_STRING || nodeTextDatum.u.str.len >= sizeof(outNode->text) || nodeTextDatum.u.str.len == 0) {
        log_error("Node text is not a string, or is too long, or has no content!\n");
        return EOF;
    }
    if (nodeSetDatum.type != TOML_UNKNOWN && (nodeSetDatum.type != TOML_INT64 || nodeSetDatum.u.int64 < 0)) {
        log_error("Node set must be integer >= 0!\n");
        return EOF;
    }
    if (nodeUnsetDatum.type != TOML_UNKNOWN && (nodeUnsetDatum.type != TOML_INT64 || nodeUnsetDatum.u.int64 < 0)) {
        log_error("Node unset must be integer >= 0!\n");
        return EOF;
    }
    strncpy(outNode->text, nodeTextDatum.u.str.ptr, nodeTextDatum.u.str.len);
    if (nodeSetDatum.type != TOML_UNKNOWN) {
        outNode->setsFlag = nodeSetDatum.u.int64;
    }
    if (nodeUnsetDatum.type != TOML_UNKNOWN) {
        outNode->unsetsFlag = nodeUnsetDatum.u.int64;
    }

    const toml_datum_t nodeOptionsDatum = toml_get(*tomlNode, "option");
    if (nodeOptionsDatum.type == TOML_UNKNOWN) {
        return 0;
    }
    if (nodeOptionsDatum.type != TOML_ARRAY || nodeOptionsDatum.u.arr.elem[0].type != TOML_TABLE) {
        log_error("Node options must be an array of tables!\n");
        return EOF;
    }
    if (nodeOptionsDatum.u.arr.size > sizeof(outNode->options)/sizeof(outNode->options[0])) {
        log_error("Node options array is too long!\n");
        return EOF;
    }

    for (size_t i = 0; i < nodeOptionsDatum.u.arr.size; ++i) {
        const toml_datum_t optionTextDatum = toml_get(nodeOptionsDatum.u.arr.elem[i], "text");
        const toml_datum_t optionLinkDatum = toml_get(nodeOptionsDatum.u.arr.elem[i], "linksTo");
        if (optionTextDatum.type != TOML_STRING || optionTextDatum.u.str.len >= sizeof(outNode->options[i].text) || optionTextDatum.u.str.len == 0) {
            log_error("Node option text is not a string, or is too long, or has no content!\n");
            return EOF;
        }
        if (optionLinkDatum.type != TOML_INT64 || optionLinkDatum.u.int64 <= 0) {
            log_error("Option link must be an integer > 0!\n");
            return EOF;
        }

        strncpy(outNode->options[i].text, optionTextDatum.u.str.ptr, optionTextDatum.u.str.len);
        outNode->options[i].linksTo = optionLinkDatum.u.int64;
    }

    return 0;
}


int parse_convos(const char* const filePath) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse convos file\n");
        toml_free(tomlResult);
        return EOF;
    }

    const toml_datum_t convosDatum = toml_get(tomlResult.toptab, "conversation");
    if (convosDatum.type != TOML_ARRAY || convosDatum.u.arr.elem[0].type != TOML_TABLE) {
        log_error("Conversations TOML must be an array of tables!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (convosDatum.u.arr.size > sizeof(allConvos)/sizeof(allConvos[0])) {
        log_error("Conversations array is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }

    const size_t maxNodes = sizeof(allConvos[0].nodes) / sizeof(allConvos[0].nodes[0]);
    for (size_t i = 0; i < convosDatum.u.arr.size; ++i) {
        const toml_datum_t convoNodesDatum = toml_get(convosDatum.u.arr.elem[i], "node");
        if (convoNodesDatum.type != TOML_ARRAY || convoNodesDatum.u.arr.elem[0].type != TOML_TABLE) {
            log_error("Conversation nodes must be an array of tables!\n");
            toml_free(tomlResult);
            return EOF;
        }
        if (convoNodesDatum.u.arr.size > maxNodes) {
            log_error("Conversation nodes array is too long!\n");
            toml_free(tomlResult);
            return EOF;
        }

        for (size_t j = 0; j < convoNodesDatum.u.arr.size; ++j) {
            if (parse_convo_node(&(convoNodesDatum.u.arr.elem[j]), &(allConvos[i].nodes[j])) == EOF) {
                toml_free(tomlResult);
                return EOF;
            }
        }
    }

    toml_free(tomlResult);
    return 0;
}


void print_all_convos() {
    const size_t maxConvos = sizeof(allConvos) / sizeof(allConvos[0]);
    const size_t maxNodes = sizeof(allConvos[0].nodes) / sizeof(allConvos[0].nodes[0]);
    for (size_t i = 0; i < maxConvos; ++i) {
        if (allConvos[i].nodes[0].text[0] == '\0') {
            continue;
        }
        printf("Conversation %zu\n", i+1);
        for (size_t j = 0; j < maxNodes; ++j) {
            if (allConvos[i].nodes[j].text[0] == '\0') {
                break;
            }
            printf("%s\n", allConvos[i].nodes[j].text);
        }
    }
}
