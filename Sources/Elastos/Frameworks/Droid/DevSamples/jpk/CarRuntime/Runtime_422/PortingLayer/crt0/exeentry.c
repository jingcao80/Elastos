
#include <KD/kd.h>

#ifdef _openkode
int __dso_handle;
#endif

#if defined( _win32) || defined(_wince)
#define MAX_PATH      260
#define MAX_ARGC      128
#define MAX_ARGS_LEN  1024

static char cmdLine[MAX_ARGS_LEN];
static char appname[MAX_PATH];

#if defined(_win32) && defined(_MSVC)
extern void _crt_exe_setpointer();
#endif
#if defined(_win32) && defined(_GNUC)
void __main()
{
}
#endif

int kdProcessGetStartInfoKT(char*, char*);
static int GetMainArgs(int *pArgc, char **pArgv)
{
    int argc = 0;
    char **argv = pArgv;
    char *p = 0;

    kdProcessGetStartInfoKT(appname, cmdLine);

    argv[argc++] = appname;
    if ('\0' == cmdLine[0]) {
        *pArgc = 1;
        return 0;
    }

    p = cmdLine;
    while ((0 != p) && ('\0' != *p) && (argc < MAX_ARGC)) {
        argv[argc++] = p;
        while(*p != '\0' && *p != ' ') p++;//strchr(p, ' ');
        p = ('\0' == *p) ? 0 : p;
        if (0 != p) {
            *p = '\0';
            p++;
        }
    }

    *pArgc = argc;
    return 0;
}
void _crtinit(void);

int mainCRTStartup()
{
    int ret = 0;
    int argc = 0;
    char *argv[MAX_ARGC];

#if defined(_win32) && defined(_MSVC)
    _crt_exe_setpointer();
#endif

    _crtinit();
    GetMainArgs(&argc, argv);
    ret = kdMain(argc, argv);
    kdExit(ret);
    return ret;
}

#else
extern int g_pDllCarCode;
#ifdef _openkode
extern void kdDebugBreakKT();
int kdMainCRTStartup(int argc, const char**argv, int isDebug)
#else
int main(int argc, const char**argv)
#endif
{
    volatile int *for_link_resource_section = &g_pDllCarCode;
    int ret;
    *for_link_resource_section = g_pDllCarCode;
#ifdef _openkode
    if (isDebug) kdDebugBreakKT();
#endif
    ret = kdMain(argc, argv);
    kdExit(ret);
    return ret;
}

#endif
