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

#include "elastos/droid/systemui/recents/views/FixedSizeImageView.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CAR_INTERFACE_IMPL(FixedSizeImageView, ImageView, IFixedSizeImageView)

FixedSizeImageView::FixedSizeImageView()
    : mAllowRelayout(TRUE)
    , mAllowInvalidate(TRUE)
{}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return ImageView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

// @Override
ECode FixedSizeImageView::RequestLayout()
{
    if (mAllowRelayout) {
        ImageView::RequestLayout();
    }
    return NOERROR;
}

// @Override
ECode FixedSizeImageView::Invalidate()
{
    if (mAllowInvalidate) {
        ImageView::Invalidate();
    }
    return NOERROR;
}

// @Override
ECode FixedSizeImageView::SetImageDrawable(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IBitmap> bitmap;
    AutoPtr<IBitmapDrawable> bd = IBitmapDrawable::Probe(drawable);
    if (bd != NULL) {
        bd->GetBitmap((IBitmap**)&bitmap);
    }
    if (drawable == NULL || (bitmap == NULL)) {
        mAllowRelayout = FALSE;
        mAllowInvalidate = FALSE;
    }
    ImageView::SetImageDrawable(drawable);
    mAllowRelayout = TRUE;
    mAllowInvalidate = TRUE;
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
