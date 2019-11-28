#include <stdint.h>
#include <avr/pgmspace.h>

#include "gitdefs.h"
#include "gitinfo.h"


#ifndef GIT_HASH_HEX
#define GIT_HASH_HEX 0x00000000
#endif

#ifndef GIT_HASH_STR
#warning Missing Git commit info
#define GIT_HASH_STR "00000000"
#endif

#ifndef GIT_BRANCH_STR
#warning Missing Git branch info
#define GIT_BRANCH_STR  "unknown-branch"
#endif

#ifndef GIT_VERSION_STR
#warning Missing version info
#define GIT_VERSION_STR  "unknown-version"
#endif

#ifndef GIT_BUILD_NR
#warning Missing build number
#define GIT_BUILD_NR  0
#endif

const uint16_t Git_BuildNr      = GIT_BUILD_NR;
const uint32_t Git_Hash32       = GIT_HASH_HEX;
const char     Git_HashStr[]    PROGMEM = GIT_HASH_STR;
const char     Git_BranchStr[]  PROGMEM = GIT_BRANCH_STR;
const char     Git_VersionStr[] PROGMEM = GIT_VERSION_STR;
