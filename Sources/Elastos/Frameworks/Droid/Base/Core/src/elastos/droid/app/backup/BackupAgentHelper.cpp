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

#include "elastos/droid/app/backup/BackupAgentHelper.h"
#ifdef DROID_CORE
#include "elastos/droid/app/backup/CBackupHelperDispatcher.h"
#endif
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::Backup::CBackupHelperDispatcher;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

extern "C" const InterfaceID EIID_BackupAgentHelper;

// 89b3a0d5-7f59-43fc-a281-596c645c381c
extern "C" const InterfaceID EIID_BackupAgentHelper =
     { 0x89b3a0d5, 0x7f59, 0x43fc, { 0xa2, 0x81, 0x59, 0x6c, 0x64, 0x5c, 0x38, 0x1c }  };

CString BackupAgentHelper::TAG = "BackupAgentHelper";

BackupAgentHelper::BackupAgentHelper()
{
    CBackupHelperDispatcher::New((IBackupHelperDispatcher**)&mDispatcher);
}

BackupAgentHelper::~BackupAgentHelper()
{

}

PInterface BackupAgentHelper::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IBackupAgentHelper*)this;
    }
    else if (riid == EIID_IBackupAgentHelper) {
        return (IBackupAgentHelper*)this;
    }
    else if (riid == EIID_BackupAgent) {
        return reinterpret_cast<PInterface>((BackupAgent*)this);
    }
    else if (riid == EIID_BackupAgentHelper) {
        return reinterpret_cast<PInterface>(this);
    }

    return BackupAgent::Probe(riid);
}

UInt32 BackupAgentHelper::AddRef()
{
    return BackupAgent::AddRef();
}

UInt32 BackupAgentHelper::Release()
{
    return BackupAgent::Release();
}

ECode BackupAgentHelper::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IBackupAgentHelper *)this) {
        *pIID = EIID_IBackupAgentHelper;
        return NOERROR;
    }

    return BackupAgent::GetInterfaceID(pObject, pIID);
}

ECode BackupAgentHelper::OnBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    return mDispatcher->PerformBackup(oldState, data, newState);
}

ECode BackupAgentHelper::OnRestore(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    return mDispatcher->PerformRestore(data, appVersionCode, newState);
}

ECode BackupAgentHelper::GetDispatcher(
    /* [out] */ IBackupHelperDispatcher** dispatcher)
{
    VALIDATE_NOT_NULL(dispatcher);
    *dispatcher = mDispatcher;
    REFCOUNT_ADD(*dispatcher);
    return NOERROR;
}

ECode BackupAgentHelper::AddHelper(
    /* [in] */ const String& keyPrefix,
    /* [in] */ IBackupHelper* helper) {
    mDispatcher->AddHelper(keyPrefix, helper);
    return NOERROR;
}

}
}
}
}
