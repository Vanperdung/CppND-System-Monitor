#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds)
{
    int hour = (uint64_t)(seconds / 3600U);
    uint8_t minute = (uint8_t)((uint64_t)(seconds % 3600U) / 60U);
    uint8_t second = (uint8_t)(seconds % 60U);

    char str[10] = {0};
    sprintf(str, "%02d:%02d:%02d", hour, minute, second);

    return string(str);
}