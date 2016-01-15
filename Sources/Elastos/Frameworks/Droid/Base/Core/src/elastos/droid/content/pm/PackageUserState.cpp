
#include "elastos/droid/content/pm/PackageUserState.h"
#include <Elastos.CoreLibrary.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

PackageUserState::PackageUserState()
    : mStopped(FALSE)
    , mNotLaunched(FALSE)
    , mInstalled(TRUE)
    , mHidden(FALSE)
    , mEnabled(IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT)
    , mBlockUninstall(FALSE)
{}

PackageUserState::PackageUserState(
    /* [in] */ PackageUserState* orig)
{
    assert(orig != NULL);
    mStopped = orig->mStopped;
    mNotLaunched = orig->mNotLaunched;
    mInstalled = orig->mInstalled;
    mEnabled = orig->mEnabled;
    mHidden = orig->mHidden;
    mLastDisableAppCaller = orig->mLastDisableAppCaller;

    if (orig->mDisabledComponents != NULL) {
        mDisabledComponents = new HashSet<String>(orig->mDisabledComponents->Begin(),
            orig->mDisabledComponents->End());
    }

    if (orig->mEnabledComponents != NULL) {
        mEnabledComponents = new HashSet<String>(orig->mEnabledComponents->Begin(),
            orig->mEnabledComponents->End());
    }

    mBlockUninstall = orig->mBlockUninstall;
}

PackageUserState::~PackageUserState()
{
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
