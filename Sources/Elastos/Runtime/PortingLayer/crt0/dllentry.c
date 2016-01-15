

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

typedef int BOOL;
typedef void* HANDLE;
typedef void* HINSTANCE;
typedef unsigned int DWORD;
typedef void* LPVOID;
#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0

BOOL WINAPI DllMain(
        HINSTANCE hinstDLL,
        DWORD dwReason,
        LPVOID lpvReserved);

BOOL WINAPI _DllMainCRTStartup(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved)
{
    BOOL ret;

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
