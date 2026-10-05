#include "conversation.h"
#include "util.h"
#include "tomlc17.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


Conversation allConvos[512] = {};


int parse_convo_node(const toml_datum_t* const tomlNode, ConvoNode* const outNode) {
    const toml_datum_t nodeTextDatum = toml_get(*tomlNode, "text");
    if (nodeTextDatum.type != TOML_STRING || nodeTextDatum.u.str.len >= sizeof(outNode->text) || nodeTextDatum.u.str.len == 0) {
        log_error("Node text is not a string, or is too long, or has no content!\n");
        return EOF;
    }
    strncpy(outNode->text, nodeTextDatum.u.str.ptr, nodeTextDatum.u.str.len);

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
        return EOF;
    }

    const toml_datum_t convosDatum = toml_get(tomlResult.toptab, "conversation");
    if (convosDatum.type != TOML_ARRAY || convosDatum.u.arr.elem[0].type != TOML_TABLE) {
        log_error("Conversations TOML must be an array of tables!\n");
        return EOF;
    }
    if (convosDatum.u.arr.size > sizeof(allConvos)/sizeof(allConvos[0])) {
        log_error("Conversations array is too long!\n");
        return EOF;
    }

    const size_t maxNodes = sizeof(allConvos[0].nodes) / sizeof(allConvos[0].nodes[0]);
    for (size_t i = 0; i < convosDatum.u.arr.size; ++i) {
        const toml_datum_t convoIDDatum = toml_get(convosDatum.u.arr.elem[i], "id");
        if (convoIDDatum.type != TOML_INT64 || convoIDDatum.u.int64 <= 0) {
            log_error("Conversation IDs must be integers > 0!\n");
            return EOF;
        }
        allConvos[i].id = convoIDDatum.u.int64;
        const toml_datum_t convoNodesDatum = toml_get(convosDatum.u.arr.elem[i], "node");
        if (convoNodesDatum.type != TOML_ARRAY || convoNodesDatum.u.arr.elem[0].type != TOML_TABLE) {
            log_error("Conversation nodes must be an array of tables!\n");
            return EOF;
        }
        if (convoNodesDatum.u.arr.size > maxNodes) {
            log_error("Conversation nodes array is too long!\n");
            return EOF;
        }

        for (size_t j = 0; j < convoNodesDatum.u.arr.size; ++j) {
            if (parse_convo_node(&(convoNodesDatum.u.arr.elem[j]), &(allConvos[i].nodes[j])) == EOF) {
                return EOF;
            }
        }
    }
    return 0;
}


void conversation(const unsigned id) {
    Conversation* convo = nullptr;
    size_t countConvos = sizeof(allConvos) / sizeof(allConvos[0]);
    for (size_t i = 0; i < countConvos; ++i) {
        if (allConvos[i].id == id) {
            convo = &allConvos[i];
            break;
        } else if (allConvos[i].id == 0) {
            break;
        }
    }
    if (convo == nullptr) {
        log_error("Conversation not found!\n");
        return;
    }

    bool exit = false;
    ConvoNode* node = &(convo->nodes[0]);
    size_t maxOptions = sizeof(node->options) / sizeof(node->options[0]);
    const size_t numCharsToRead = (maxOptions / 10) + 1;
    while (!exit) {
        printf("%s\n", node->text);
        size_t numOptions = 0;
        char readText[numCharsToRead + 1] = {};
        for (size_t i = 0; i < maxOptions; ++i) {
            // We don't have IDs on the options, cos we don't need them; 
            // so we just check if the option text is empty.
            if (node->options[i].text[0] == '\0') {
                break;
            }
            ++numOptions;
            printf("[%zu] %s\n", i, node->options[i].text);
        }
        if (numOptions == 0) {
            break;
        }
        printf("[%zu] Exit\n", numOptions);

        fgets(readText, numCharsToRead+1, stdin);
        const int selection = atoi(readText);
        if (selection < 0 || selection > numOptions) {
            printf("Invalid Input!\n");
        } else if (selection == numOptions) {
            exit = true;
        } else {
            const unsigned link = node->options[selection].linksTo;
            node = &(convo->nodes[link - 1]);
        }
    }
}


void print_all_convos() {
    const size_t maxConvos = sizeof(allConvos) / sizeof(allConvos[0]);
    const size_t maxNodes = sizeof(allConvos[0].nodes) / sizeof(allConvos[0].nodes[0]);
    for (size_t i = 0; i < maxConvos; ++i) {
        if (allConvos[i].nodes[0].text[0] == '\0') {
            continue;
        }
        printf("Conversation %u\n", allConvos[i].id);
        for (size_t j = 0; j < maxNodes; ++j) {
            if (allConvos[i].nodes[j].text[0] == '\0') {
                break;
            }
            printf("%s\n", allConvos[i].nodes[j].text);
        }
    }
}
