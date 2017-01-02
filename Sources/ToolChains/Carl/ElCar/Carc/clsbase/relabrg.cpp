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

//#ifdef _win32
#include <elatypes.h>
//#else
//#include <elastos.h>
//#endif

#include "clstype.h"
#include "abrgcls.h"

#define RELOC(p, t, b)    p = (t)((int)(p) + b)

void RelocAbridgedCLS(void *pvAbrgCLS)
{
    size_t n, m, nBase;
    AbridgedModuleInfo *pInfo;
    AbridgedClass *pClass;
    AbridgedInterface *pIntf;

    pInfo = (AbridgedModuleInfo *)pvAbrgCLS;
    nBase = (int)pvAbrgCLS;

    if ((size_t)(pInfo->pClasses) > nBase) return; // has relocated

    RELOC(pInfo->pClasses, AbridgedClass *, nBase);
    RELOC(pInfo->pInterfaces, AbridgedInterface *, nBase);

    for (n = 0; n < pInfo->mClassCount; n++) {
        pClass = &pInfo->pClasses[n];
        RELOC(pClass->mInterfaces, AbridgedInterface **, nBase);

        for (m = 0; m < pClass->mInterfaceCount; m++) {
            RELOC(pClass->mInterfaces[m], AbridgedInterface *, nBase);
        }
        RELOC(pClass->pszUunm, char *, nBase);
    }

    for (n = 0; n < pInfo->mInterfaceCount; n++) {
        pIntf = &pInfo->pInterfaces[n];
        RELOC(pIntf->pMethods, AbridgedMethod *, nBase);
        for (m = 0; m < pIntf->mMethodCount; m++) {
            RELOC(pIntf->pMethods[m].pParams, AbridgedParamsInfo *, nBase);
        }
    }
}
