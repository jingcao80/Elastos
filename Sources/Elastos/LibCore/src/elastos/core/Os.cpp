
#include "Os.h"
#include "NativeThread.h"
// #include <sys/time.h>
#include <sys/resource.h>
// #include <limits.h>
#include <errno.h>

// #include <cutils/sched_policy.h>
#include <utils/threads.h>

using Elastos::Core::NTHREAD_MIN_PRIORITY;
using Elastos::Core::NTHREAD_NORM_PRIORITY;
using Elastos::Core::NTHREAD_MAX_PRIORITY;

#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

/*
 * Conversion map for "nice" values.
 *
 * We use Android thread priority constants to be consistent with the rest
 * of the system.  In some cases adjacent entries may overlap.
 */
static const int kNiceValues[10] = {
    ANDROID_PRIORITY_LOWEST,                /* 1 (MIN_PRIORITY) */
    ANDROID_PRIORITY_BACKGROUND + 6,
    ANDROID_PRIORITY_BACKGROUND + 3,
    ANDROID_PRIORITY_BACKGROUND,
    ANDROID_PRIORITY_NORMAL,                /* 5 (NORM_PRIORITY) */
    ANDROID_PRIORITY_NORMAL - 2,
    ANDROID_PRIORITY_NORMAL - 4,
    ANDROID_PRIORITY_URGENT_DISPLAY + 3,
    ANDROID_PRIORITY_URGENT_DISPLAY + 2,
    ANDROID_PRIORITY_URGENT_DISPLAY         /* 10 (MAX_PRIORITY) */
};

int os_getThreadPriorityFromSystem()
{
    errno = 0;
    int sysprio = getpriority(PRIO_PROCESS, 0);
    if (sysprio == -1 && errno != 0) {
        // ALOGW("getpriority() failed: %s", strerror(errno));
        return NTHREAD_NORM_PRIORITY;
    }

    int jprio = NTHREAD_MIN_PRIORITY;
    for (int i = 0; i < NELEM(kNiceValues); i++) {
        if (sysprio >= kNiceValues[i]) {
            break;
        }
        jprio++;
    }
    if (jprio > NTHREAD_MAX_PRIORITY) {
        jprio = NTHREAD_MAX_PRIORITY;
    }
    return jprio;
}
