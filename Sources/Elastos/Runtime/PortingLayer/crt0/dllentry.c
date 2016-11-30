

#if defined(_win32) || defined(_wince)
  #define WINAPI __stdcall
  void _crtinit(void);
  void _crtterm(void);
  static void (*s_dummy_for_link_mark_c)(void) = _crtinit;
#else
  #define WINAPI
#endif

#if defined(_win32) && defined(_MSVC)
extern void _crt_dll_setpointer();
extern void _crt_dllexit();
#endif

#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0

int DllMain(
        void* hinstDLL,
        unsigned int dwReason,
        void* lpvReserved);

int _DllMainCRTStartup(
        void*  hDllHandle,
        unsigned int   dwReason,
        void*  lpreserved)
{
    int ret;

    if (dwReason == DLL_PROCESS_ATTACH) {
#if defined(_win32) && defined(_MSVC)
        _crt_dll_setpointer();
#endif

#if !(defined (_linux) || defined (_openkode))
        _crtinit();
#endif
    }

    ret = DllMain(hDllHandle, dwReason, lpreserved);

    if (dwReason == DLL_PROCESS_DETACH) {
#if defined(_win32) && defined(_MSVC)
        _crt_dllexit();
#endif

#if !(defined (_linux) || defined (_openkode))
        _crtterm();
#endif
    }
    return ret;
}
