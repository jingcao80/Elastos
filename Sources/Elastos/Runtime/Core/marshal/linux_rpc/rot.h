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

#ifndef __ROT_H__
#define __ROT_H__

#include <prxstub.h>
#include <elcontainer.h>

namespace Elastos {
namespace RPC {

typedef struct ExportObject: SLinkNode
{
    String              mNetAddress;
    IInterface*         mObject;
    IStub*              mIStub;
} ExportObject;

typedef struct ImportObject: SLinkNode
{
    String              mNetAddress;
    IProxy*             mIProxy;
} ImportObject;

extern ECode InitROT();

extern void UninitROT();

extern ECode RegisterExportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IInterface* object,
    /* [in] */ IStub* stub);

extern ECode UnregisterExportObject(
    /* [in] */ const String& netAddress);

extern ECode FindExportObject(
    /* [in] */ IInterface* object,
    /* [out] */ ExportObject* expObj);

extern ECode FindExportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ExportObject* expObj);

extern ECode RegisterImportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IProxy* proxy);

extern ECode UnregisterImportObject(
    /* [in] */ const String& netAddress);

extern ECode FindImportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ImportObject* impObj);

} // namespace RPC
} // namespace Elastos

#endif //__ROT_H__
