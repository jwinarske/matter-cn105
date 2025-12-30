// Minimal stub to satisfy link: otThreadErrorToString, otPlatLog, otPlatUartSend, and platformUartPanic
// This should be replaced by OpenThread's implementation once OT config is corrected.
#include <openthread/error.h>
#include <openthread/platform/logging.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

extern "C" const char *otThreadErrorToString(otError /*aError*/) {
    return "OT error";
}

extern "C" void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...) {
    // Platform-specific logging implementation
    // This stub forwards to standard output
    (void)aLogLevel;
    (void)aLogRegion;

    va_list args;
    va_start(args, aFormat);
    vprintf(aFormat, args);
    va_end(args);
    printf("\n");
}

extern "C" void otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength) {
    // Platform-specific UART send for Spinel protocol
    // This stub outputs to stdout
    if (aBuf && aBufLength > 0) {
        fwrite(aBuf, 1, aBufLength, stdout);
        fflush(stdout);
    }
}

extern "C" void platformUartPanic(void) {
    // Called in panic mode to prepare UART for raw output
    // This is a no-op stub
}
