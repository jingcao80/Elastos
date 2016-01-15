//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include <unistd.h>
#include <lube.h>

#ifdef _linux
#define _access access
#endif

#define _MAX_PATH 256

extern const char *g_pszOutputPath;

FILE * g_RedirectFile = NULL;

LubeContext::LubeContext(
    LubeHeader *pHeader, CLSModule *pModule, DWORD dwOptions)
{
    int n;

    assert(NULL != pModule);
    memset(this, 0, sizeof(LubeContext));

    if (pModule->mLibraryCount > 0) {
        m_pFirstLibrary = pModule->mLibraryNames[0];
        m_pLastLibrary = pModule->mLibraryNames[pModule->mLibraryCount - 1];
    }
    for (n = 0; n < pModule->mClassCount; n++) {
        ClassDirEntry *pClass = pModule->mClassDirs[n];
        if (!pClass->mNameSpace
            && !(pClass->mDesc->mAttribs & ClassAttrib_t_generic)) {
            if (!m_pFirstClass) m_pFirstClass = pClass;
            m_pLastClass = pClass;
        }
    }
    for (n = 0; n < pModule->mDefinedInterfaceCount; n++) {
        InterfaceDirEntry *pIntf = pModule->mInterfaceDirs[pModule->mDefinedInterfaceIndexes[n]];
        if (!m_pFirstInterface) m_pFirstInterface = pIntf;
        m_pLastInterface = pIntf;
    }
    for (n = 0; n < pModule->mStructCount; n++) {
        if (!pModule->mStructDirs[n]->mNameSpace) {
            if (!m_pFirstStruct) m_pFirstStruct = pModule->mStructDirs[n];
            m_pLastStruct = pModule->mStructDirs[n];
        }
    }
    for (n = 0; n < pModule->mEnumCount; n++) {
        if (!pModule->mEnumDirs[n]->mNameSpace) {
            if (!m_pFirstEnum) m_pFirstEnum = pModule->mEnumDirs[n];
            m_pLastEnum = pModule->mEnumDirs[n];
        }
    }
    for (n = 0; n < pModule->mConstCount; n++) {
        if (!pModule->mConstDirs[n]->mNameSpace) {
            if (!m_pFirstConst) m_pFirstConst = pModule->mConstDirs[n];
            m_pLastConst = pModule->mConstDirs[n];
        }
    }
    for (n = 0; n < pModule->mAliasCount; n++) {
        if (!pModule->mAliasDirs[n]->mNameSpace) {
            if (!m_pFirstTypedef) m_pFirstTypedef = pModule->mAliasDirs[n];
            m_pLastTypedef = pModule->mAliasDirs[n];
        }
    }
    m_pModule = pModule;
    m_pHeader = pHeader;
    m_dwOptions = dwOptions;
}

int LubeContext::PutString(const char *pszString)
{
    if (NULL != m_pFile) fputs(pszString, m_pFile);
    return LUBE_OK;
}

int LubeContext::PutData(PVOID pvData, int nSize)
{
    if (NULL != m_pFile) fwrite(pvData, nSize, 1, m_pFile);
    return LUBE_OK;
}

int LubeContext::ExecTemplate(const char *pszName)
{
    for (int n = 0; n < m_pHeader->cTemplates; n++) {
        if (!_stricmp(pszName, m_pHeader->ppTemplates[n]->mName)) {
            return this->ExecTemplate(n);
        }
    }
    this->ErrMsg("Can't find template %s.", pszName);
    return LUBE_FAIL;
}

int LubeContext::ExecTemplate(int nIndex)
{
    int nRet;
    LubeTemplate *pOrigTemplate;
    PSTATEDESC pRoot;

    pOrigTemplate = m_pTemplate;
    m_pTemplate = m_pHeader->ppTemplates[nIndex];

    nRet = LUBE_FAIL;
    pRoot = &m_pTemplate->tRoot;
    if (Object_Interface == pRoot->object) {
        if (NULL == m_pInterface) {
            this->ErrMsg("No valid interface exist here.");
            goto ErrorExit;
        }
    }
    else if (Object_Class == pRoot->object) {
        if (NULL == m_pClass) {
            this->ErrMsg("No valid class exist here.");
            goto ErrorExit;
        }
    }
    else {
        assert(Object_Module == pRoot->object);
    }

    nRet = this->ExecStatements(pRoot->pBlockette);

ErrorExit:
    m_pTemplate = pOrigTemplate;
    return nRet;
}

int LubeContext::ExecStatements(PSTATEDESC pDesc)
{
    StatementFn_t fn;

    while (pDesc) {
        fn = GetStatementFunc(pDesc->uStateIndex);
        if ((*fn)(this, pDesc) < 0) return LUBE_FAIL;
        pDesc = pDesc->pNext;
    }
    return LUBE_OK;
}

int LubeContext::EnterFile(const char *pszName, FILE **ppOrigFile)
{
    FILE *pFile;
    char szNameBuf[c_nStrBufSize];
    char szName[_MAX_PATH];

    if (this->ParseStringEmbed(pszName, szNameBuf) < 0) {
        return LUBE_FAIL;
    }
    if (m_dwOptions & LubeOpt_Redirect) {
        *ppOrigFile = m_pFile;
        if (g_RedirectFile) {
            m_pFile = g_RedirectFile;
        }
        else {
            m_pFile = stdout;
        }
        return LUBE_OK;
    }

    if (!(m_dwOptions & LubeOpt_Overwrite)
        && 0 == _access(szNameBuf, 0)) {
        this->WarnMsg("File %s has existed.", szNameBuf);
        return LUBE_FAIL;
    }
    if (!strlen(g_pszOutputPath))
        pFile = fopen(szNameBuf, "w+t");
    else {
        strcpy(szName, g_pszOutputPath);
#ifdef _win32
        strcat(szName, "\\");
#else
        strcat(szName, "/");
#endif
        strcat(szName, szNameBuf);
        pFile = fopen(szName, "w+t");
    }
    if (!pFile) {
        this->ErrMsg("Can't open file %s.", szNameBuf);
        return LUBE_FAIL;
    }
    *ppOrigFile = m_pFile;
    m_pFile = pFile;
    strcpy(m_szFileName, szNameBuf);
    return LUBE_OK;
}

void LubeContext::LeaveFile(FILE *pOrigFile)
{
    if (!(m_dwOptions & LubeOpt_Redirect) && m_pFile) {
        fclose(m_pFile);
    }
    m_pFile = pOrigFile;
}

int LubeContext::ParseStringEmbed(const char *pszSource, char *pDest)
{
    ObjectType object;
    MemberType member;

    while (*pszSource) {
        if (c_nEmbedPrefix != *pszSource) {
            *pDest++ = *pszSource++;
        }
        else {
            pszSource++; // skip prefix
            object = (ObjectType)*pszSource++;
            member = (MemberType)*pszSource++;
            if (this->GetMemberString(object, member, pDest) < 0) {
                return LUBE_FAIL;
            }
            pDest += strlen(pDest);
        }
    }
    *pDest = 0;
    return LUBE_OK;
}

void LubeContext::ErrMsg(
    const char *pMsg, const char *pArg1, const char *pArg2)
{
    fprintf(stderr, "[ERROR] lube (0x0201)");
    if (m_pTemplate) {
        fprintf(stderr, "template %s", m_pTemplate->mName);
    }
    fputs(" : ", stderr);
    fprintf(stderr, pMsg, pArg1, pArg2);
    fputc('\n', stderr);
}

void LubeContext::WarnMsg(
    const char *pMsg, const char *pArg1, const char *pArg2)
{
    fprintf(stderr, "[WARN] lube (0x0202)");
    if (m_pTemplate) {
        fprintf(stderr, "template %s", m_pTemplate->mName);
    }
    fputs(" : ", stderr);
    fprintf(stderr, pMsg, pArg1, pArg2);
    fputc('\n', stderr);
}
