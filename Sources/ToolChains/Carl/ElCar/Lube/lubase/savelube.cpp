//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

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
    inline int WriteData(const void *pvData, int nSize);
    inline int WriteString(const char *pString);

    int WriteState(PSTATEDESC pDesc);
    int WriteTemplate(LubeTemplate *);

private:
    PLUBEHEADER         m_pLube;
    char *              m_pBuffer;
    int                 m_nOffset;
};

int CLubeBuffer::WriteData(const void *pvData, int nSize)
{
    int nBegin = m_nOffset;

    memcpy(m_pBuffer + m_nOffset, pvData, nSize);
    m_nOffset += RoundUp4(nSize);

    return nBegin;
}

int CLubeBuffer::WriteString(const char *pString)
{
    return WriteData(pString, strlen(pString) + 1);
}

int CLubeBuffer::WriteState(PSTATEDESC pDesc)
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

int CLubeBuffer::WriteTemplate(LubeTemplate *pTemplate)
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
    int *p, n;

    m_pLube = pLube;
    m_pBuffer = pBuffer;
    m_nOffset = sizeof(LubeHeader);

    pLube = (PLUBEHEADER)pBuffer;
    memcpy(pLube, m_pLube, sizeof(LubeHeader));

    p = (int *)_alloca(pLube->cTemplates * sizeof(int));
    for (n = 0; n < pLube->cTemplates; n++) {
        p[n] = WriteTemplate(pLube->ppTemplates[n]);
    }
    pLube->ppTemplates = (LubeTemplate **)WriteData(p, n * sizeof(int));

    return m_nOffset;
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
