//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
