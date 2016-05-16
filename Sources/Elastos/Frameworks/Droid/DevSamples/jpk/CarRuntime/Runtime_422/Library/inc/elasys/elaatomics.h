#ifndef _ELAATOMICS_H
#define _ELAATOMICS_H

#ifdef __cplusplus
extern "C" {
#endif

extern int atomic_cmpxchg(int old, int _new, volatile int *ptr);
extern int atomic_swap(int _new, volatile int *ptr);

extern int atomic_inc(volatile int *ptr);
extern int atomic_dec(volatile int *ptr);
extern int atomic_add(int value, volatile int* ptr);
extern int atomic_and(int value, volatile int* ptr);
extern int atomic_or(int value, volatile int *ptr);

#ifdef __cplusplus
}
#endif

#endif /* _ELAATOMICS_H */
