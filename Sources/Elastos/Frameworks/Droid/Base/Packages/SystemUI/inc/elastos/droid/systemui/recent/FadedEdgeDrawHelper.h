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

#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_FADEDEDGEDRAWHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_FADEDEDGEDRAWHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::ILinearGradient;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

class FadedEdgeDrawHelper
    : public Object
{
public:
    static CARAPI_(AutoPtr<FadedEdgeDrawHelper>) Create(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IView* scrollView,
        /* [in] */ Boolean isVertical);

    FadedEdgeDrawHelper(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IView* scrollView,
        /* [in] */ Boolean isVertical);

    CARAPI OnAttachedToWindowCallback(
        /* [in] */ ILinearLayout* layout,
        /* [in] */ Boolean hardwareAccelerated);

    CARAPI AddViewCallback(
        /* [in] */ IView* newLinearLayoutChild);

    CARAPI DrawCallback(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 top,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Float topFadingEdgeStrength,
        /* [in] */ Float bottomFadingEdgeStrength,
        /* [in] */ Float leftFadingEdgeStrength,
        /* [in] */ Float rightFadingEdgeStrength,
        /* [in] */ Int32 mPaddingTop);

    CARAPI GetVerticalFadingEdgeLength(
        /* [out] */ Int32* length);

    CARAPI GetHorizontalFadingEdgeLength(
        /* [out] */ Int32* length);

public:
    static const Boolean OPTIMIZE_SW_RENDERED_RECENTS;
    static const Boolean USE_DARK_FADE_IN_HW_ACCELERATED_MODE;

private:
    AutoPtr<IView> mScrollView;

    Int32 mFadingEdgeLength;
    Boolean mIsVertical;
    Boolean mSoftwareRendered;
    AutoPtr<IPaint> mBlackPaint;
    AutoPtr<IPaint> mFadePaint;
    AutoPtr<IMatrix> mFadeMatrix;
    AutoPtr<ILinearGradient> mFade;

};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_FADEDEDGEDRAWHELPER_H__