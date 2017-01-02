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

#ifndef  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICOUTLINEHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICOUTLINEHELPER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IBlurMaskFilter;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class HolographicOutlineHelper
    : public Object
{
public:
    HolographicOutlineHelper();

    /**
     * Returns the interpolated holographic highlight alpha for the effect we want when scrolling
     * pages.
     */
    static CARAPI_(Float) HighlightAlphaInterpolator(
        /* [in] */ Float r);

    /**
     * Returns the interpolated view alpha for the effect we want when scrolling pages.
     */
    static CARAPI_(Float) ViewAlphaInterpolator(
        /* [in] */ Float r);

    /**
     * Applies a more expensive and accurate outline to whatever is currently drawn in a specified
     * bitmap.
     */
    CARAPI ApplyExpensiveOutlineWithBlur(
        /* [in] */ IBitmap* srcDst,
        /* [in] */ ICanvas* srcDstCanvas,
        /* [in] */ Int32 color,
        /* [in] */ Int32 outlineColor,
        /* [in] */ Int32 thickness);

    CARAPI ApplyExpensiveOutlineWithBlur(
        /* [in] */ IBitmap* srcDst,
        /* [in] */ ICanvas* srcDstCanvas,
        /* [in] */ Int32 color,
        /* [in] */ Int32 outlineColor,
        /* [in] */ Boolean clipAlpha,
        /* [in] */ Int32 thickness);

    CARAPI ApplyExtraThickExpensiveOutlineWithBlur(
        /* [in] */ IBitmap* srcDst,
        /* [in] */ ICanvas* srcDstCanvas,
        /* [in] */ Int32 color,
        /* [in] */ Int32 outlineColor);

    CARAPI ApplyThickExpensiveOutlineWithBlur(
        /* [in] */ IBitmap* srcDst,
        /* [in] */ ICanvas* srcDstCanvas,
        /* [in] */ Int32 color,
        /* [in] */ Int32 outlineColor);

    CARAPI ApplyMediumExpensiveOutlineWithBlur(
        /* [in] */ IBitmap* srcDst,
        /* [in] */ ICanvas* srcDstCanvas,
        /* [in] */ Int32 color,
        /* [in] */ Int32 outlineColor,
        /* [in] */ Boolean clipAlpha);

    CARAPI ApplyMediumExpensiveOutlineWithBlur(
        /* [in] */ IBitmap* srcDst,
        /* [in] */ ICanvas* srcDstCanvas,
        /* [in] */ Int32 color,
        /* [in] */ Int32 outlineColor);

    static CARAPI_(Int32) GetMAX_OUTER_BLUR_RADIUS();

    static CARAPI_(Int32) GetMIN_OUTER_BLUR_RADIUS();

private:
    CARAPI_(AutoPtr<IBlurMaskFilter>) GetExtraThickOuterBlurMaskFilter();

    CARAPI_(AutoPtr<IBlurMaskFilter>) GetThickOuterBlurMaskFilter();

    CARAPI_(AutoPtr<IBlurMaskFilter>) GetMediumOuterBlurMaskFilter();

    CARAPI_(AutoPtr<IBlurMaskFilter>) GetThinOuterBlurMaskFilter();

    CARAPI_(AutoPtr<IBlurMaskFilter>) GetThickInnerBlurMaskFilter();

    CARAPI_(AutoPtr<IBlurMaskFilter>) GetExtraThickInnerBlurMaskFilter();

    CARAPI_(AutoPtr<IBlurMaskFilter>) GetMediumInnerBlurMaskFilter();

private:
    static /*const*/ Int32 MAX_OUTER_BLUR_RADIUS;
    static /*const*/ Int32 MIN_OUTER_BLUR_RADIUS;

    AutoPtr<IPaint> mHolographicPaint;
    AutoPtr<IPaint> mBlurPaint;
    AutoPtr<IPaint> mErasePaint;

    static AutoPtr<IBlurMaskFilter> sExtraThickOuterBlurMaskFilter;
    static AutoPtr<IBlurMaskFilter> sThickOuterBlurMaskFilter;
    static AutoPtr<IBlurMaskFilter> sMediumOuterBlurMaskFilter;
    static AutoPtr<IBlurMaskFilter> sThinOuterBlurMaskFilter;
    static AutoPtr<IBlurMaskFilter> sThickInnerBlurMaskFilter;
    static AutoPtr<IBlurMaskFilter> sExtraThickInnerBlurMaskFilter;
    static AutoPtr<IBlurMaskFilter> sMediumInnerBlurMaskFilter;

    static const Int32 THICK;
    static const Int32 MEDIUM;
    static const Int32 EXTRA_THICK;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICOUTLINEHELPER_H__