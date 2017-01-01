
#include <assert.h>
#include "clsbase.h"
#include <stdio.h>
#include <clsdef.h>

static int s_cModules = 0;
static CLSModule *s_modules[MAX_LIBRARY_NUMBER + 2];

int AddCLSLibrary(CLSModule *pModule)
{
    s_modules[s_cModules++] = pModule;
    return 0;
}

void DestroyAllLibraries()
{
    int n;

    for (n = 0; n < s_cModules; n++) {
        if (s_modules[n]->mAttribs & CARAttrib_inheap)
            DisposeFlattedCLS(s_modules[n]);
        else
            DestroyCLS(s_modules[n]);
    }
    s_cModules = 0;
}

int RetrieveAlias(const char *pszName,
    CLSModule *pModule, BOOL bLibOnly)
{
    int n, m;

    assert(pszName && pModule);

    if (!bLibOnly) {
        n = SelectAliasDirEntry(pszName, pModule);
        if (n >= 0) return n;
    }
    for (n = s_cModules - 1; n >= 0; n--) {
        m = SelectAliasDirEntry(pszName, s_modules[n]);
        if (m >= 0)
            return AliasCopy(s_modules[n], m, pModule, TRUE);
    }

    return -1;
}

int RetrieveConst(const char *pszName,
    CLSModule *pModule, BOOL bLibOnly)
{
    int n, m;

    assert(pszName && pModule);

    if (!bLibOnly) {
        n = SelectConstDirEntry(pszName, pModule);
        if (n >= 0) return n;
    }
    for (n = s_cModules - 1; n >= 0; n--) {
        m = SelectConstDirEntry(pszName, s_modules[n]);
        if (m >= 0)
            return ConstCopy(s_modules[n], m, pModule, TRUE);
    }

    return -1;
}

int RetrieveEnum(const char *pszName, const char *pszNamespaces,
    CLSModule *pModule, BOOL bLibOnly)
{
    int n, m;

    assert(pszName && pModule);

    if (!bLibOnly) {
        n = SelectEnumDirEntry(pszName, pszNamespaces, pModule);
        if (n >= 0) return n;
    }
    for (n = s_cModules - 1; n >= 0; n--) {
        m = SelectEnumDirEntry(pszName, pszNamespaces, s_modules[n]);
        if (m >= 0)
            return EnumCopy(s_modules[n], m, pModule, TRUE);
    }

    return -1;
}

int RetrieveStruct(const char *pszName,
    CLSModule *pModule, BOOL bLibOnly)
{
    int n, m;

    assert(pszName && pModule);

    if (!bLibOnly) {
        n = SelectStructDirEntry(pszName, pModule);
        if (n >= 0) return n;
    }
    for (n = s_cModules - 1; n >= 0; n--) {
        m = SelectStructDirEntry(pszName, s_modules[n]);
        if (m >= 0)
            return StructCopy(s_modules[n], m, pModule, TRUE);
    }

    return -1;
}

int RetrieveInterface(const char *pszName, const char *pszNamespaces,
    CLSModule *pModule, BOOL bLibOnly)
{
    int n, m;

    assert(pszName && pModule);

    if (!bLibOnly) {
        n = SelectInterfaceDirEntry(pszName, pszNamespaces, pModule);
        if (n >= 0) return n;
    }
    for (n = s_cModules - 1; n >= 0; n--) {
        m = SelectInterfaceDirEntry(pszName, pszNamespaces, s_modules[n]);
        if (m >= 0)
            return InterfaceCopy(s_modules[n], m, pModule, TRUE);
    }

    return -1;
}

int RetrieveMethod(const char *pszName,
    CLSModule *pModule,
    const ClassInterface *pDesc)
{
    assert(pszName && pDesc);

    return SelectInterfaceMethod(pszName,
                                pModule->mInterfaceDirs[pDesc->mIndex]->mDesc);
}

int RetrieveIdentifyType(const char *pszName, const char *pszNamespaces,
    CLSModule *pModule, TypeDescriptor *pType, BOOL bLibOnly)
{
    int n;
    TypeDescriptor type;

    assert(pszName && pModule && pType);

    if (!bLibOnly) {
        if (GetIdentifyType(pszName, pszNamespaces, pModule, pType) == CLS_NoError)
            return 0;
    }
    memset(&type, 0, sizeof(TypeDescriptor));

    for (n = s_cModules - 1; n >= 0; n--) {
        if (GetIdentifyType(pszName, pszNamespaces, s_modules[n], &type) == CLS_NoError) {
            return TypeCopy(s_modules[n], &type, pModule, pType, TRUE);
        }
    }

    return -1;
}

int RetrieveClass(const char *pszName, const char *pszNamespace,
    CLSModule *pModule, BOOL bLibOnly)
{
    int n, m;

    assert(pszName && pModule);

    if (!bLibOnly) {
        n = SelectClassDirEntry(pszName, pszNamespace, pModule);
        if (n >= 0) return n;
    }
    for (n = s_cModules - 1; n >= 0; n--) {
        m = SelectClassDirEntry(pszName, pszNamespace, s_modules[n]);
        if (m >= 0)
            return ClassCopy(s_modules[n], m, pModule, TRUE);
    }

    return -1;
}

int MergeCLS(const CLSModule *pSrc, CLSModule *pDest)
{
    int n;

    for (n = 0; n < pSrc->mEnumCount; n++) {
        if (EnumCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    for (n = 0; n < pSrc->mStructCount; n++) {
        if (StructCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    for (n = 0; n < pSrc->mAliasCount; n++) {
        if (AliasCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    for (n = 0; n < pSrc->mInterfaceCount; n++) {
        if (InterfaceCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    for (n = 0; n < pSrc->mClassCount; n++) {
        if (ClassCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    return 0;
}

int CopyCLS(const CLSModule *pSrc, CLSModule *pDest)
{
    int n;

    memcpy(pDest->mMagic, pSrc->mMagic, MAGIC_STRING_LENGTH);
    pDest->mMajorVersion = pSrc->mMajorVersion;
    pDest->mMinorVersion = pSrc->mMinorVersion;
    pDest->mCLSModuleVersion = pSrc->mCLSModuleVersion;
    pDest->mSize = pSrc->mSize;

    if (pSrc->mUunm) {
        pDest->mUunm = new char[strlen(pSrc->mUunm)+1];
        strcpy(pDest->mUunm, pSrc->mUunm);
    }
    else {
        pDest->mUunm = NULL;
    }

    memcpy(&pDest->mUuid, &pSrc->mUuid, sizeof(pSrc->mUuid));
    pDest->mAttribs = pSrc->mAttribs;
    if (pSrc->mName) {
        pDest->mName = new char[strlen(pSrc->mName)+1];
        strcpy(pDest->mName, pSrc->mName);
    }
    else {
        pDest->mName = NULL;
    }

    if (pSrc->mServiceName) {
        pDest->mServiceName = new char[strlen(pSrc->mServiceName)+1];
        strcpy(pDest->mServiceName, pSrc->mServiceName);
    }
    else {
        pDest->mServiceName = NULL;
    }

    pDest->mDefinedInterfaceCount = 0;
    for (n = 0; n < pSrc->mDefinedInterfaceCount; n++) {
        pDest->mDefinedInterfaceIndexes[pDest->mDefinedInterfaceCount++] = pSrc->mDefinedInterfaceIndexes[n];
    }

    pDest->mLibraryCount = 0;
    for (n = 0; n < pSrc->mLibraryCount; n++) {
        pDest->mLibraryNames[n] = new char[strlen(pSrc->mLibraryNames[n])+1];
        strcpy(pDest->mLibraryNames[n], pSrc->mLibraryNames[n]);
        pDest->mLibraryCount++;
    }

    pDest->mArrayCount = 0;
    for (n = 0; n < pSrc->mArrayCount; n++) {
        if (ArrayXCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    pDest->mConstCount = 0;
    for (n = 0; n < pSrc->mConstCount; n++) {
        if (ConstCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    pDest->mEnumCount = 0;
    for (n = 0; n < pSrc->mEnumCount; n++) {
        if (EnumCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    pDest->mStructCount = 0;
    for (n = 0; n < pSrc->mStructCount; n++) {
        if (StructXCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    pDest->mAliasCount = 0;
    for (n = 0; n < pSrc->mAliasCount; n++) {
        if (AliasXCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    pDest->mInterfaceCount = 0;
    for (n = 0; n < pSrc->mInterfaceCount; n++) {
        if (InterfaceXCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    pDest->mClassCount = 0;
    for (n = 0; n < pSrc->mClassCount; n++) {
        if (ClassXCopy(pSrc, n, pDest, FALSE) < 0) return -1;
    }

    return 0;
}
