//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef ___LUBE_H__
#define ___LUBE_H__

#include <lube.h>

typedef struct CommandArgs {
    DWORD       mAttribs;

    char        *pszTemplates;
    char        *pszInputLBO;
    char        *pszInputCLS;
    char        *pszInputPath;
    char        *pszOutputPath;
    char        *pszOutputFile; /* output into one file! */
}   CommandArgs;

typedef enum CommandOptions {
    Command_r_Overwrite         = 0x0001,
    Command_l_ListTemplates     = 0x0002,
    Command_r_Redirect          = 0x0004,
    Command_r_Foreground        = 0x0008,
    Command_e_NoElastos         = 0x0010,
    Command_k_InKernel          = 0x0020,
    Command_k_InSDK             = 0x0040,
    Command_z_UseCustomClassObject     = 0x0080,
    Command_w_HaveSandwichMethod       = 0x0100,
    Command_n_UseNakedMode             = 0x0200,
    Command_u_WeakRef           = 0x0400,
}   CommandOptions;

extern int ParseArgs(int, char **, CommandArgs *);
extern void ListAllTemplates(PLUBEHEADER);

#endif // ___LUBE_H__
