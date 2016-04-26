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

private:
    static CARAPI_(Boolean) InitStaticBlock();

public:
    static /*const*/ Int32 MAX_OUTER_BLUR_RADIUS;
    static /*const*/ Int32 MIN_OUTER_BLUR_RADIUS;

private:
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

    static Boolean initStaticBlock;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICOUTLINEHELPER_H__