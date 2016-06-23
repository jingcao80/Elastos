
#include "Atomic.h"

int QuasiAtomicCas64(int64_t oldvalue, int64_t newvalue,
    volatile int64_t* addr)
{
    int64_t prev;
    int status;
    do {
        __asm__ __volatile__ ("@ QuasiAtomicCas64\n"
            "ldrexd     %0, %H0, [%3]\n"
            "mov        %1, #0\n"
            "teq        %0, %4\n"
            "teqeq      %H0, %H4\n"
            "strexdeq   %1, %5, %H5, [%3]"
            : "=&r" (prev), "=&r" (status), "+m"(*addr)
            : "r" (addr), "Ir" (oldvalue), "r" (newvalue)
            : "cc");
    } while (__builtin_expect(status != 0, 0));
    return prev != oldvalue;
}
