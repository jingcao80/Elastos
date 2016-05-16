
typedef void (__cdecl *PF)(void);

#if defined(__GNUC__) && !defined(_android)
#include <KD/kd.h>

extern PF __CTOR_LIST__[];
extern PF __DTOR_LIST__[];

void initterm(PF *pfbegin)
{
    // constructor and destructor should begin with 0xfffffff
    // and end of 0
    // assert(*pfbegin == (PF)-1);

    if (*pfbegin == (PF)-1) {
        pfbegin++;
    }
    else {
        kdAssert(0);
    }

    while (*pfbegin) {
        (**pfbegin)();
        pfbegin++;
    }
}

void _crtinit()
{
    initterm(__CTOR_LIST__);
}

void _crtterm()
{
    initterm(__DTOR_LIST__);
}

#elif defined(_win32) || defined(_wince)


extern PF __xi_a[], __xi_z[]; /* C initializers */
extern PF __xc_a[], __xc_z[]; /* C++ initializers */
extern PF __xp_a[], __xp_z[]; /* C pre-terminators */
extern PF __xt_a[], __xt_z[]; /* C terminators */

static void initterm(PF *pfbegin, PF *pfend)
{
    while (pfbegin < pfend) {
        if (*pfbegin != 0) {
            (**pfbegin)();
        }
        pfbegin++;
    }
}

void _crtinit()
{
    initterm(__xc_a, __xc_z);
}

void _crtterm()
{
    initterm(__xt_a, __xt_z);
}

#pragma data_seg(".CRT$XCA", "DATA")
PF __xc_a[] = { 0 };                    /* C++ initializers */

#pragma data_seg(".CRT$XCZ", "DATA")
PF __xc_z[] = { 0 };

#pragma data_seg(".CRT$XTA", "DATA")
PF __xt_a[] = { 0 };                    /* C terminators */

#pragma data_seg(".CRT$XTZ", "DATA")
PF __xt_z[] = { 0 };

#endif

