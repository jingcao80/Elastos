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

#ifndef __LOCMOD_H__
#define __LOCMOD_H__

#include <clsinfo.h>
#include <elcontainer.h>

using namespace Elastos;

typedef ECode (__stdcall *PDLLGETCLASSOBJECT)(REMuid, REIID, PInterface*);
typedef ECode (__stdcall *PDLLCANUNLOADNOW)();

typedef struct LocalModule : DLinkNode {
    String              mUunm;
    Void*               mIModule;
    PDLLGETCLASSOBJECT  mDllGetClassObjectFunc;
    PDLLCANUNLOADNOW    mDllCanUnloadNowFunc;
    CIClassInfo*        mClassInfo;
    Int32               mAskCount;
} LocalModule;

ECode  AcquireClassObjectFromLocalModule(
    /* [in] */ RClassID rclsid,
    /* [in] */ REIID riid,
    /* [out] */ PInterface* object);

#endif // __LOCMOD_H__
