
#ifndef __ELASTOS_CORE_OS_H__
#define __ELASTOS_CORE_OS_H__

/*
 * Returns the thread priority for the current thread by querying the system.
 * This is useful when attaching a thread through JNI.
 *
 * Returns a value from 1 to 10 (compatible with java.lang.Thread values).
 */
int os_getThreadPriorityFromSystem();

#endif //__ELASTOS_CORE_OS_H__
