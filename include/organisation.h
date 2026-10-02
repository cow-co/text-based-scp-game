/**
 * @file organisation.h
 */

#ifndef ORGANISATION_H
#define ORGANISATION_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    LOW = 0,
    MODERATE = 50,
    HIGH = 100
} Reputation;

typedef struct {
  uint16_t operational;
  uint16_t wounded;
  uint16_t dead;
} Personnel;

/**
 * @brief Inflicts the given numbers of casualties on the given personnel entity.
 * 
 * @detail Ensures that the number of active personnel does not wrap around
 * if losses exceed the number of personnel remaining. Inflicts fatalities before wounded.
 * So if we start with 10 operational personnel, and inflict 9 dead and 5 wounded, then 
 * we will end up with 0 operational, 9 dead, and 1 wounded.
 */
void inflict_casualties(Personnel* const personnel, const uint16_t dead, const uint16_t wounded);

/**
 * @brief Regenerates active personnel into the personnel pool.
 *
 * @detail If fromWounded is set, then the personnel are regenerated from the pool of 
 * injured personnel, and the number regenerated is min(count, number of wounded).
 */
void regenerate(Personnel* const personnel, const uint16_t count, const bool fromWounded);

typedef struct {
  unsigned id;
  Personnel personnel;
  bool active;
  char name[64];
} MTF;


extern MTF allMTFs[32];

/**
 * @brief Reads the given data file, containing all the available MTFs, and parses them into the `allMTFs` array.
 * @detail Of course, this only allows up to size(allMTFs) items in the TOML array.
 * @return EOF for failure, 0 otherwise
 */
int parse_mtf_data(const char* const filePath);


typedef struct {
    unsigned budget;
    uint8_t reputation;
    unsigned scps[32];   // IDs of contained SCPs
    const char playerName[64];
    unsigned mtfs[8];
} Organisation;

/**
 * @brief Displays all the information about an organisation.
 * 
 * @detail Currently mostly for debug purposes, but may later be extended to 
 * be used for actual gameplay.
 */
void print_organisation_details(const Organisation* const org);

/**
 * @brief This function reads in a data file containing the initial setup of the player's Organisation, and fills the data into an Organisation object.
 * 
 * @return EOF if failure, 0 otherwise
 * 
 * @usage if (parse_organisation_data("file", &org) == EOF) { <handle error> } else {<continue>}
 */
int parse_organisation_data(const char* const filePath, Organisation* const outOrg);

/**
 * @brief Updates changes the reputation of the organisation by the given amount.
 * 
 * @detail Note: maximum is 100 rep, so this will clamp to min(100, rep+delta)
 */
void change_reputation(Organisation* const org, const int16_t delta);
#endif
