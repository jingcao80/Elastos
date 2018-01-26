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

#include <malloc.h>
#include <stdlib.h>
#include <lube.h>

#ifdef _linux
#define _alloca alloca
#endif

#define ALIGN_BOUND 4

inline int RoundUp4(int n)
{
    return ((n) + ALIGN_BOUND - 1) & ~(ALIGN_BOUND - 1);
}

class CLubeBuffer
{
public:
    int Flat(PLUBEHEADER pLube, char *pBuffer);

private:
    inline ptrdiff_t WriteData(const void *pvData, int nSize);
    inline ptrdiff_t WriteString(const char *pString);

    ptrdiff_t WriteState(PSTATEDESC pDesc);
    ptrdiff_t WriteTemplate(LubeTemplate *);

private:
    PLUBEHEADER         m_pLube;
    char *              m_pBuffer;
    ptrdiff_t           m_nOffset;
};

ptrdiff_t CLubeBuffer::WriteData(const void *pvData, int nSize)
{
    ptrdiff_t nBegin = m_nOffset;

    memcpy(m_pBuffer + m_nOffset, pvData, nSize);
    m_nOffset += RoundUp4(nSize);

    return nBegin;
}

ptrdiff_t CLubeBuffer::WriteString(const char *pString)
{
    return WriteData(pString, strlen(pString) + 1);
}

ptrdiff_t CLubeBuffer::WriteState(PSTATEDESC pDesc)
{
    StateDesc state;

    memcpy(&state, pDesc, sizeof(StateDesc));
    if (state.pBlockette) {
        state.pBlockette = (PSTATEDESC)WriteState(state.pBlockette);
    }
    if (state.pNext) {
        state.pNext = (PSTATEDESC)WriteState(state.pNext);
    }
    if (state.pvData) {
        state.pvData = (PVOID)WriteData(state.pvData, state.uDataSize);
    }

    return WriteData(&state, sizeof(StateDesc));
}

ptrdiff_t CLubeBuffer::WriteTemplate(LubeTemplate *pTemplate)
{
    LubeTemplate tmp;

    memcpy(&tmp, pTemplate, sizeof(LubeTemplate));
    tmp.mName = (char *)WriteString(tmp.mName);
    if (tmp.tRoot.pBlockette) {
        tmp.tRoot.pBlockette = (PSTATEDESC)WriteState(tmp.tRoot.pBlockette);
    }
    return WriteData(&tmp, sizeof(LubeTemplate));
}

int CLubeBuffer::Flat(PLUBEHEADER pLube, char *pBuffer)
{
    int n;
    ptrdiff_t *p;

    m_pLube = pLube;
    m_pBuffer = pBuffer;
    m_nOffset = sizeof(LubeHeader);

    pLube = (PLUBEHEADER)pBuffer;
    memcpy(pLube, m_pLube, sizeof(LubeHeader));

    p = (ptrdiff_t *)_alloca(pLube->cTemplates * sizeof(ptrdiff_t));
    for (n = 0; n < pLube->cTemplates; n++) {
        p[n] = WriteTemplate(pLube->ppTemplates[n]);
    }
    pLube->ppTemplates = (LubeTemplate **)WriteData(p, n * sizeof(ptrdiff_t));

    return (int)m_nOffset;
}

int CalcStatementsSize(PSTATEDESC pDesc)
{
    int nSize = 0;

    while (pDesc) {
        if (pDesc->pBlockette) {
            nSize += CalcStatementsSize(pDesc->pBlockette);
        }
        nSize += sizeof(StateDesc) + RoundUp4(pDesc->uDataSize);
        pDesc = pDesc->pNext;
    }
    return nSize;
}

int CalcTemplateSize(LubeTemplate *pTemplate)
{
    int nSize = sizeof(LubeTemplate);

    nSize += CalcStatementsSize(pTemplate->tRoot.pBlockette);
    nSize += RoundUp4(strlen(pTemplate->mName) + 1);

    return nSize;
}

int CalcLubeSize(PLUBEHEADER pLube)
{
    int n, nSize = sizeof(LubeHeader);

    for (n = 0; n < pLube->cTemplates; n++) {
        nSize += CalcTemplateSize(pLube->ppTemplates[n]);
    }
    nSize += pLube->cTemplates * sizeof(LubeTemplate *);

    return nSize;
}

int SaveLube(PLUBEHEADER pLube, const char *pszName)
{
    FILE *pFile;
    int nSize;
    char *pBuffer;
    CLubeBuffer buffer;

    nSize = CalcLubeSize(pLube);
    pBuffer = (char *)_alloca(nSize);
    if (!pBuffer) return LUBE_FAIL;

    pFile = fopen(pszName, "w+b");
    if (!pFile) return LUBE_FAIL;

    nSize = buffer.Flat(pLube, pBuffer);
    fwrite(pBuffer, nSize, 1, pFile);

    fclose(pFile);
    return LUBE_OK;
}
