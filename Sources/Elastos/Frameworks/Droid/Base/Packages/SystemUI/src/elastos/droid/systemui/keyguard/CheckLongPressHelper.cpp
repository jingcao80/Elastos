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

#include "elastos/droid/systemui/keyguard/CheckLongPressHelper.h"
#include <elastos/core/Math.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CheckLongPressHelper::CheckForLongPress::Run()
{
    AutoPtr<IViewParent> parent;
    mHost->mView->GetParent((IViewParent**)&parent);
    Boolean res;
    if ((parent != NULL) && (mHost->mView->HasWindowFocus(&res), res)
            && !mHost->mHasPerformedLongPress) {

        if (mHost->mView->PerformLongClick(&res), res) {
            mHost->mView->SetPressed(FALSE);
            mHost->mHasPerformedLongPress = TRUE;
        }
    }
    return NOERROR;
}

CheckLongPressHelper::CheckLongPressHelper(
    /* [in] */ IView* v)
    : mView(v)
    , mHasPerformedLongPress(FALSE)
    , mDownX(0.0)
    , mDownY(0.0)
{
    AutoPtr<IContext> context;
    v->GetContext((IContext**)&context);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> viewConfig;
    helper->Get(context, (IViewConfiguration**)&viewConfig);
    viewConfig->GetScaledTouchSlop(&mScaledTouchSlop);
    helper->GetLongPressTimeout(&mLongPressTimeout);
}

ECode CheckLongPressHelper::PostCheckForLongPress(
    /* [in] */ IMotionEvent* ev)
{
    ev->GetX(&mDownX);
    ev->GetY(&mDownY);
    mHasPerformedLongPress = FALSE;

    if (mPendingCheckForLongPress == NULL) {
        mPendingCheckForLongPress = new CheckForLongPress(this);
    }
    Boolean res;
    return mView->PostDelayed(IRunnable::Probe(mPendingCheckForLongPress), mLongPressTimeout, &res);
}

ECode CheckLongPressHelper::OnMove(
    /* [in] */ IMotionEvent* ev)
{
    Float x;
    ev->GetX(&x);
    Float y;
    ev->GetY(&y);
    Boolean xMoved = Elastos::Core::Math::Abs(mDownX - x) > mScaledTouchSlop;
    Boolean yMoved = Elastos::Core::Math::Abs(mDownY - y) > mScaledTouchSlop;

    if (xMoved || yMoved) {
        CancelLongPress();
    }
    return NOERROR;
}

ECode CheckLongPressHelper::CancelLongPress()
{
    mHasPerformedLongPress = FALSE;
    if (mPendingCheckForLongPress != NULL) {
        Boolean res;
        mView->RemoveCallbacks(
                IRunnable::Probe(mPendingCheckForLongPress), &res);
        mPendingCheckForLongPress = NULL;
    }
    return NOERROR;
}

ECode CheckLongPressHelper::HasPerformedLongPress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHasPerformedLongPress;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
