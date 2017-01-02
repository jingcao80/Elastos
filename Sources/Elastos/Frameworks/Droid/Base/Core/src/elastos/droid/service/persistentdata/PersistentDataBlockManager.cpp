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

#include "elastos/droid/service/persistentdata/PersistentDataBlockManager.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Persistentdata {

const String PersistentDataBlockManager::TAG("PersistentDataBlockManager");

CAR_INTERFACE_IMPL(PersistentDataBlockManager, Object, IPersistentDataBlockManager)

PersistentDataBlockManager::PersistentDataBlockManager()
{
}

ECode PersistentDataBlockManager::constructor(
    /* [in] */ IIPersistentDataBlockService* service)
{
    sService = service;
    return NOERROR;
}

ECode PersistentDataBlockManager::Write(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    ECode ec = sService->Write(data, ret);
    if (FAILED(ec)) {
        OnError(String("writing data"));
        *ret = -1;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode PersistentDataBlockManager::Read(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data)
    ECode ec = sService->Read(data);
    if (FAILED(ec)) {
        OnError(String("reading data"));
        *data = NULL;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode PersistentDataBlockManager::GetDataBlockSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    ECode ec = sService->GetDataBlockSize(size);
    if (FAILED(ec)) {
        OnError(String("getting data block size"));
        *size = -1;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode PersistentDataBlockManager::GetMaximumDataBlockSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    ECode ec = sService->GetMaximumDataBlockSize(size);
    if (FAILED(ec)) {
        OnError(String("getting maximum data block size"));
        *size = -1;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode PersistentDataBlockManager::Wipe()
{
    ECode ec = sService->Wipe();
    if (FAILED(ec)) {
        OnError(String("wiping persistent partition"));
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode PersistentDataBlockManager::SetOemUnlockEnabled(
    /* [in] */ Boolean enabled)
{
    ECode ec = sService->SetOemUnlockEnabled(enabled);
    if (FAILED(ec)) {
        StringBuilder sb("setting OEM unlock enabled to ");
        sb += enabled;
        OnError(sb.ToString());
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode PersistentDataBlockManager::GetOemUnlockEnabled(
    /* [out] */ Boolean* isenabled)
{
    VALIDATE_NOT_NULL(isenabled)
    ECode ec = sService->GetOemUnlockEnabled(isenabled);
    if (FAILED(ec)) {
        OnError(String("getting OEM unlock enabled bit"));
        *isenabled = FALSE;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

void PersistentDataBlockManager::OnError(
    /* [in] */ const String& msg)
{
    Slogger::V(TAG, String("Remote exception while ") + msg);
}

} // namespace Persistentdata
} // namespace Service
} // namepsace Droid
} // namespace Elastos
