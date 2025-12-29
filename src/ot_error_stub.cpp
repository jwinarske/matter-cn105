// Minimal stub to satisfy link: otThreadErrorToString
// This should be replaced by OpenThread's implementation once OT config is corrected.
#include <openthread/error.h>

extern "C" const char *otThreadErrorToString(otError /*aError*/) {
    return "OT error";
}
