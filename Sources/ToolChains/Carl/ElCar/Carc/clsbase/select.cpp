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
#include <stdlib.h>
#include <assert.h>

#include <clsbase.h>

int SelectFileDirEntry(const char *pszPath, const CLSModule *pModule)
{
    int n;

    assert(pszPath != NULL);

    for (n = 1; n < pModule->mFileCount; n++) {
        if (!strcmp(pszPath, pModule->mFileDirs[n]->mPath)) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectClassDirEntry(const char *pszName, const char *pszNamespaces, const CLSModule *pModule)
{
    int n;

    assert(pModule != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pModule->mClassCount; n++) {
        if (!strcmp(pszName, pModule->mClassDirs[n]->mName)) {
            if (pModule->mClassDirs[n]->mNameSpace == NULL) _ReturnOK (n);
            //temp
            if (!strcmp("systypes", pModule->mClassDirs[n]->mNameSpace)) _ReturnOK (n);

            const char *begin, *semicolon;
            begin = pszNamespaces;
            while (begin != NULL) {
                semicolon = strchr(begin, ';');
                if (semicolon != NULL) { *const_cast<char*>(semicolon) = '\0'; }
                if (!strcmp(begin, pModule->mClassDirs[n]->mNameSpace)) _ReturnOK (n);
                if (semicolon != NULL) { *const_cast<char*>(semicolon) = ';'; begin = semicolon + 1; }
                else begin = NULL;
            }
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectInterfaceDirEntry(const char *pszName, const char *pszNamespaces, const CLSModule *pModule)
{
    int n;

    assert(pModule != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pModule->mInterfaceCount; n++) {
        if (!strcmp(pszName, pModule->mInterfaceDirs[n]->mName)) {
            if (pModule->mInterfaceDirs[n]->mNameSpace == NULL) _ReturnOK (n);
            //temp
            if (!strcmp("systypes", pModule->mInterfaceDirs[n]->mNameSpace)) _ReturnOK (n);

            const char *begin, *semicolon;
            begin = pszNamespaces;
            while (begin != NULL) {
                semicolon = strchr(begin, ';');
                if (semicolon != NULL) { *const_cast<char*>(semicolon) = '\0'; }
                if (!strcmp(begin, pModule->mInterfaceDirs[n]->mNameSpace)) _ReturnOK (n);
                if (semicolon != NULL) { *const_cast<char*>(semicolon) = ';'; begin = semicolon + 1; }
                else begin = NULL;
            }
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectArrayDirEntry(unsigned short nElems, const TypeDescriptor &desp,
    const CLSModule *pModule)
{
    int n;
    assert(nElems > 0);
    assert(pModule != NULL);

    for (n = 0; n < pModule->mArrayCount; n++) {
        if (nElems == pModule->mArrayDirs[n]->mElementCount
            && 0 == memcmp(&desp, &(pModule->mArrayDirs[n]->mType), sizeof(TypeDescriptor)))
            _ReturnOK (n);
    }

    _ReturnError (CLSError_NotFound);
}

int SelectStructDirEntry(const char *pszName, const CLSModule *pModule)
{
    int n;

    assert(pModule != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pModule->mStructCount; n++) {
        if (!strcmp(pszName, pModule->mStructDirs[n]->mName))
            _ReturnOK (n);
    }

    _ReturnError (CLSError_NotFound);
}

int SelectEnumDirEntry(const char *pszName, const char *pszNamespaces, const CLSModule *pModule)
{
    int n;

    assert(pModule != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pModule->mEnumCount; n++) {
        if (!strcmp(pszName, pModule->mEnumDirs[n]->mName)) {
            if (pModule->mEnumDirs[n]->mNameSpace == NULL) _ReturnOK (n);
            //temp
            if (!strcmp("systypes", pModule->mEnumDirs[n]->mNameSpace)) _ReturnOK (n);

            const char *begin, *semicolon;
            begin = pszNamespaces;
            while (begin != NULL) {
                semicolon = strchr(begin, ';');
                if (semicolon != NULL) { *const_cast<char*>(semicolon) = '\0'; }
                if (!strcmp(begin, pModule->mEnumDirs[n]->mNameSpace)) _ReturnOK (n);
                if (semicolon != NULL) { *const_cast<char*>(semicolon) = ';'; begin = semicolon + 1; }
                else begin = NULL;
            }
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectConstDirEntry(const char *pszName, const CLSModule *pModule)
{
    int n;

    assert(pModule != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pModule->mConstCount; n++) {
        if (!strcmp(pszName, pModule->mConstDirs[n]->mName)) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectAliasDirEntry(const char *pszName, const CLSModule *pModule)
{
    int n;

    assert(pModule != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pModule->mAliasCount; n++) {
        if (!strcmp(pszName, pModule->mAliasDirs[n]->mName))
            _ReturnOK (n);
    }

    _ReturnError (CLSError_NotFound);
}

int SelectClassInterface(USHORT index, const ClassDescriptor *pDesc)
{
    int n;

    assert(pDesc != NULL);

    for (n = 0; n < pDesc->mInterfaceCount; n++) {
        if (pDesc->mInterfaces[n]->mIndex == index) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectInterfaceConstDirEntry(
    const char *pszName, const InterfaceDescriptor *pDesc)
{
    int n;

    assert(pDesc != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pDesc->mConstCount; n++) {
        if (!strcmp(pszName, pDesc->mConsts[n]->mName)) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectInterfaceMethod(
    const char *pszName, const InterfaceDescriptor *pDesc)
{
    int n;

    assert(pDesc != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pDesc->mMethodCount; n++) {
        if (!strcmp(pszName, pDesc->mMethods[n]->mName)) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectMethodParam(const char *pszName, const MethodDescriptor *pDesc)
{
    int n;

    assert(pDesc != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pDesc->mParamCount; n++) {
        if (!strcmp(pszName, pDesc->mParams[n]->mName)) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectStructElement(
    const char *pszName, const StructDescriptor *pDesc)
{
    int n;

    assert(pDesc != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pDesc->mElementCount; n++) {
        if (!strcmp(pszName, pDesc->mElements[n]->mName)) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int SelectEnumElement(const char *pszName, const EnumDescriptor *pDesc)
{
    int n;

    assert(pDesc != NULL);
    assert(pszName != NULL);

    for (n = 0; n < pDesc->mElementCount; n++) {
        if (!strcmp(pszName, pDesc->mElements[n]->mName)) {
            _ReturnOK (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

int GlobalSelectSymbol(
    const char *pszName, const char *pszNamespace, const CLSModule *pModule,
    GlobalSymbolType except, GlobalSymbolType *pType)
{
    int n;

    assert(pModule != NULL);
    assert(pszName != NULL);

    if (except != GType_Class) {
        n = SelectClassDirEntry(pszName, pszNamespace, pModule);
        if (n >= 0) {
            ExtraMessage(pModule->mClassDirs[n]->mNameSpace,
                        "class", pszName);
            if (pType) *pType = GType_Class;
            _Return (n);
        }
    }

    if (except != GType_Interface) {
        n = SelectInterfaceDirEntry(pszName, pszNamespace, pModule);
        if (n >= 0) {
            ExtraMessage(pModule->mInterfaceDirs[n]->mNameSpace,
                        "interface", pszName);
            if (pType) *pType = GType_Interface;
            _Return (n);
        }
    }

    if (except != GType_Struct) {
        n = SelectStructDirEntry(pszName, pModule);
        if (n >= 0) {
            ExtraMessage(pModule->mStructDirs[n]->mNameSpace,
                        "struct", pszName);
            if (pType) *pType = GType_Struct;
            _Return (n);
        }
    }

    if (except != GType_Alias) {
        n = SelectAliasDirEntry(pszName, pModule);
        if (n >= 0) {
            ExtraMessage(pModule->mAliasDirs[n]->mNameSpace,
                        "alias", pszName);
            if (pType) *pType = GType_Alias;
            _Return (n);
        }
    }

    if (except != GType_Enum) {
        n = SelectEnumDirEntry(pszName, pszNamespace, pModule);
        if (n >= 0) {
            ExtraMessage(pModule->mEnumDirs[n]->mNameSpace,
                        "enum", pszName);
            if (pType) *pType = GType_Enum;
            _Return (n);
        }
    }

    _ReturnError (CLSError_NotFound);
}

EnumElement *GlobalSelectEnumElement(
    const char *pszName, const CLSModule *pModule)
{
    int n, m;

    for (n = 0; n < pModule->mEnumCount; n++) {
        m = SelectEnumElement(pszName, pModule->mEnumDirs[n]->mDesc);
        if (m >= 0) {
            ExtraMessage(pModule->mEnumDirs[n]->mNameSpace,
                        "enum", pModule->mEnumDirs[n]->mName);
            return pModule->mEnumDirs[n]->mDesc->mElements[m];
        }
    }
    return NULL;
}

int SelectInterfaceMemberSymbol(
    const char *pszName, InterfaceDescriptor *pDesc)
{
    int n;

    assert(pszName != NULL);
    assert(pDesc != NULL);

    for (n = 0; n < pDesc->mConstCount; ++n) {
        if (!strcmp(pszName, pDesc->mConsts[n]->mName)) _Return (n);
    }

    for (n = 0; n < pDesc->mMethodCount; ++n) {
        if (!strcmp(pszName, pDesc->mMethods[n]->mName)) _Return (n);
    }

    _ReturnError (CLSError_NotFound);
}
