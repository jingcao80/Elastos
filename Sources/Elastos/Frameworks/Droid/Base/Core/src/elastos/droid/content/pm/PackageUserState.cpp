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
#include "elastos/droid/content/pm/PackageUserState.h"
#include <Elastos.CoreLibrary.h>

using Elastos::Droid::Content::Pm::IPackageManager;
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
    mProtectedComponents = orig->mProtectedComponents != NULL
            ? new HashSet<String>(*(orig->mProtectedComponents.Get())) : NULL;
    mVisibleComponents = orig->mVisibleComponents != NULL
            ? new HashSet<String>(*(orig->mVisibleComponents.Get())) : NULL;
}

PackageUserState::~PackageUserState()
{
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
