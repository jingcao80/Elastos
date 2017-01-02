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

#include "elastos/droid/launcher2/CheckLongPressHelper.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "Elastos.Droid.Service.h"

using Elastos::Droid::View::IViewParent;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CheckLongPressHelper::CheckForLongPress::CheckForLongPress(
    /* [in] */ CheckLongPressHelper* host)
    : mHost(host)
{
}

ECode CheckLongPressHelper::CheckForLongPress::Run()
{
    AutoPtr<IViewParent> parent;
    mHost->mView->GetParent((IViewParent**)&parent);
    if (parent != NULL) {
        Boolean res;
        mHost->mView->HasWindowFocus(&res);
        if (res) {
            if (!mHost->mHasPerformedLongPress) {
                Boolean tmp;
                mHost->mView->PerformLongClick(&tmp);
                if (tmp) {
                    mHost->mView->SetPressed(FALSE);
                    mHost->mHasPerformedLongPress = TRUE;
                }
            }
        }
    }
    return NOERROR;
}

CheckLongPressHelper::CheckLongPressHelper(
    /* [in] */ IView* v)
    : mView(v)
    , mHasPerformedLongPress(FALSE)
{
}

ECode CheckLongPressHelper::PostCheckForLongPress()
{
    mHasPerformedLongPress = FALSE;

    if (mPendingCheckForLongPress == NULL) {
        mPendingCheckForLongPress = new CheckForLongPress(this);
    }

    Int32 timeout;
    LauncherApplication::GetLongPressTimeout(&timeout);
    Boolean res;
    return mView->PostDelayed(mPendingCheckForLongPress, timeout, &res);
}

void CheckLongPressHelper::CancelLongPress()
{
    mHasPerformedLongPress = FALSE;
    if (mPendingCheckForLongPress != NULL) {
        Boolean res;
        mView->RemoveCallbacks(mPendingCheckForLongPress, &res);
        mPendingCheckForLongPress = NULL;
    }
    return;
}

Boolean CheckLongPressHelper::HasPerformedLongPress()
{
    return mHasPerformedLongPress;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos