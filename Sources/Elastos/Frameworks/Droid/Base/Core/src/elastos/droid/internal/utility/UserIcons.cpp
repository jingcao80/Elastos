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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/utility/UserIcons.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_IN;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

const Int32 UserIcons::USER_ICON_COLORS[8] = {
    R::color::user_icon_1,
    R::color::user_icon_2,
    R::color::user_icon_3,
    R::color::user_icon_4,
    R::color::user_icon_5,
    R::color::user_icon_6,
    R::color::user_icon_7,
    R::color::user_icon_8
};

ECode UserIcons::ConvertToBitmap(
    /* [in] */ IDrawable* icon,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap)
    *bitmap = NULL;

    if (icon == NULL) {
        return NOERROR;
    }
    Int32 width, height;
    icon->GetIntrinsicWidth(&width);
    icon->GetIntrinsicHeight(&height);
    FAIL_RETURN(CBitmap::CreateBitmap(width, height, BitmapConfig_ARGB_8888, bitmap))
    AutoPtr<ICanvas> canvas;
    CCanvas::New(*bitmap, (ICanvas**)&canvas);
    return icon->Draw(canvas);
}

ECode UserIcons::GetDefaultUserIcon(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean light,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    Int32 colorResId = light ? R::color::user_icon_default_white : R::color::user_icon_default_gray;
    if (userId != IUserHandle::USER_NULL) {
        // Return colored icon instead
        colorResId = USER_ICON_COLORS[userId % 8/*USER_ICON_COLORS.length*/];
    }

    AutoPtr<IDrawable> d;
    CResources::GetSystem()->GetDrawable(R::drawable::ic_account_circle, (IDrawable**)&d);
    d->Mutate();
    Int32 color;
    CResources::GetSystem()->GetColor(colorResId, &color);
    d->SetColorFilter(color, PorterDuffMode_SRC_IN);
    Int32 width, height;
    d->GetIntrinsicWidth(&width);
    d->GetIntrinsicHeight(&height);
    d->SetBounds(0, 0, width, height);
    *drawable = d;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
