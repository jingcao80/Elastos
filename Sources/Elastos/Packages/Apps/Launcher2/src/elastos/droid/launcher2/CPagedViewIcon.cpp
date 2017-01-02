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

#include "elastos/droid/launcher2/CPagedViewIcon.h"
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "elastos/droid/launcher2/CFastBitmapDrawable.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CFastBitmapDrawable;
using Elastos::Droid::App::IFastBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CPagedViewIcon::MyRunnable::MyRunnable(
    /* [in] */ CPagedViewIcon* host)
    : mHost(host)
{
}

ECode CPagedViewIcon::MyRunnable::Run()
{
    return mHost->RefreshDrawableState();
}

const String CPagedViewIcon::TAG("PagedViewIcon");
const Float CPagedViewIcon::PRESS_ALPHA = 0.4f;

CAR_INTERFACE_IMPL(CPagedViewIcon, TextView, IPagedViewIcon);

CAR_OBJECT_IMPL(CPagedViewIcon);

CPagedViewIcon::CPagedViewIcon()
    : mLockDrawableState(FALSE)
{}

ECode CPagedViewIcon::ApplyFromApplicationInfo(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Boolean scaleUp,
    /* [in] */ IPagedViewIconPressedCallback* cb)
{
    ApplicationInfo* _info = (ApplicationInfo*)info;
    mIcon = _info->mIconBitmap;
    mPressedCallback = cb;
    AutoPtr<IDrawable> drawable;
    CFastBitmapDrawable::New(mIcon, (IDrawable**)&drawable);
    SetCompoundDrawablesWithIntrinsicBounds(NULL, drawable.Get(), NULL, NULL);
    SetText(_info->mTitle);
    if (_info->mContentDescription != NULL) {
        SetContentDescription(_info->mContentDescription);
    }
    return SetTag(info);
}

ECode CPagedViewIcon::LockDrawableState()
{
    mLockDrawableState = TRUE;
    return NOERROR;
}

ECode CPagedViewIcon::ResetDrawableState()
{
    mLockDrawableState = FALSE;
    AutoPtr<MyRunnable> run = new MyRunnable(this);
    Boolean res;
    return Post(run, &res);
}

ECode CPagedViewIcon::DrawableStateChanged()
{
    FAIL_RETURN(TextView::DrawableStateChanged());

    // We keep in the pressed state until resetDrawableState() is called to reset the press
    // feedback
    Boolean res;
    IsPressed(&res);
    if (res) {
        SetAlpha(PRESS_ALPHA);
        if (mPressedCallback != NULL) {
            mPressedCallback->IconPressed(this);
        }
    }
    else if (!mLockDrawableState) {
        SetAlpha(1.0f);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos
