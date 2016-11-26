//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <clsbase.h>

int IsValidCLS(CLSModule *pModule, int nSize, const char *pszName)
{
    if (pModule->mCLSModuleVersion != CLSMODULE_VERSION) {
        ExtraMessage("CLSModule version is incompatable", pszName, "should be recompiled and relinked");
        _ReturnError(CLSError_CLSModuleVersion);
    }
    if (memcmp(pModule->mMagic, MAGIC_STRING, MAGIC_STRING_LENGTH)) {
        _ReturnError(CLSError_FormatMagic);
    }
    if (nSize < (int)sizeof(CLSModule) || pModule->mSize < nSize) {
        _ReturnError (CLSError_FormatSize);
    }
    _ReturnError (CLS_NoError);
}

void DeleteFileDirEntry(FileDirEntry* pFileDir)
{
    assert(pFileDir != NULL);

    if (pFileDir->mPath) delete pFileDir->mPath;
    delete pFileDir;
}

void DeleteKeyValuePair(KeyValuePair* pPair)
{
    assert(pPair != NULL);

    if (pPair->mKey) delete pPair->mKey;
    if (pPair->mValue) delete pPair->mValue;
    delete pPair;
}

void DeleteAnnotation(AnnotationDescriptor* pDesc)
{
    assert(pDesc != NULL);

    if (pDesc->mName) delete pDesc->mName;
    if (pDesc->mNameSpace) delete pDesc->mNameSpace;

    for (int n = 0; n < pDesc->mKeyValuePairCount; n++) {
        DeleteKeyValuePair(pDesc->mKeyValuePairs[n]);
    }
    delete [] pDesc->mKeyValuePairs;
    delete pDesc;
}

ClassInterface *NewClassInterface(USHORT index)
{
    ClassInterface *pClassInterface;

    pClassInterface = new ClassInterface;
    if (!pClassInterface) return NULL;
    memset(pClassInterface, 0, sizeof(ClassInterface));
    pClassInterface->mIndex = index;

    return pClassInterface;
}

void DeleteClassInterface(ClassInterface *pClassInterface)
{
    assert(pClassInterface != NULL);

    delete pClassInterface;
}

FileDirEntry *NewFileDirEntry(const char *pszPath)
{
    FileDirEntry *pFile;

    assert(pszPath != NULL);

    pFile = new FileDirEntry;
    if (!pFile) return NULL;

    pFile->mPath = new char[strlen(pszPath) + 1];
    if (!pFile->mPath) goto ErrorExit;
    strcpy(pFile->mPath, pszPath);
    return pFile;

ErrorExit:
    delete pFile;
    return NULL;
}

ClassDirEntry *NewClassDirEntry(const char *pszName, const char *pszNamespace)
{
    ClassDirEntry *pClass;

    assert(pszName != NULL);

    pClass = new ClassDirEntry;
    if (!pClass) return NULL;
    memset(pClass, 0, sizeof(ClassDirEntry));

    pClass->mDesc = new ClassDescriptor;
    if (!pClass->mDesc) goto ErrorExit;
    memset(pClass->mDesc, 0, sizeof(ClassDescriptor));

    pClass->mDesc->mAnnotations = \
        new AnnotationDescriptor *[MAX_ANNOTATION_NUMBER];
    if (!pClass->mDesc->mAnnotations) goto ErrorExit;

    pClass->mDesc->mInterfaces = \
        new ClassInterface *[MAX_CLASS_INTERFACE_NUMBER];
    if (!pClass->mDesc->mInterfaces) goto ErrorExit;

    pClass->mDesc->mAggrIndexes = new USHORT[MAX_CLASS_ASPECT_NUMBER];
    if (!pClass->mDesc->mAggrIndexes) goto ErrorExit;

    pClass->mDesc->mAspectIndexes = new USHORT[MAX_CLASS_ASPECT_NUMBER];
    if (!pClass->mDesc->mAspectIndexes) goto ErrorExit;

    pClass->mDesc->mClassIndexes = new USHORT[MAX_AGGRCLASSE_OF_ASPECT_NUMBER];
    if (!pClass->mDesc->mClassIndexes) goto ErrorExit;

    pClass->mName = new char[strlen(pszName) + 1];
    if (!pClass->mName) goto ErrorExit;
    strcpy(pClass->mName, pszName);
    if (pszNamespace != NULL) {
        pClass->mNameSpace = new char[strlen(pszNamespace) + 1];
        if (!pClass->mNameSpace) goto ErrorExit;
        strcpy(pClass->mNameSpace, pszNamespace);
    }

    return pClass;

ErrorExit:
    if (pClass->mDesc) {
        if (pClass->mDesc->mClassIndexes)
            delete [] pClass->mDesc->mClassIndexes;
        if (pClass->mDesc->mAspectIndexes)
            delete [] pClass->mDesc->mAspectIndexes;
        if (pClass->mDesc->mAggrIndexes)
            delete [] pClass->mDesc->mAggrIndexes;
        if (pClass->mDesc->mInterfaces)
            delete [] pClass->mDesc->mInterfaces;
        if (pClass->mDesc->mAnnotations)
            delete [] pClass->mDesc->mAnnotations;
        delete pClass->mDesc;
    }
    delete pClass;
    return NULL;
}

void DeleteClassDirEntry(ClassDirEntry *pClass)
{
    assert(pClass != NULL);
    assert(pClass->mDesc != NULL);
    assert(pClass->mDesc->mInterfaces != NULL);
    assert(pClass->mDesc->mAggrIndexes != NULL);
    assert(pClass->mDesc->mAspectIndexes != NULL);

    for (int n = 0; n < pClass->mDesc->mAnnotationCount; n++) {
        DeleteAnnotation(pClass->mDesc->mAnnotations[n]);
    }
    delete [] pClass->mDesc->mAnnotations;
    for (int n = 0; n < pClass->mDesc->mInterfaceCount; n++) {
        DeleteClassInterface(pClass->mDesc->mInterfaces[n]);
    }
    delete [] pClass->mDesc->mInterfaces;
    delete [] pClass->mDesc->mAggrIndexes;
    delete [] pClass->mDesc->mAspectIndexes;
    delete pClass->mDesc;
    if (pClass->mName) delete pClass->mName;
    if (pClass->mNameSpace) delete pClass->mNameSpace;
    delete pClass;
}

InterfaceConstDescriptor *NewInterfaceConstDirEntry(const char *pszName)
{
    InterfaceConstDescriptor *pConst;

    assert(pszName != NULL);

    pConst = new InterfaceConstDescriptor;
    if (!pConst) return NULL;
    memset(pConst, 0, sizeof(InterfaceConstDescriptor));

    pConst->mName = new char[strlen(pszName) + 1];
    if (!pConst->mName) {
        delete pConst;
        return NULL;
    }
    strcpy(pConst->mName, pszName);
    return pConst;
}

void DeleteInterfaceConst(InterfaceConstDescriptor *pDesc)
{
    assert(pDesc != NULL);

    if (pDesc->mName) delete pDesc->mName;
    delete pDesc;
}

ParamDescriptor *NewParam(const char *pszName)
{
    ParamDescriptor *pParam;

    assert(pszName != NULL);

    pParam = new ParamDescriptor;
    if (!pParam) return NULL;
    memset(pParam, 0, sizeof(ParamDescriptor));

    pParam->mName = new char[strlen(pszName) + 1];
    if (!pParam->mName) {
        delete pParam;
        return NULL;
    }
    strcpy(pParam->mName, pszName);
    return pParam;
}

void DeleteParam(ParamDescriptor *pParam)
{
    assert(pParam != NULL);
    assert(pParam->mName != NULL);

    if (pParam->mType.mNestedType) delete pParam->mType.mNestedType;
    delete pParam->mName;
    delete pParam;
}

MethodDescriptor *NewMethod(const char *pszName)
{
    MethodDescriptor *pMethod;

    assert(pszName != NULL);

    pMethod = new MethodDescriptor;
    if (!pMethod) return NULL;
    memset(pMethod, 0, sizeof(MethodDescriptor));

    pMethod->mAnnotations = \
        new AnnotationDescriptor *[MAX_ANNOTATION_NUMBER];
    if (!pMethod->mAnnotations) goto ErrorExit;

    pMethod->mParams = \
        new ParamDescriptor *[MAX_PARAM_NUMBER];
    if (!pMethod->mParams) goto ErrorExit;

    pMethod->mName = new char[strlen(pszName) + 1];
    if (!pMethod->mName) goto ErrorExit;
    strcpy(pMethod->mName, pszName);

    return pMethod;

ErrorExit:
    if (pMethod->mParams)
        delete [] pMethod->mParams;
    if (pMethod->mAnnotations)
        delete [] pMethod->mAnnotations;
    delete pMethod;
    return NULL;
}

void DeleteMethod(MethodDescriptor *pMethod)
{
    assert(pMethod != NULL);
    assert(pMethod->mName != NULL);
    assert(pMethod->mParams != NULL);

    for (int n = 0; n < pMethod->mAnnotationCount; n++) {
        DeleteAnnotation(pMethod->mAnnotations[n]);
    }
    delete [] pMethod->mAnnotations;
    for (int n = 0; n < pMethod->mParamCount; n++) {
        DeleteParam(pMethod->mParams[n]);
    }
    delete [] pMethod->mParams;
    delete pMethod->mSignature;
    delete pMethod->mName;
    delete pMethod;
}

InterfaceDirEntry *NewInterfaceDirEntry(const char *pszName, const char *pszNamespace)
{
    assert(pszName != NULL);

    InterfaceDirEntry *pInterface;

    pInterface = new InterfaceDirEntry;
    if (!pInterface) return NULL;
    memset(pInterface, 0, sizeof(InterfaceDirEntry));

    pInterface->mDesc = new InterfaceDescriptor;
    if (!pInterface->mDesc) goto ErrorExit;
    memset(pInterface->mDesc, 0, sizeof(InterfaceDescriptor));

    pInterface->mDesc->mAnnotations = \
        new AnnotationDescriptor *[MAX_ANNOTATION_NUMBER];
    if (!pInterface->mDesc->mAnnotations) goto ErrorExit;

    pInterface->mDesc->mConsts = \
        new InterfaceConstDescriptor *[MAX_INTERFACE_CONST_NUMBER];
    if (!pInterface->mDesc->mConsts) goto ErrorExit;

    pInterface->mDesc->mMethods = \
        new MethodDescriptor *[MAX_METHOD_NUMBER];
    if (!pInterface->mDesc->mMethods) goto ErrorExit;

    pInterface->mName = new char[strlen(pszName) + 1];
    if (!pInterface->mName) goto ErrorExit;
    strcpy(pInterface->mName, pszName);
    if (pszNamespace != NULL) {
        pInterface->mNameSpace = new char[strlen(pszNamespace) + 1];
        if (!pInterface->mNameSpace) goto ErrorExit;
        strcpy(pInterface->mNameSpace, pszNamespace);
    }

    return pInterface;

ErrorExit:
    if (pInterface->mDesc) {
        if (pInterface->mDesc->mAnnotations)
            delete [] pInterface->mDesc->mAnnotations;
        if (pInterface->mDesc->mConsts)
            delete [] pInterface->mDesc->mConsts;
        if (pInterface->mDesc->mMethods)
            delete [] pInterface->mDesc->mMethods;
        delete pInterface->mDesc;
    }
    delete pInterface;
    return NULL;
}

void DeleteInterfaceDirEntry(InterfaceDirEntry *pInterface)
{
    assert(pInterface != NULL);
    assert(pInterface->mName != NULL);
    assert(pInterface->mDesc != NULL);
    assert(pInterface->mDesc->mMethods != NULL);

    for (int n = 0; n < pInterface->mDesc->mAnnotationCount; n++) {
        DeleteAnnotation(pInterface->mDesc->mAnnotations[n]);
    }
    delete [] pInterface->mDesc->mAnnotations;
    for (int n = 0; n < pInterface->mDesc->mConstCount; n++) {
        DeleteInterfaceConst(pInterface->mDesc->mConsts[n]);
    }
    delete [] pInterface->mDesc->mConsts;
    for (int n = 0; n < pInterface->mDesc->mMethodCount; n++) {
        DeleteMethod(pInterface->mDesc->mMethods[n]);
    }
    delete [] pInterface->mDesc->mMethods;
    delete pInterface->mDesc;
    delete pInterface->mName;
    if (pInterface->mNameSpace) delete pInterface->mNameSpace;
    delete pInterface;
}

StructElement *NewStructElement(const char *pszName)
{
    StructElement *pElement;

    assert(pszName != NULL);

    pElement = new StructElement;
    if (!pElement) return NULL;
    memset(pElement, 0, sizeof(StructElement));

    pElement->mName = new char[strlen(pszName) + 1];
    if (!pElement->mName) {
        delete pElement;
        return NULL;
    }
    strcpy(pElement->mName, pszName);
    return pElement;
}

void DeleteStructElement(StructElement *pElement)
{
    assert(pElement != NULL);
    assert(pElement->mName != NULL);

    if (pElement->mType.mNestedType) delete pElement->mType.mNestedType;
    delete pElement->mName;
    delete pElement;
}

StructDirEntry *NewStructDirEntry(const char *pszName)
{
    StructDirEntry *pStruct;

    assert(pszName != NULL);

    pStruct = new StructDirEntry;
    if (!pStruct) return NULL;
    memset(pStruct, 0, sizeof(StructDirEntry));

    pStruct->mDesc = new StructDescriptor;
    if (!pStruct->mDesc) goto ErrorExit;
    memset(pStruct->mDesc, 0, sizeof(StructDescriptor));

    pStruct->mDesc->mElements = new StructElement *[MAX_STRUCT_ELEMENT_NUMBER];
    if (!pStruct->mDesc->mElements) goto ErrorExit;

    pStruct->mName = new char[strlen(pszName) + 1];
    if (!pStruct->mName) goto ErrorExit;
    strcpy(pStruct->mName, pszName);

    return pStruct;

ErrorExit:
    if (pStruct->mDesc) {
        if (pStruct->mDesc->mElements)
            delete [] pStruct->mDesc->mElements;
        delete pStruct->mDesc;
    }
    delete pStruct;
    return NULL;
}

void DeleteStructDirEntry(StructDirEntry *pStruct)
{
    assert(pStruct != NULL);
    assert(pStruct->mName != NULL);
    assert(pStruct->mDesc != NULL);
    assert(pStruct->mDesc->mElements != NULL);

    for (int n = 0; n < pStruct->mDesc->mElementCount; n++) {
        DeleteStructElement(pStruct->mDesc->mElements[n]);
    }
    delete [] pStruct->mDesc->mElements;
    delete pStruct->mDesc;
    delete pStruct->mName;
    if (pStruct->mNameSpace) delete pStruct->mNameSpace;
    delete pStruct;
}

ArrayDirEntry *NewArrayDirEntry()
{
    ArrayDirEntry *pArray = new ArrayDirEntry;
    if (NULL == pArray)
        return NULL;

    memset(pArray, 0, sizeof(ArrayDirEntry));

    return pArray;
}

void DeleteArrayDirEntry(ArrayDirEntry *pArray)
{
    assert(pArray);

    if (pArray->mNameSpace) {
        delete pArray->mNameSpace;
        pArray->mNameSpace = NULL;
    }

    if (pArray->mType.mNestedType) {
        delete pArray->mType.mNestedType;
        pArray->mType.mNestedType = NULL;
    }

    delete pArray;
}

ConstDirEntry *NewConstDirEntry(const char *pszName)
{
    assert(pszName != NULL);

    ConstDirEntry *pConst = new ConstDirEntry;
    if (NULL == pConst)
        return NULL;

    memset(pConst, 0, sizeof(ConstDirEntry));

    pConst->mName = new char[strlen(pszName) + 1];
    if (!pConst->mName) goto ErrorExit;
    strcpy(pConst->mName, pszName);

    return pConst;

ErrorExit:
    delete pConst;
    return NULL;
}

void DeleteConstDirEntry(ConstDirEntry *pConst)
{
    assert(pConst != NULL);
    assert(pConst->mName != NULL);

    if (pConst->mNameSpace) {
        delete pConst->mNameSpace;
        pConst->mNameSpace = NULL;
    }

    delete pConst->mName;
    if (pConst->mType == TYPE_STRING && pConst->mV.mStrValue.mValue != NULL) {
        free(pConst->mV.mStrValue.mValue);
    }
    delete pConst;
}

EnumElement *NewEnumElement(const char *pszName)
{
    EnumElement *pElement;

    assert(pszName != NULL);

    pElement = new EnumElement;
    if (!pElement) return NULL;
    memset(pElement, 0, sizeof(EnumElement));

    pElement->mName = new char[strlen(pszName) + 1];
    if (!pElement->mName) {
        delete pElement;
        return NULL;
    }
    strcpy(pElement->mName, pszName);
    return pElement;
}

void DeleteEnumElement(EnumElement *pElement)
{
    assert(pElement != NULL);
    assert(pElement->mName != NULL);

    delete pElement->mName;
    delete pElement;
}

EnumDirEntry *NewEnumDirEntry(const char *pszName, const char *pszNamespace)
{
    EnumDirEntry *pEnum;

    assert(pszName != NULL);

    pEnum = new EnumDirEntry;
    if (!pEnum) return NULL;
    memset(pEnum, 0, sizeof(EnumDirEntry));

    pEnum->mDesc = new EnumDescriptor;
    if (!pEnum->mDesc) goto ErrorExit;
    memset(pEnum->mDesc, 0, sizeof(EnumDescriptor));

    pEnum->mDesc->mElements = new EnumElement *[MAX_ENUM_ELEMENT_NUMBER];
    if (!pEnum->mDesc->mElements) goto ErrorExit;

    pEnum->mName = new char[strlen(pszName) + 1];
    if (!pEnum->mName) goto ErrorExit;
    strcpy(pEnum->mName, pszName);
    if (pszNamespace != NULL) {
        pEnum->mNameSpace = new char[strlen(pszNamespace) + 1];
        if (!pEnum->mNameSpace) goto ErrorExit;
        strcpy(pEnum->mNameSpace, pszNamespace);
    }

    return pEnum;

ErrorExit:
    if (pEnum->mDesc) {
        if (pEnum->mDesc->mElements)
            delete [] pEnum->mDesc->mElements;
        delete pEnum->mDesc;
    }
    delete pEnum;
    return NULL;
}

void DeleteEnumDirEntry(EnumDirEntry *pEnum)
{
    assert(pEnum != NULL);
    assert(pEnum->mName != NULL);
    assert(pEnum->mDesc != NULL);
    assert(pEnum->mDesc->mElements != NULL);

    for (int n = 0; n < pEnum->mDesc->mElementCount; n++) {
        DeleteEnumElement(pEnum->mDesc->mElements[n]);
    }
    delete [] pEnum->mDesc->mElements;
    delete pEnum->mDesc;
    delete pEnum->mName;
    if (pEnum->mNameSpace) delete pEnum->mNameSpace;
    delete pEnum;
}

AliasDirEntry *NewAliasDirEntry(const char *pszName)
{
    assert(pszName != NULL);

    AliasDirEntry *pAlias;

    pAlias = new AliasDirEntry;
    if (!pAlias) return NULL;
    memset(pAlias, 0, sizeof(AliasDirEntry));

    pAlias->mName = new char[strlen(pszName) + 1];
    if (!pAlias->mName) {
        delete pAlias;
        return NULL;
    }
    strcpy(pAlias->mName, pszName);

    return pAlias;
}

void DeleteAliasDirEntry(AliasDirEntry *pAlias)
{
    assert(pAlias != NULL);
    assert(pAlias->mName != NULL);

    if (pAlias->mType.mNestedType) delete pAlias->mType.mNestedType;
    delete pAlias->mName;
    if (pAlias->mNameSpace) delete pAlias->mNameSpace;
    delete pAlias;
}

CLSModule *CreateCLS()
{
    CLSModule *pModule;
    FileDirEntry *pFile;

    pModule = new CLSModule;
    if (!pModule) return NULL;
    memset(pModule, '\0', sizeof(CLSModule));

    memcpy(pModule->mMagic, MAGIC_STRING, MAGIC_STRING_LENGTH);
    pModule->mCLSModuleVersion = CLSMODULE_VERSION;

    pModule->mFileDirs = new FileDirEntry *[MAX_FILE_NUMBER];
    pModule->mClassDirs = new ClassDirEntry *[MAX_CLASS_NUMBER];
    pModule->mInterfaceDirs = new InterfaceDirEntry *[MAX_INTERFACE_NUMBER];
    pModule->mDefinedInterfaceIndexes = new int[MAX_DEFINED_INTERFACE_NUMBER];
    pModule->mStructDirs = new StructDirEntry *[MAX_STRUCT_NUMBER];
    pModule->mEnumDirs = new EnumDirEntry *[MAX_ENUM_NUMBER];
    pModule->mAliasDirs = new AliasDirEntry *[MAX_ALIAS_NUMBER];
    pModule->mLibraryNames = new char *[MAX_LIBRARY_NUMBER];
    pModule->mArrayDirs = new ArrayDirEntry *[MAX_ARRAY_NUMBER];
    pModule->mConstDirs = new ConstDirEntry *[MAX_CONST_NUMBER];
    pFile = new FileDirEntry;

    if (!pModule->mClassDirs || !pModule->mInterfaceDirs ||
        !pModule->mStructDirs || !pModule->mEnumDirs ||
        !pModule->mAliasDirs || !pModule->mLibraryNames ||
        !pModule->mArrayDirs || !pModule->mConstDirs ||
        !pModule->mDefinedInterfaceIndexes || !pModule->mFileDirs ||
        !pFile) {
        DestroyCLS(pModule);
        return NULL;
    }

    pFile->mPath = NULL;
    pModule->mFileDirs[pModule->mFileCount++] = pFile;

    return pModule;
}

void DestroyCLS(CLSModule *pModule)
{
    int n;

    assert(pModule != NULL);

    if (pModule->mFileDirs) {
        for (n = 0; n < pModule->mFileCount; n++) {
            DeleteFileDirEntry(pModule->mFileDirs[n]);
        }
        delete [] pModule->mFileDirs;
    }

    if (pModule->mClassDirs) {
        for (n = 0; n < pModule->mClassCount; n++) {
            DeleteClassDirEntry(pModule->mClassDirs[n]);
        }
        delete [] pModule->mClassDirs;
    }

    if (pModule->mInterfaceDirs) {
        for (n = 0; n < pModule->mInterfaceCount; n++) {
            DeleteInterfaceDirEntry(pModule->mInterfaceDirs[n]);
        }
        delete [] pModule->mInterfaceDirs;
    }

    if (pModule->mDefinedInterfaceIndexes) {
        delete [] pModule->mDefinedInterfaceIndexes;
    }

    if (pModule->mArrayDirs) {
        for (n = 0; n < pModule->mArrayCount; n++) {
            DeleteArrayDirEntry(pModule->mArrayDirs[n]);
        }
        delete [] pModule->mArrayDirs;
    }

    if (pModule->mStructDirs) {
        for (n = 0; n < pModule->mStructCount; n++) {
            DeleteStructDirEntry(pModule->mStructDirs[n]);
        }
        delete [] pModule->mStructDirs;
    }

    if (pModule->mEnumDirs) {
        for (n = 0; n < pModule->mEnumCount; n++) {
            DeleteEnumDirEntry(pModule->mEnumDirs[n]);
        }
        delete [] pModule->mEnumDirs;
    }

    if (pModule->mAliasDirs) {
        for (n = 0; n < pModule->mAliasCount; n++) {
            DeleteAliasDirEntry(pModule->mAliasDirs[n]);
        }
        delete [] pModule->mAliasDirs;
    }

    if (pModule->mConstDirs) {
        for (n = 0; n < pModule->mConstCount; n++) {
            DeleteConstDirEntry(pModule->mConstDirs[n]);
        }
        delete [] pModule->mConstDirs;
    }

    if (pModule->mLibraryNames) {
        for (n = 0; n < pModule->mLibraryCount; n++) {
            delete pModule->mLibraryNames[n];
        }
        delete [] pModule->mLibraryNames;
    }

    if (pModule->mUunm) delete pModule->mUunm;
    if (pModule->mName) delete pModule->mName;

    delete pModule;
}

int CreateFileDirEntry(
    const char *pszPath, CLSModule *pModule)
{
    int n;
    FileDirEntry *pFile;

    if (!pszPath || pszPath[0] == '\0') return 0;

    n = SelectFileDirEntry(pszPath, pModule);
    if (n >= 1) {
        _ReturnOK (n);
    }

    assert(pModule->mFileCount < MAX_FILE_NUMBER);

    pFile = NewFileDirEntry(pszPath);
    pModule->mFileDirs[pModule->mFileCount] = pFile;

    _ReturnOK (pModule->mFileCount++);
}

int CreateClassDirEntry(
    const char *pszName, CLSModule *pModule, unsigned long attribs)
{
    int n;
    ClassDirEntry *pClass;
    ClassDescriptor *pDesc;

    char *pszNamespace = NULL;
    const char *dot = strrchr(pszName, '.');
    if (dot != NULL) {
        pszNamespace = (char*)malloc(dot - pszName + 1);
        memset(pszNamespace, 0, dot - pszName + 1);
        memcpy(pszNamespace, pszName, dot - pszName);
        pszName = dot + 1;
    }
    n = SelectClassDirEntry(pszName, pszNamespace, pModule);
    if (n >= 0) {
        pDesc = pModule->mClassDirs[n]->mDesc;
        if (pszNamespace != NULL) free(pszNamespace);

        if (CLASS_TYPE(attribs) != CLASS_TYPE(pDesc->mAttribs)) {
            ExtraMessage(pModule->mClassDirs[n]->mNameSpace,
                        "class", pszName);
            _ReturnError (CLSError_NameConflict);
        }
        if (pDesc->mInterfaceCount > 0
            || (pDesc->mAttribs & ClassAttrib_hasparent) > 0
            || IsValidUUID(&pDesc->mClsid)) {
            ExtraMessage(pModule->mClassDirs[n]->mNameSpace,
                        "class", pszName);
            _ReturnError (CLSError_DupEntry);
        }
        _ReturnOK (n);
    }

    n = GlobalSelectSymbol(pszName, pszNamespace, pModule, GType_Class, NULL);
    if (n >= 0) {
        if (pszNamespace != NULL) free(pszNamespace);
        _ReturnError (CLSError_NameConflict);
    }
    if (pModule->mClassCount >= MAX_CLASS_NUMBER) {
        if (pszNamespace != NULL) free(pszNamespace);
        _ReturnError (CLSError_FullEntry);
    }

    pClass = NewClassDirEntry(pszName, pszNamespace);
    if (pszNamespace != NULL) free(pszNamespace);
    if (!pClass) _ReturnError (CLSError_OutOfMemory);
    pModule->mClassDirs[pModule->mClassCount] = pClass;

    _ReturnOK (pModule->mClassCount++);
}

int CreateInterfaceDirEntry(
    const char *pszName, CLSModule *pModule, unsigned long attribs)
{
    int n;
    InterfaceDirEntry *pInterface;
    InterfaceDescriptor *pDesc;

    char *pszNamespace = NULL;
    const char *dot = strrchr(pszName, '.');
    if (dot != NULL) {
        pszNamespace = (char*)malloc(dot - pszName + 1);
        memset(pszNamespace, 0, dot - pszName + 1);
        memcpy(pszNamespace, pszName, dot - pszName);
        pszName = dot + 1;
    }
    n = SelectInterfaceDirEntry(pszName, pszNamespace, pModule);
    if (n >= 0) {
        pDesc = pModule->mInterfaceDirs[n]->mDesc;
        if (pszNamespace != NULL) free(pszNamespace);

        if (INTERFACE_TYPE(attribs) != \
            INTERFACE_TYPE(pDesc->mAttribs)) {
            ExtraMessage(pModule->mInterfaceDirs[n]->mNameSpace,
                        "interface", pModule->mInterfaceDirs[n]->mName);
            _ReturnError (CLSError_NameConflict);
        }
        if (pDesc->mMethodCount > 0 || pDesc->mParentIndex != 0
            || IsValidUUID(&pDesc->mIID)) {
            ExtraMessage(pModule->mInterfaceDirs[n]->mNameSpace,
                        "interface", pModule->mInterfaceDirs[n]->mName);
            _ReturnError (CLSError_DupEntry);
        }
        _ReturnOK (n);
    }

    n = GlobalSelectSymbol(pszName, pszNamespace, pModule, GType_Interface, NULL);
    if (n >= 0) {
        if (pszNamespace != NULL) free(pszNamespace);
        _ReturnError (CLSError_NameConflict);
    }
    if (pModule->mInterfaceCount >= MAX_INTERFACE_NUMBER) {
        if (pszNamespace != NULL) free(pszNamespace);
        _ReturnError (CLSError_FullEntry);
    }

    pInterface = NewInterfaceDirEntry(pszName, pszNamespace);
    if (pszNamespace != NULL) free(pszNamespace);
    if (!pInterface) _ReturnError (CLSError_OutOfMemory);
    pModule->mInterfaceDirs[pModule->mInterfaceCount] = pInterface;

    _ReturnOK (pModule->mInterfaceCount++);
}

int CreateArrayDirEntry(CLSModule *pModule)
{
    ArrayDirEntry *pArray;

    pArray = NewArrayDirEntry();
    if (!pArray) _ReturnError (CLSError_OutOfMemory);

    pModule->mArrayDirs[pModule->mArrayCount] = pArray;

    _ReturnOK (pModule->mArrayCount++);
}

int CreateStructDirEntry(
    const char *pszName, CLSModule *pModule)
{
    int n;
    StructDirEntry *pStruct;

    n = SelectStructDirEntry(pszName, pModule);
    if (n >= 0) {
        if (pModule->mStructDirs[n]->mDesc->mElementCount > 0) {
            ExtraMessage(pModule->mStructDirs[n]->mNameSpace,
                        "struct", pModule->mStructDirs[n]->mName);
            _ReturnError (CLSError_DupEntry);
        }
        _ReturnOK (n);
    }

    n = GlobalSelectSymbol(pszName, NULL, pModule, GType_Struct, NULL);
    if (n >= 0) _ReturnError (CLSError_NameConflict);
    if (pModule->mStructCount >= MAX_STRUCT_NUMBER)
        _ReturnError (CLSError_FullEntry);

    pStruct = NewStructDirEntry(pszName);
    if (!pStruct) _ReturnError (CLSError_OutOfMemory);
    pModule->mStructDirs[pModule->mStructCount] = pStruct;

    _ReturnOK (pModule->mStructCount++);
}

int CreateEnumDirEntry(
    const char *pszName, CLSModule *pModule)
{
    int n;
    EnumDirEntry *pEnum;

    char *pszNamespace = NULL;
    const char *dot = strrchr(pszName, '.');
    if (dot != NULL) {
        pszNamespace = (char*)malloc(dot - pszName + 1);
        memset(pszNamespace, 0, dot - pszName + 1);
        memcpy(pszNamespace, pszName, dot - pszName);
        pszName = dot + 1;
    }
    n = SelectEnumDirEntry(pszName, pszNamespace, pModule);
    if (n >= 0) {
        if (pszNamespace != NULL) free(pszNamespace);
        if (pModule->mEnumDirs[n]->mDesc->mElementCount > 0) {
            ExtraMessage(pModule->mEnumDirs[n]->mNameSpace,
                        "enum", pModule->mEnumDirs[n]->mName);
            _ReturnError (CLSError_DupEntry);
        }
        _ReturnOK (n);
    }

    n = GlobalSelectSymbol(pszName, pszNamespace, pModule, GType_Enum, NULL);
    if (n >= 0) {
        if (pszNamespace != NULL) free(pszNamespace);
        _ReturnError (CLSError_NameConflict);
    }
    if (pModule->mEnumCount >= MAX_ENUM_NUMBER) {
        if (pszNamespace != NULL) free(pszNamespace);
        _ReturnError (CLSError_FullEntry);
    }

    pEnum = NewEnumDirEntry(pszName, pszNamespace);
    if (pszNamespace != NULL) free(pszNamespace);
    if (!pEnum) _ReturnError (CLSError_OutOfMemory);
    pModule->mEnumDirs[pModule->mEnumCount] = pEnum;

    _ReturnOK (pModule->mEnumCount++);
}

int CreateConstDirEntry(
    const char *pszName, CLSModule *pModule)
{
    int n;
    ConstDirEntry *pConst;

    n = SelectConstDirEntry(pszName, pModule);
    if (n >= 0) {
        _ReturnError (CLSError_DupEntry);
    }

    n = GlobalSelectSymbol(pszName, NULL, pModule, GType_Const, NULL);
    if (n >= 0) _ReturnError (CLSError_NameConflict);
    if (pModule->mConstCount >= MAX_CONST_NUMBER) _ReturnError (CLSError_FullEntry);

    pConst = NewConstDirEntry(pszName);
    if (!pConst) _ReturnError (CLSError_OutOfMemory);
    pModule->mConstDirs[pModule->mConstCount] = pConst;

    _ReturnOK (pModule->mConstCount++);
}

int CreateAliasDirEntry(
    const char *pszName, CLSModule *pModule, TypeDescriptor *pType)
{
    int n;
    AliasDirEntry *pAlias;
    GlobalSymbolType gType;
    TypeDescriptor type;

    n = GlobalSelectSymbol(pszName, NULL, pModule, GType_None, &gType);
    if (n >= 0) {
        if (gType == GType_Class) {
            _ReturnError (CLSError_NameConflict);
        }

        memset(&type, 0, sizeof(type));
        type.mType = (CARDataType)gType;
        type.mIndex = n;
        if (!IsEqualType(pModule, &type, pType)) {
            _ReturnError (CLSError_NameConflict);
        }
    }
    if (pModule->mAliasCount >= MAX_ALIAS_NUMBER)
        _ReturnError (CLSError_FullEntry);

    pAlias = NewAliasDirEntry(pszName);
    if (!pAlias) _ReturnError (CLSError_OutOfMemory);
    memcpy(&pAlias->mType, pType, sizeof(TypeDescriptor));
    pModule->mAliasDirs[pModule->mAliasCount] = pAlias;

    _ReturnOK (pModule->mAliasCount++);
}

int CreateClassInterface(USHORT index, ClassDescriptor *pDesc)
{
    int n;
    ClassInterface *pClassInterface;

    assert(pDesc != NULL);

    n = SelectClassInterface(index, pDesc);
    if (n >= 0) _ReturnError (CLSError_DupEntry);

    if (pDesc->mInterfaceCount >= MAX_CLASS_INTERFACE_NUMBER)
        _ReturnError (CLSError_FullEntry);

    pClassInterface = NewClassInterface(index);
    if (!pClassInterface) _ReturnError (CLSError_OutOfMemory);
    pDesc->mInterfaces[pDesc->mInterfaceCount] = pClassInterface;

    _ReturnOK (pDesc->mInterfaceCount++);
}

int CreateInterfaceConstDirEntry(
    const char *pszName, InterfaceDescriptor *pInterface)
{
    int n;
    InterfaceConstDescriptor *pConst;

    n = SelectInterfaceConstDirEntry(pszName, pInterface);
    if (n >= 0) {
        _ReturnError (CLSError_DupEntry);
    }

    n = SelectInterfaceMemberSymbol(pszName, pInterface);
    if (n >= 0) _ReturnError (CLSError_NameConflict);
    if (pInterface->mConstCount >= MAX_INTERFACE_CONST_NUMBER) _ReturnError (CLSError_FullEntry);

    pConst = NewInterfaceConstDirEntry(pszName);
    if (!pConst) _ReturnError (CLSError_OutOfMemory);
    pInterface->mConsts[pInterface->mConstCount] = pConst;

    _ReturnOK (pInterface->mConstCount++);
}

int CreateInterfaceMethod(
    const char *pszName, InterfaceDescriptor *pInterface)
{
    MethodDescriptor *pMethod;

    assert(pInterface != NULL);
    assert(pszName != NULL);

    if (pInterface->mMethodCount >= MAX_METHOD_NUMBER)
        _ReturnError (CLSError_FullEntry);

    pMethod = NewMethod(pszName);
    if (!pMethod) _ReturnError (CLSError_OutOfMemory);
    pInterface->mMethods[pInterface->mMethodCount] = pMethod;

    _ReturnOK (pInterface->mMethodCount++);
}

int CreateMethodParam(
    const char *pszName, MethodDescriptor *pMethod)
{
    int n;
    ParamDescriptor *pParam;

    assert(pMethod != NULL);
    assert(pszName != NULL);

    n = SelectMethodParam(pszName, pMethod);
    if (n >= 0) {
        ExtraMessage("method parameter", pszName);
        _ReturnError (CLSError_DupEntry);
    }

    if (pMethod->mParamCount >= MAX_PARAM_NUMBER) _ReturnError (CLSError_FullEntry);
    pParam = NewParam(pszName);
    if (!pParam) _ReturnError (CLSError_OutOfMemory);
    pMethod->mParams[pMethod->mParamCount] = pParam;

    _ReturnOK (pMethod->mParamCount++);
}

int CreateStructElement(
    const char *pszName, StructDescriptor *pStruct)
{
    int n;
    StructElement *pElement;

    assert(pStruct != NULL);
    assert(pszName != NULL);

    n = SelectStructElement(pszName, pStruct);
    if (n >= 0) {
        ExtraMessage("struct member", pszName);
        _ReturnError (CLSError_DupEntry);
    }

    if (pStruct->mElementCount >= MAX_STRUCT_ELEMENT_NUMBER)
        _ReturnError (CLSError_FullEntry);

    pElement = NewStructElement(pszName);
    if (!pElement) _ReturnError (CLSError_OutOfMemory);
    pStruct->mElements[pStruct->mElementCount] = pElement;

    _ReturnOK (pStruct->mElementCount++);
}

int CreateEnumElement(
    const char *pszName, CLSModule *pModule, EnumDescriptor *pEnum)
{
    int n;
    EnumElement *pElement;

    assert(pModule != NULL);
    assert(pEnum != NULL);
    assert(pszName != NULL);

    n = SelectEnumElement(pszName, pEnum);
    if (n >= 0) {
        ExtraMessage("enum member", pszName);
        _ReturnError (CLSError_DupEntry);
    }

    pElement = GlobalSelectEnumElement(pszName, pModule);
    if (pElement) _ReturnError (CLSError_NameConflict);

    if (pEnum->mElementCount >= MAX_ENUM_ELEMENT_NUMBER)
        _ReturnError (CLSError_FullEntry);

    pElement = NewEnumElement(pszName);
    if (!pElement) _ReturnError (CLSError_OutOfMemory);
    pEnum->mElements[pEnum->mElementCount] = pElement;

    _ReturnOK (pEnum->mElementCount++);
}

int MethodAppend(
    const MethodDescriptor *pSrc, InterfaceDescriptor *pDesc)
{
    int n, m;
    MethodDescriptor *pDest;
    ParamDescriptor *pParam;

    n = CreateInterfaceMethod(pSrc->mName, pDesc);
    if (n < 0) _Return (n);

    pDest = pDesc->mMethods[n];
    pDest->mSignature = new char[strlen(pSrc->mSignature) + 1];
    strcpy(pDest->mSignature, pSrc->mSignature);

    TypeCopy(&pSrc->mType, &pDest->mType);

    for (n = 0; n < pSrc->mParamCount; n++) {
        m = CreateMethodParam(pSrc->mParams[n]->mName, pDest);
        if (m < 0) _Return (m);
        pParam = pDest->mParams[m];
        pParam->mAttribs = pSrc->mParams[n]->mAttribs;
        TypeCopy(&pSrc->mParams[n]->mType, &pParam->mType);
    }

    _ReturnOK (CLS_NoError);
}

int InterfaceMethodsAppend(const CLSModule *pModule,
    const InterfaceDescriptor *pSrc, InterfaceDescriptor *pDest)
{
    int n, m;

    if (0 != pSrc->mParentIndex) {
        n = InterfaceMethodsAppend(pModule,
                pModule->mInterfaceDirs[pSrc->mParentIndex]->mDesc,
                pDest);
        if (n < 0) _Return (n);
    }

    for (n = 0; n < pSrc->mMethodCount; n++) {
        m = MethodAppend(pSrc->mMethods[n], pDest);
        if (m < 0) _Return (m);
    }

    _ReturnOK (CLS_NoError);
}
