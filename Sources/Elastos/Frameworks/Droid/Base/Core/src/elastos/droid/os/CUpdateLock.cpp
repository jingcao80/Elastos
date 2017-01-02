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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CUpdateLock.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CBinder.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

const Boolean CUpdateLock::DEBUG = FALSE;
const String CUpdateLock::TAG("UpdateLock");

AutoPtr<IIUpdateLock> CUpdateLock::sService;

CAR_INTERFACE_IMPL(CUpdateLock, Object, IUpdateLock)

CAR_OBJECT_IMPL(CUpdateLock)

void CUpdateLock::CheckService()
{
    if (sService == NULL) {
        sService = IIUpdateLock::Probe(ServiceManager::GetService(IContext::UPDATE_LOCK_SERVICE).Get());
    }
}

CUpdateLock::CUpdateLock()
    : mCount(0)
    , mRefCounted(TRUE)
    , mHeld(FALSE)
{
}

ECode CUpdateLock::constructor(
    /* [in] */ const String& tag)
{
    mTag = tag;
    CBinder::New((IBinder**)&mToken);
    return NOERROR;
}

ECode CUpdateLock::SetReferenceCounted(
    /* [in] */ Boolean isRefCounted)
{
    if (DEBUG) {
        // Logger::V(TAG, "setting refcounted=" + isRefCounted + " : " + this);
    }
    mRefCounted = isRefCounted;
    return NOERROR;
}

ECode CUpdateLock::IsHeld(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock lock(mLock);
        *result = mHeld;
    }
    return NOERROR;
}

ECode CUpdateLock::AcquireLock()
{
    if (DEBUG) {
        // Log.v(TAG, "acquire() : " + this, new RuntimeException("here"));
    }
    CheckService();
    {
        AutoLock lock(mLock);
        AcquireLocked();
    }
    return NOERROR;
}

ECode CUpdateLock::AcquireLocked()
{
    if (!mRefCounted || mCount++ == 0) {
        if (sService != NULL) {
            ECode ec = sService->AcquireUpdateLock(mToken, mTag);
            if (FAILED(ec)) {
                Logger::E(TAG, "Unable to contact service to acquire");
                return E_REMOTE_EXCEPTION;
            }
        }
        mHeld = TRUE;
    }
    return NOERROR;
}

ECode CUpdateLock::ReleaseLock()
{
    if (DEBUG) {
        // Log.v(TAG, "release() : " + this, new RuntimeException("here"));
    }
    CheckService();
    {
        AutoLock lock(mLock);
        ReleaseLocked();
    }
    return NOERROR;
}

ECode CUpdateLock::ReleaseLocked()
{
    if (!mRefCounted || --mCount == 0) {
        if (sService != NULL) {
            ECode ec = sService->ReleaseUpdateLock(mToken);
            if (FAILED(ec)) {
                Logger::E(TAG, "Unable to contact service to release");
                return E_REMOTE_EXCEPTION;
            }
        }
        mHeld = FALSE;
    }
    if (mCount < 0) {
        Logger::E(TAG, "UpdateLock under-locked");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

CUpdateLock::~CUpdateLock()
{
    AutoLock lock(mLock);
    // if mHeld is true, sService must be non-null
    if (mHeld) {
        // Log.wtf(TAG, "UpdateLock finalized while still held");
        ECode ec = sService->ReleaseUpdateLock(mToken);
        if (FAILED(ec)) {
            Logger::E(TAG, "Unable to contact service to release");
            // return E_REMOTE_EXCEPTION;
        }
    }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
