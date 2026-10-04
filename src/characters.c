#include "characters.h"
#include "util.h"
#include "tomlc17.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


SCP allSCPs[32] = {};
Conversation allConvos[512] = {};

int parse_scps(const char* const filePath) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse SCPs file!\n");
        return EOF;
    }
    
    const toml_datum_t scpsDatum = toml_get(tomlResult.toptab, "scp");
    
    if (scpsDatum.type != TOML_ARRAY) {
        log_error("SCPs list should be an array!");
        return EOF;
    }
    
    if (scpsDatum.u.arr.size > sizeof(allSCPs) / sizeof(allSCPs[0])) {
        log_error("SCPs list is too long!\n");
        return EOF;
    }

    for (size_t i = 0; i < scpsDatum.u.arr.size; ++i) {
        const toml_datum_t scpDatum = scpsDatum.u.arr.elem[i];
        if (scpDatum.type != TOML_TABLE) {
            log_error("SCP array elements should be tables!\n");
            return EOF;
        }
        const toml_datum_t scpIDDatum = toml_get(scpDatum, "id");
        const toml_datum_t scpNameDatum = toml_get(scpDatum, "name");
        const toml_datum_t scpDescriptionDatum = toml_get(scpDatum, "description");
        const toml_datum_t scpObjClassDatum = toml_get(scpDatum, "objectClass");
        
        if (scpIDDatum.type != TOML_INT64 || scpIDDatum.u.int64 <= 0) {
            log_error("SCP IDs must be strictly positive integers!\n");
            return EOF;
        }
        if (scpNameDatum.type != TOML_STRING) {
            log_error("SCP names must be strings!\n");
            return EOF;
        }
        if (scpDescriptionDatum.type != TOML_STRING) {
            log_error("SCP descriptions must be strings!\n");
            return EOF;
        }
        if (scpObjClassDatum.type != TOML_INT64 || scpObjClassDatum.u.int64 < SAFE || scpObjClassDatum.u.int64 > KETER) {
            log_error("SCP Object class should be an integer corresponding to one of the object class enum constants!\n");
            return EOF;
        }

        if (scpNameDatum.u.str.len >= sizeof(allSCPs[i].name)) {
            log_error("SCP Name is too long!\n");
            return EOF;
        }
        if (scpDescriptionDatum.u.str.len >= sizeof(allSCPs[i].description)) {
            log_error("SCP description is too long!\n");
            return EOF;
        }

        allSCPs[i].id = scpIDDatum.u.int64;
        strncpy(allSCPs[i].name, scpNameDatum.u.str.ptr, scpNameDatum.u.str.len);
        strncpy(allSCPs[i].description, scpDescriptionDatum.u.str.ptr, scpDescriptionDatum.u.str.len);
        allSCPs[i].objectClass = scpObjClassDatum.u.int64;
    }
    return 0;
}

void print_scps() {
    size_t countSCPs = sizeof(allSCPs) / sizeof(allSCPs[0]);
    printf("SCPs:\n");
    for (size_t i = 0; i < countSCPs; ++i) {
        char* class = "SAFE";
        if (allSCPs[i].id == 0) {
            return;
        }
        if (allSCPs[i].objectClass == EUCLID) {
            class = "EUCLID";
        } else if (allSCPs[i].objectClass == KETER) {
            class = "KETER";
        }
        printf(
            "%s\n\tID: %u\n\tObject Class: %s\n\tDescription: %s\n", 
            allSCPs[i].name,
            allSCPs[i].id,
            class,
            allSCPs[i].description
        );
    }
}

int parse_convo_node(const toml_datum_t* const tomlNode, ConvoNode* const outNode) {
    const toml_datum_t nodeIDDatum = toml_get(*tomlNode, "id");
    const toml_datum_t nodeTextDatum = toml_get(*tomlNode, "text");
    if (nodeIDDatum.type != TOML_INT64 || nodeIDDatum.u.int64 <= 0) {
        log_error("Node ID must be an integer > 0!\n");
        return EOF;
    }
    if (nodeTextDatum.type != TOML_STRING || nodeTextDatum.u.str.len >= sizeof(outNode->text)) {
        log_error("Node text is not a string, or is too long!\n");
        return EOF;
    }
    outNode->id = nodeIDDatum.u.int64;
    strncpy(outNode->text, nodeTextDatum.u.str.ptr, nodeTextDatum.u.str.len);
    
    const toml_datum_t nodeOptionsDatum = toml_get(*tomlNode, "option");
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
        if (optionTextDatum.type != TOML_STRING || optionTextDatum.u.str.len >= sizeof(outNode->options[i].text)) {
            log_error("Node option text is not a string, or is too long!\n");
            return EOF;
        }
        if (optionLinkDatum.type != TOML_INT64 || optionLinkDatum.u.int64 <= 0) {
            log_error("Option linkmust be an integer > 0!\n");
            return EOF;
        }
        strncpy(outNode->options[i].text, optionTextDatum.u.str.ptr, optionTextDatum.u.str.len);
        outNode->options[i].linksTo = optionLinkDatum.u.int64;
    }
    return EOF;
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
        printf("Processing convo with ID %ld\n", convoIDDatum.u.int64);
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

int parse_people(const char* const filePath) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse persons file!\n");
        return EOF;
    }

    const toml_datum_t personsDatum = toml_get(tomlResult.toptab, "person");
    if (personsDatum.type != TOML_ARRAY) {
        log_error("Persons list must be an array!\n");
        return EOF;
    }

    // TODO Parse array
    return 0;
}
