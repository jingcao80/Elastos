
#ifndef __CHANGE_PATH_H__
#define __CHANGE_PATH_H__


extern void GetCygpath();
extern BOOL HasCygpath();
extern char *Turn2WinPath(char *szUnixPath);
extern void GetUnixpath();
extern char *Turn2UnixPath(const char *szWinPath);

#endif //__CHANGE_PATH_H__
