
#ifndef __LUBC_H__
#define __LUBC_H__

#include "parser.h"

typedef struct CommandArgs {
    DWORD       mAttribs;

    char        *pszSource;
    char        *pszLBO;
    char        *pszSourcePath;
}   CommandArgs;

typedef enum CommandOption {
    Command_o_GenLBO        = 0x0001,
    Command_p_Preview       = 0x0002,
    Command_I_SrcPath       = 0x0004,
    Command_u_ListUserFuncs = 0x0008,
}   CommandOption;

extern int ParseArgs(int, char **, CommandArgs *);
extern void PreviewResult(LubeHeader *);
extern void GenerateLBO(const char *, LubeHeader *);
extern void ListAllUserFuncs();

#endif // __LUBC_H__
