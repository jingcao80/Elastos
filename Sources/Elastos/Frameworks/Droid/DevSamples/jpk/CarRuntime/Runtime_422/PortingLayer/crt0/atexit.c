
/*
 * atexit.c :
 *  Used in win32 platform to solve global object destructor.
 */

/*
 * same as enum in atexit.h of CRuntime.
 */
enum __atexit_types
{
  __et_atexit,
  __et_onexit,
  __et_cxa
};

extern int __register_exitproc(int, void (*fn) (void), void *, void *);
extern __cxa_atexit (void (__cdecl *func )( void ), void *, void *);
extern void __cxa_finalize(void *);
extern void * _crt_getpointer();
extern int _crt_is_in_exe();

int atexit(
    void (__cdecl *func )( void ))
{
    if (_crt_is_in_exe()) {
        return __register_exitproc(__et_atexit, func, 0, 0);
    }

    return __cxa_atexit (func, 0, _crt_getpointer());
}

void _crt_dllexit()
{
    __cxa_finalize(_crt_getpointer());
}

