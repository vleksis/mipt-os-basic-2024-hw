#include <stdint.h>
#include <string.h>

#include "ieee754_clf.h"

#define SIGN_BIT (1ull << 63)
#define EXP_BITS (((1ull << 11) - 1) << 52)
#define FRACTION_BITS ((1ull << 52) - 1)

float_class_t classify(double x) {
    uint64_t bits;
    memcpy(&bits, &x, sizeof(x));
    uint64_t sign = bits & SIGN_BIT;
    uint64_t exp = bits & EXP_BITS;
    uint64_t fraction = bits & FRACTION_BITS;

    if (exp == EXP_BITS && fraction != 0) {
        return NaN;
    }

    if (sign == SIGN_BIT) {
        if (exp == EXP_BITS) {
            return MinusInf;
        }
        if (exp == 0 && fraction == 0) {
            return MinusZero;
        }
        if (exp == 0) {
            return MinusDenormal;
        }
        return MinusRegular;
    }

    if (exp == EXP_BITS) {
        return Inf;
    }
    if (exp == 0 && fraction == 0) {
        return Zero;
    }
    if (exp == 0) {
        return Denormal;
    }

    return Regular;
}
