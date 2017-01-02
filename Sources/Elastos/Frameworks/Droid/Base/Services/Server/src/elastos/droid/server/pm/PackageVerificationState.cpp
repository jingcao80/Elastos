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

#include "elastos/droid/server/pm/PackageVerificationState.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

PackageVerificationState::PackageVerificationState(
    /* [in] */ Int32 requiredVerifierUid,
    /* [in] */ InstallArgs* args)
    : mArgs(args)
    , mSufficientVerifierUids(10)
    , mRequiredVerifierUid(requiredVerifierUid)
    , mSufficientVerificationComplete(FALSE)
    , mSufficientVerificationPassed(FALSE)
    , mRequiredVerificationComplete(FALSE)
    , mRequiredVerificationPassed(FALSE)
    , mExtendedTimeout(FALSE)
{}

AutoPtr<InstallArgs> PackageVerificationState::GetInstallArgs()
{
    return mArgs;
}

void PackageVerificationState::AddSufficientVerifier(
    /* [in] */ Int32 uid)
{
    mSufficientVerifierUids[uid] = TRUE;
}

Boolean PackageVerificationState::SetVerifierResponse(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 code)
{
    if (uid == mRequiredVerifierUid) {
        mRequiredVerificationComplete = TRUE;
        switch (code) {
            case IPackageManager::VERIFICATION_ALLOW_WITHOUT_SUFFICIENT:
                mSufficientVerifierUids.Clear();
                // fall through
            case IPackageManager::VERIFICATION_ALLOW:
                mRequiredVerificationPassed = TRUE;
                break;
            default:
                mRequiredVerificationPassed = FALSE;
        }
        return TRUE;
    }
    else {
        HashMap<Int32, Boolean>::Iterator it = mSufficientVerifierUids.Find(uid);
        if (it != mSufficientVerifierUids.End()) {
            if (code == IPackageManager::VERIFICATION_ALLOW) {
                mSufficientVerificationComplete = TRUE;
                mSufficientVerificationPassed = TRUE;
            }

            mSufficientVerifierUids.Erase(it);
            if (mSufficientVerifierUids.IsEmpty()) {
                mSufficientVerificationComplete = TRUE;
            }

            return TRUE;
        }
    }

    return FALSE;
}

Boolean PackageVerificationState::IsVerificationComplete()
{
    if (!mRequiredVerificationComplete) {
        return FALSE;
    }

    if (mSufficientVerifierUids.IsEmpty()) {
        return TRUE;
    }

    return mSufficientVerificationComplete;
}

Boolean PackageVerificationState::IsInstallAllowed()
{
    if (!mRequiredVerificationPassed) {
        return FALSE;
    }

    if (mSufficientVerificationComplete) {
        return mSufficientVerificationPassed;
    }

    return TRUE;
}

void PackageVerificationState::ExtendTimeout()
{
    if (!mExtendedTimeout) {
        mExtendedTimeout = TRUE;
    }
}

Boolean PackageVerificationState::TimeoutExtended()
{
    return mExtendedTimeout;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
