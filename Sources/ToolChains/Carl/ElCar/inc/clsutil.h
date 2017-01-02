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

#ifndef __CLSUTIL_H__
#define __CLSUTIL_H__

#include <clsdef.h>
#include <namelist.h>

extern const char *ParamAttrib2String(DWORD attribs);
extern const char *StructType2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType);
extern const char *CStyleStructParamType2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType);
extern const char *Type2CString(
                    const CLSModule *pModule,
                    const TypeDescriptor *pType);
extern const char *Array2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType,
    const char *pszElemName);
extern const char *Dims2CString(
    const CLSModule * pModule,
    const TypeDescriptor * pType);
extern const char *Type2IString(
                    const CLSModule *pModule,
                    const TypeDescriptor *pType);
extern const char *Uuid2CString(const GUID *pUuid, BOOL bUpper);
extern const char *Uuid2IString(const GUID *pUuid, BOOL bUpper);

extern int CLS2AbrgCpp(const char *pszFile, const CLSModule *pModule);
extern int CLS2AbrgCpp_(FILE *pFile, const CLSModule *pModule);
extern int CLS2CAR(const char *pszFile, const CLSModule *pModule);
extern int CLS2CAR_(FILE *pFile, const CLSModule *pModule);
extern int InterfaceDescriptorCopy(const CLSModule *pSrcModule, const InterfaceDescriptor *pSrc, CLSModule *pDestModule, InterfaceDescriptor *pDest, BOOL bNameSpace);
extern void GenerateTypeStringForParam(
    const CLSModule *pModule,
    const TypeDescriptor *pType,
    char *pszBuf);
extern unsigned int rabin(const void *buf, int size);

#endif // __CLSUTIL_H__
