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
namespace IPC {

typedef struct ExportObject: SLinkNode
{
    android::IBinder*   mBinder;
    IInterface*         mObject;
    IStub*              mIStub;
} ExportObject;

typedef struct ImportObject: SLinkNode
{
    android::IBinder*   mBinder;
    IProxy*             mIProxy;
} ImportObject;

extern ECode InitROT();

extern void UninitROT();

extern ECode RegisterExportObject(
    /* [in] */ android::IBinder* binder,
    /* [in] */ IInterface* object,
    /* [in] */ IStub* stub);

extern ECode UnregisterExportObject(
    /* [in] */ android::IBinder* binder);

extern ECode FindExportObject(
    /* [in] */ IInterface* object,
    /* [out] */ ExportObject* expObj);

extern ECode FindExportObject(
    /* [in] */ android::IBinder* binder,
    /* [out] */ ExportObject* expObj);

extern ECode RegisterImportObject(
    /* [in] */ android::IBinder* binder,
    /* [in] */ IProxy* proxy);

extern ECode UnregisterImportObject(
    /* [in] */ android::IBinder* binder);

extern ECode FindImportObject(
    /* [in] */ android::IBinder* binder,
    /* [out] */ ImportObject* impObj);

} // namespace IPC
} // namespace Elastos

#endif //__ROT_H__
