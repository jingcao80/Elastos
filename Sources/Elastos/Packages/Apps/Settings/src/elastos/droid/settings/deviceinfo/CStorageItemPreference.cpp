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

#include "elastos/droid/settings/deviceinfo/CStorageItemPreference.h"
#include "../R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::CShapeDrawable;
using Elastos::Droid::Graphics::Drawable::Shapes::IShape;
using Elastos::Droid::Graphics::Drawable::Shapes::CRectShape;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

CAR_OBJECT_IMPL(CStorageItemPreference)

CStorageItemPreference::CStorageItemPreference()
    : mColor(0)
    , mUserHandle(0)
{}

CStorageItemPreference::~CStorageItemPreference()
{}

ECode CStorageItemPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 titleRes,
    /* [in] */ Int32 colorRes)
{
    AutoPtr<ICharSequence> seq;
    context->GetText(titleRes, (ICharSequence**)&seq);
    return constructor(context, seq, colorRes, IUserHandle::USER_NULL);
}

ECode CStorageItemPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* title,
    /* [in] */ Int32 colorRes,
    /* [in] */ Int32 userHandle)
{
    Preference::constructor(context);

    if (colorRes != 0) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetColor(colorRes, &mColor);

        Int32 width;
        res->GetDimensionPixelSize(R::dimen::device_memory_usage_button_width, &width);
        Int32 height;
        res->GetDimensionPixelSize(R::dimen::device_memory_usage_button_height, &height);
        SetIcon(IDrawable::Probe(CreateRectShape(width, height, mColor)));
    }
    else {
        mColor = IColor::MAGENTA;
    }

    SetTitle(title);
    SetSummary(R::string::memory_calculating_size);

    mUserHandle = userHandle;
    return NOERROR;
}

AutoPtr<IShapeDrawable> CStorageItemPreference::CreateRectShape(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 color)
{
    AutoPtr<IShape> rs;
    CRectShape::New((IShape**)&rs);
    AutoPtr<IShapeDrawable> shape;
    CShapeDrawable::New(rs, (IShapeDrawable**)&shape);
    shape->SetIntrinsicHeight(height);
    shape->SetIntrinsicWidth(width);
    AutoPtr<IPaint> paint;
    shape->GetPaint((IPaint**)&paint);
    paint->SetColor(color);
    return shape;
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos