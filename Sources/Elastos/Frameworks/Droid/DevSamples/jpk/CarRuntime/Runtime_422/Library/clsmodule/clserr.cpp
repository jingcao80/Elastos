//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdio.h>
#include <clsbase.h>

namespace ErrRec {
    int nNumber;
    char szName[32];
    char szMessage[256];
}

int CLSLastError()
{
    return (ErrRec::nNumber < 0 ? ErrRec::nNumber:CLS_NoError);
}

const char *CLSLastErrorMessage()
{
    static char s_szMsg[300];

    if (ErrRec::nNumber >= 0) return "None";

    if (0 == ErrRec::szMessage[0]) {
        strcpy(s_szMsg, ErrRec::szName);
    }
    else {
        strcpy(s_szMsg, ErrRec::szName);
        strcat(s_szMsg, "(");
        strcat(s_szMsg, ErrRec::szMessage);
        strcat(s_szMsg, ")");
//        sprintf(s_szMsg, "%s(%s)", ErrRec::szName, ErrRec::szMessage);
    }

    ErrRec::szMessage[0] = 0;
    ErrRec::nNumber = CLS_NoError;

    return s_szMsg;
}

void ExtraMessage(
    const char *pszMsg1, const char *pszMsg2,
    const char *pszMsg3, const char *pszMsg4)
{
    BOOL bColon = FALSE;

    ErrRec::szMessage[0] = 0;

    if (pszMsg1) {
        strcat(ErrRec::szMessage, pszMsg1);
        bColon = TRUE;
    }
    if (pszMsg2) {
        if (bColon) strcat(ErrRec::szMessage, "::");
        strcat(ErrRec::szMessage, pszMsg2);
        bColon = TRUE;
    }
    if (pszMsg3) {
        if (bColon) strcat(ErrRec::szMessage, "::");
        strcat(ErrRec::szMessage, pszMsg3);
        bColon = TRUE;
    }
    if (pszMsg4) {
        if (bColon) strcat(ErrRec::szMessage, "::");
        strcat(ErrRec::szMessage, pszMsg4);
    }
}
