
#include <lube.h>

#define _MAX_PATH 256

PLUBEHEADER CreateLube()
{
    PLUBEHEADER pLube;

    pLube = new LubeHeader;
    if (!pLube) return NULL;

    pLube->ppTemplates = new LubeTemplate *[c_cMaxTemplates];
    if (!pLube->ppTemplates) {
        delete pLube;
        return NULL;
    }

    pLube->dwMagic = c_dwLubeMagic;
    pLube->cTemplates = 0;
    return pLube;
}

void DestroyStatement(PSTATEDESC pDesc)
{
    PSTATEDESC pTemp;

    while (pDesc) {
        if (pDesc->pBlockette) {
            DestroyStatement(pDesc->pBlockette);
        }
        if (pDesc->pvData) {
            delete [] (char *)pDesc->pvData;
        }
        pTemp = pDesc;
        pDesc = pDesc->pNext;
        delete pTemp;
    }
}

void DestroyLube(PLUBEHEADER pLube)
{
    for (int n = 0; n < pLube->cTemplates; n++) {
        DestroyStatement(pLube->ppTemplates[n]->tRoot.pBlockette);
        delete [] pLube->ppTemplates[n]->mName;
        delete pLube->ppTemplates[n];
    }
    delete [] pLube->ppTemplates;
    delete pLube;
}

void GetTemplateName(const char *pszName, char *pszBuf)
{
    int n = strlen(pszName) - 1;

    while (n >= 0) {
        if (IS_PATH_SEPARATOR(*(pszName + n))) break;
        n--;
    }
    pszName += n + 1;

    while (0 != *pszName && '.' != *pszName) {
        *pszBuf = *pszName;
        pszName++;
        pszBuf++;
    }
    *pszBuf = 0;
}

int CreateTemplate(PLUBEHEADER pLube, const char *pszName)
{
    char szBuf[c_nStrBufSize];
    LubeTemplate *pTemplate;

    if (c_cMaxTemplates <= pLube->cTemplates) return LUBE_FAIL;
    pTemplate = new LubeTemplate;
    if (!pTemplate) return LUBE_FAIL;
    memset(pTemplate, 0, sizeof(LubeTemplate));

    GetTemplateName(pszName, szBuf);
    pTemplate->mName = new char[strlen(szBuf) + 1];
    if (!pTemplate->mName) goto ErrorExit;
    strcpy(pTemplate->mName, szBuf);

    pLube->ppTemplates[pLube->cTemplates] = pTemplate;
    return pLube->cTemplates++;

ErrorExit:
    delete pTemplate;
    return LUBE_FAIL;
}

int FindTemplate(PLUBEHEADER pLube, const char *pszName)
{
    char szBuf[_MAX_PATH];

    GetTemplateName(pszName, szBuf);
    for (int n = 0; n < pLube->cTemplates; n++) {
        if (!_stricmp(szBuf, pLube->ppTemplates[n]->mName)) {
            return n;
        }
    }
    return LUBE_FAIL;
}

IMPL_STATEMENT(Template)(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    return pCtx->ExecTemplate(pDesc->dwExtra);
}
