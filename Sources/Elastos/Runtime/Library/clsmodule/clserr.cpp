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

#include <stdio.h>
#include <clsbase.h>

int sCLSErrorNumber;
char sCLSErrorName[32];
char sCLSErrorMessage[256];

int CLSLastError()
{
    return (sCLSErrorNumber < 0 ? sCLSErrorNumber : CLS_NoError);
}

const char* CLSLastErrorMessage()
{
    static char sMsg[300];

    if (sCLSErrorNumber >= 0) return "None";

    if (0 == sCLSErrorMessage[0]) {
        strcpy(sMsg, sCLSErrorName);
    }
    else {
        strcpy(sMsg, sCLSErrorName);
        strcat(sMsg, "(");
        strcat(sMsg, sCLSErrorMessage);
        strcat(sMsg, ")");
    }

    sCLSErrorMessage[0] = 0;
    sCLSErrorNumber = CLS_NoError;

    return sMsg;
}

void ExtraMessage(
    /* [in] */ const char* msg1,
    /* [in] */ const char* msg2,
    /* [in] */ const char* msg3,
    /* [in] */ const char* msg4)
{
    BOOL hasColon = FALSE;

    sCLSErrorMessage[0] = 0;

    if (msg1) {
        strcat(sCLSErrorMessage, msg1);
        hasColon = TRUE;
    }
    if (msg2) {
        if (hasColon) strcat(sCLSErrorMessage, "::");
        strcat(sCLSErrorMessage, msg2);
        hasColon = TRUE;
    }
    if (msg3) {
        if (hasColon) strcat(sCLSErrorMessage, "::");
        strcat(sCLSErrorMessage, msg3);
        hasColon = TRUE;
    }
    if (msg4) {
        if (hasColon) strcat(sCLSErrorMessage, "::");
        strcat(sCLSErrorMessage, msg4);
    }
}
