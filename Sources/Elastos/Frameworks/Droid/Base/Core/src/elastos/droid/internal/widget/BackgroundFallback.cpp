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

#include <Elastos.Droid.View.h>
#include "elastos/droid/internal/widget/BackgroundFallback.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IPixelFormat;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(BackgroundFallback, Object, IBackgroundFallback);

BackgroundFallback::BackgroundFallback()
{}

BackgroundFallback::~BackgroundFallback()
{}

ECode BackgroundFallback::constructor()
{
    return NOERROR;
}

ECode BackgroundFallback::SetDrawable(
    /* [in] */ IDrawable* d)
{
    mBackgroundFallback = d;
    return NOERROR;
}

ECode BackgroundFallback::HasFallback(
    /* [out] */ Boolean* hasFallback)
{
    VALIDATE_NOT_NULL(hasFallback);
    *hasFallback = mBackgroundFallback != NULL;
    return NOERROR;
}

ECode BackgroundFallback::Draw(
    /* [in] */ IViewGroup* root,
    /* [in] */ ICanvas* c,
    /* [in] */ IView* content)
{
    Boolean res;
    if (HasFallback(&res), !res) {
        return NOERROR;
    }

    // Draw the fallback in the padding.
    Int32 width;
    IView::Probe(root)->GetWidth(&width);
    Int32 height;
    IView::Probe(root)->GetHeight(&height);
    Int32 left = width;
    Int32 top = height;
    Int32 right = 0;
    Int32 bottom = 0;

    Int32 childCount;
    root->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        root->GetChildAt(i, (IView**)&child);
        AutoPtr<IDrawable> childBg;
        child->GetBackground((IDrawable**)&childBg);
        Int32 visibility, opacity;
        if (child.Get() == content) {
            // We always count the content view container unless it has no background
            // and no children.
            Int32 count;
            if (childBg == NULL && IViewGroup::Probe(child) &&
                    (IViewGroup::Probe(child)->GetChildCount(&count), count) == 0) {
                continue;
            }
        }
        else if ((child->GetVisibility(&visibility), visibility) != IView::VISIBLE || childBg == NULL ||
                (childBg->GetOpacity(&opacity), opacity) != IPixelFormat::OPAQUE) {
            // Potentially translucent or invisible children don't count, and we assume
            // the content view will cover the whole area if we're in a background
            // fallback situation.
            continue;
        }

        using Elastos::Core::Math;
        Int32 _left, _top, _right, _bottom;
        child->GetLeft(&_left);
        child->GetTop(&_top);
        child->GetRight(&_right);
        child->GetBottom(&_bottom);

        left = Math::Min(left, _left);
        top = Math::Min(top, _top);
        right = Math::Max(right, _right);
        bottom = Math::Max(bottom, _bottom);
    }

    if (left >= right || top >= bottom) {
        // No valid area to draw in.
        return NOERROR;
    }

    if (top > 0) {
        mBackgroundFallback->SetBounds(0, 0, width, top);
        mBackgroundFallback->Draw(c);
    }
    if (left > 0) {
        mBackgroundFallback->SetBounds(0, top, left, height);
        mBackgroundFallback->Draw(c);
    }
    if (right < width) {
        mBackgroundFallback->SetBounds(right, top, width, height);
        mBackgroundFallback->Draw(c);
    }
    if (bottom < height) {
        mBackgroundFallback->SetBounds(left, bottom, right, height);
        mBackgroundFallback->Draw(c);
    }
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
