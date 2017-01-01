
#include <stdio.h>
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <tchar.h>
#include <assert.h>
#ifdef _win32
#include <windows.h>
#endif

//#define DEBUG

#ifdef DEBUG
#define    DP(x...) printf(x)
#else
#define    DP(x...)
#endif

#define BUFSIZE 512
#define PIPENAME "\\\\.\\pipe\\carcparserpipe"

static BOOL s_bHasCygpath;
static char s_szCmdLine[BUFSIZE];
static int s_iInsertPath;

void GetCygpath()
{
    char *cygpath = getenv("CYGPATH");
    if(NULL == cygpath){
        s_bHasCygpath = FALSE;
    } else {
        strcpy(s_szCmdLine, cygpath);
        strcat(s_szCmdLine, " -w ");
        s_iInsertPath = strlen(s_szCmdLine);
        s_bHasCygpath = TRUE;
    }
}

void GetUnixpath()
{
    char *cygpath = getenv("CYGPATH");
    if(NULL == cygpath){
        s_bHasCygpath = FALSE;
    } else {
        strcpy(s_szCmdLine, cygpath);
        strcat(s_szCmdLine, " -u ");
        s_iInsertPath = strlen(s_szCmdLine);
        s_bHasCygpath = TRUE;
    }
}

BOOL HasCygpath()
{
    return s_bHasCygpath;
}

static inline void GenerateCmdLine(char *szUnixPath)
{
    s_szCmdLine[s_iInsertPath] = '\0';
    strcat(s_szCmdLine, szUnixPath);
    strcat(s_szCmdLine, " > ");
    strcat(s_szCmdLine, PIPENAME);
    DP("cmd:%s.\n", s_szCmdLine);
}

char *Turn2WinPath(char *szUnixPath)
{
    BOOL fConnected;
    HANDLE hPipe;
    LPTSTR lpszPipename = TEXT(PIPENAME);
    char *ret = NULL;

    assert(HasCygpath());

    hPipe = CreateNamedPipe(
        lpszPipename,             // pipe name
        PIPE_ACCESS_DUPLEX,       // read/write access
        PIPE_TYPE_BYTE |          // message type pipe
        PIPE_READMODE_BYTE |      // message-read mode
        PIPE_WAIT,                // blocking mode
        1,                        // 1. instances
        BUFSIZE,                  // output buffer size
        BUFSIZE,                  // input buffer size
        NMPWAIT_USE_DEFAULT_WAIT, // client time-out
        NULL);                    // default security attribute

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "[ERROR] carc :err:%d, Create named pipe failed.\n", GetLastError());
        return ret;
    }

    GenerateCmdLine(szUnixPath);

    system(s_szCmdLine);

    fConnected = ConnectNamedPipe(hPipe, NULL);
    if(!fConnected){
        if((GetLastError() == ERROR_PIPE_CONNECTED)
            ||(GetLastError() == ERROR_NO_DATA)){
        }
    } else {
        fprintf(stderr, "[ERROR] carc :err:%d, Create named pipe failed.\n", GetLastError());
        CloseHandle(hPipe);
        return NULL;
    }

    TCHAR chRequest[BUFSIZE];
    DWORD cbBytesRead;
    BOOL fSuccess;
    memset(chRequest, 0, sizeof(chRequest));
    fSuccess = ReadFile(
        hPipe,        // handle to pipe
        chRequest,    // buffer to receive data
        BUFSIZE*sizeof(TCHAR), // size of buffer
        &cbBytesRead, // number of bytes read
        NULL);        // not overlapped I/O

    if (! fSuccess || cbBytesRead == 0){
        fprintf(stderr, "[ERROR] carc :err:%d, Read pipe failed.\n", GetLastError());
    } else {
        int len = strlen(chRequest);
        //Cut the '\n' down
        chRequest[len-1] = '\0';
        DP("read success len:%d. data:%s.\n",strlen(chRequest), chRequest);
        ret = new char[len]; // leak
        strcpy(ret, chRequest);
    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return ret;
}

static inline void GenerateCmdLine2(const char *szWinPath)
{
    s_szCmdLine[s_iInsertPath] = '\0';
    strcat(s_szCmdLine, "'");
    strcat(s_szCmdLine, szWinPath);
    strcat(s_szCmdLine, "'");
    strcat(s_szCmdLine, " > ");
    strcat(s_szCmdLine, PIPENAME);
    DP("cmd:%s.\n", s_szCmdLine);
}

char *Turn2UnixPath(const char *szWinPath)
{
    BOOL fConnected;
    HANDLE hPipe;
    LPTSTR lpszPipename = TEXT(PIPENAME);
    char *ret = NULL;

    assert(HasCygpath());

    hPipe = CreateNamedPipe(
        lpszPipename,             // pipe name
        PIPE_ACCESS_DUPLEX,       // read/write access
        PIPE_TYPE_BYTE |          // message type pipe
        PIPE_READMODE_BYTE |      // message-read mode
        PIPE_WAIT,                // blocking mode
        1,                        // 1. instances
        BUFSIZE,                  // output buffer size
        BUFSIZE,                  // input buffer size
        NMPWAIT_USE_DEFAULT_WAIT, // client time-out
        NULL);                    // default security attribute

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "[ERROR] carc :err:%d, Create named pipe failed.\n", GetLastError());
        return ret;
    }

    GenerateCmdLine2(szWinPath);

    system(s_szCmdLine);

    fConnected = ConnectNamedPipe(hPipe, NULL);
    if(!fConnected){
        if((GetLastError() == ERROR_PIPE_CONNECTED)
            ||(GetLastError() == ERROR_NO_DATA)){
        }
    } else {
        fprintf(stderr, "[ERROR] carc :err:%d, Create named pipe failed.\n", GetLastError());
        CloseHandle(hPipe);
        return NULL;
    }

    TCHAR chRequest[BUFSIZE];
    DWORD cbBytesRead;
    BOOL fSuccess;
    memset(chRequest, 0, sizeof(chRequest));
    fSuccess = ReadFile(
        hPipe,        // handle to pipe
        chRequest,    // buffer to receive data
        BUFSIZE*sizeof(TCHAR), // size of buffer
        &cbBytesRead, // number of bytes read
        NULL);        // not overlapped I/O

    if (! fSuccess || cbBytesRead == 0){
        fprintf(stderr, "[ERROR] carc :err:%d, Read pipe failed.\n", GetLastError());
    } else {
        int len = strlen(chRequest);
        //Cut the '\n' down
        chRequest[len-1] = '\0';
        DP("read success len:%d. data:%s.\n",strlen(chRequest), chRequest);
        ret = new char[len]; // leak
        strcpy(ret, chRequest);
    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return ret;
}
