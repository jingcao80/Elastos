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

#ifndef __ELASTOS_DROID_SERVER_PM_PACKAGEVERIFICATIONSTATE_H__
#define __ELASTOS_DROID_SERVER_PM_PACKAGEVERIFICATIONSTATE_H__

#include "elastos/droid/server/pm/CPackageManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class InstallArgs;

class PackageVerificationState : public Object
{
public:
    /**
     * Create a new package verification state where {@code requiredVerifierUid}
     * is the user ID for the package that must reply affirmative before things
     * can continue.
     *
     * @param requiredVerifierUid user ID of required package verifier
     * @param args
     */
    PackageVerificationState(
        /* [in] */ Int32 requiredVerifierUid,
        /* [in] */ InstallArgs* args);

    CARAPI_(AutoPtr<InstallArgs>) GetInstallArgs();

    CARAPI_(void) AddSufficientVerifier(
        /* [in] */ Int32 uid);

    CARAPI_(Boolean) SetVerifierResponse(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 code);

    CARAPI_(Boolean) IsVerificationComplete();

    CARAPI_(Boolean) IsInstallAllowed();

    CARAPI_(void) ExtendTimeout();

    CARAPI_(Boolean) TimeoutExtended();

private:
    AutoPtr<InstallArgs> mArgs;

    HashMap<Int32, Boolean> mSufficientVerifierUids;

    Int32 mRequiredVerifierUid;

    Boolean mSufficientVerificationComplete;

    Boolean mSufficientVerificationPassed;

    Boolean mRequiredVerificationComplete;

    Boolean mRequiredVerificationPassed;

    Boolean mExtendedTimeout;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PACKAGEVERIFICATIONSTATE_H__
