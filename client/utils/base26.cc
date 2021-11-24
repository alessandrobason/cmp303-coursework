#include "base26.h"

#include <errno.h>
#include <stdlib.h>
#include <tracelog.h>
#include "types.h"

static char *str2int(int &i, const char *str);

std::string ipToBase26(std::string &ip) {
    std::string result;

    const char *end = ip.c_str();
    do {
        int val = 0;
        end = str2int(val, end);
        if(end) {
            // int to base26
            int v = val, r;
            do {
                v = val / 26;
                r = val % 26;
                result += ('A' + r);
                val = v;
            } while(val != 0);
            if (*end == '.') {
                result += '.';
                end++;
            }
        }
    } while(end && *end);

    return result;
}

std::string base26ToIp(std::string &b26) {
    std::string result;

    size_t from = 0;
    size_t point = 0;

    bool is_first = true;
    int res = 0;
    for (auto &c : b26) {
        if (c == '.') {
            is_first = true;
            result += std::to_string(res);
            result += '.';
            res = 0;
        }
        else {
            int v = c - 'A';
            if (!is_first) v *= 26;
            is_first = !is_first;
            res += v;
        }
    }

    result += std::to_string(res);

    return result;
}

static char *str2int(int &i, const char *str) {
    char *end;
    long res = strtol(str, &end, 0);
    if((errno == ERANGE && res == LONG_MAX) ||
       (errno == ERANGE && res == LONG_MIN) ||
       res > INT_MAX ||
       res < INT_MIN ||
       *str == '\0') 
    {
        return NULL;
    }
    i = res;
    return end;
}