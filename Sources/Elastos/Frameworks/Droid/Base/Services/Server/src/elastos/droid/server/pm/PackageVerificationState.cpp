
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
            if (mSufficientVerifierUids.Begin() == mSufficientVerifierUids.End()) {
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

    if (mSufficientVerifierUids.Begin() == mSufficientVerifierUids.End()) {
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
