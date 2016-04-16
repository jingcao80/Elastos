
#include "elastos/droid/launcher2/HolographicOutlineHelper.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Graphics::IMaskFilter;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CBlurMaskFilter;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IPorterDuffXfermode;;
using Elastos::Droid::Graphics::BlurMaskFilterBlur_NORMAL;
using Elastos::Droid::Graphics::BlurMaskFilterBlur_OUTER;
using Elastos::Droid::Graphics::PorterDuffMode_DST_OUT;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_OUT;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/*const*/ Int32 HolographicOutlineHelper::MAX_OUTER_BLUR_RADIUS;
/*const*/ Int32 HolographicOutlineHelper::MIN_OUTER_BLUR_RADIUS;

AutoPtr<IBlurMaskFilter> HolographicOutlineHelper::sExtraThickOuterBlurMaskFilter;
AutoPtr<IBlurMaskFilter> HolographicOutlineHelper::sThickOuterBlurMaskFilter;
AutoPtr<IBlurMaskFilter> HolographicOutlineHelper::sMediumOuterBlurMaskFilter;
AutoPtr<IBlurMaskFilter> HolographicOutlineHelper::sThinOuterBlurMaskFilter;
AutoPtr<IBlurMaskFilter> HolographicOutlineHelper::sThickInnerBlurMaskFilter;
AutoPtr<IBlurMaskFilter> HolographicOutlineHelper::sExtraThickInnerBlurMaskFilter;
AutoPtr<IBlurMaskFilter> HolographicOutlineHelper::sMediumInnerBlurMaskFilter;

const Int32 HolographicOutlineHelper::THICK = 0;
const Int32 HolographicOutlineHelper::MEDIUM = 1;
const Int32 HolographicOutlineHelper::EXTRA_THICK = 2;

Boolean HolographicOutlineHelper::InitStaticBlock()
{
    Float scale;
    assert(0 && "need class LauncherApplication");
    //LauncherApplication::GetScreenDensity(&scale);

    MIN_OUTER_BLUR_RADIUS = (Int32)(scale * 1.0f);
    MAX_OUTER_BLUR_RADIUS = (Int32)(scale * 12.0f);

    CBlurMaskFilter::New(scale * 12.0f, BlurMaskFilterBlur_OUTER,
            (IBlurMaskFilter**)&sExtraThickOuterBlurMaskFilter);
    CBlurMaskFilter::New(scale * 6.0f, BlurMaskFilterBlur_OUTER,
            (IBlurMaskFilter**)&sThickOuterBlurMaskFilter);
    CBlurMaskFilter::New(scale * 2.0f, BlurMaskFilterBlur_OUTER,
            (IBlurMaskFilter**)&sMediumOuterBlurMaskFilter);
    CBlurMaskFilter::New(scale * 1.0f, BlurMaskFilterBlur_OUTER,
            (IBlurMaskFilter**)&sThinOuterBlurMaskFilter);
    CBlurMaskFilter::New(scale * 6.0f, BlurMaskFilterBlur_NORMAL,
            (IBlurMaskFilter**)&sExtraThickInnerBlurMaskFilter);
    CBlurMaskFilter::New(scale * 4.0f, BlurMaskFilterBlur_NORMAL,
            (IBlurMaskFilter**)&sThickInnerBlurMaskFilter);
    CBlurMaskFilter::New(scale * 2.0f, BlurMaskFilterBlur_NORMAL,
            (IBlurMaskFilter**)&sMediumInnerBlurMaskFilter);

    return TRUE;
}

Boolean HolographicOutlineHelper::initStaticBlock = InitStaticBlock();

HolographicOutlineHelper::HolographicOutlineHelper()
{
    CPaint::New((IPaint**)&mHolographicPaint);
    CPaint::New((IPaint**)&mBlurPaint);
    CPaint::New((IPaint**)&mErasePaint);

    mHolographicPaint->SetFilterBitmap(TRUE);
    mHolographicPaint->SetAntiAlias(TRUE);
    mBlurPaint->SetFilterBitmap(TRUE);
    mBlurPaint->SetAntiAlias(TRUE);
    AutoPtr<IPorterDuffXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_DST_OUT, (IPorterDuffXfermode**)&mode);
    mErasePaint->SetXfermode(IXfermode::Probe(mode));
    mErasePaint->SetFilterBitmap(TRUE);
    mErasePaint->SetAntiAlias(TRUE);
}

Float HolographicOutlineHelper::HighlightAlphaInterpolator(
    /* [in] */ Float r)
{
    Float maxAlpha = 0.6f;
    return (Float)Elastos::Core::Math::Pow(maxAlpha * (1.0f - r), 1.5f);
}

Float HolographicOutlineHelper::ViewAlphaInterpolator(
    /* [in] */ Float r)
{
    const Float pivot = 0.95f;
    if (r < pivot) {
        return (Float)Elastos::Core::Math::Pow(r / pivot, 1.5f);
    }
    else {
        return 1.0f;
    }
}

ECode HolographicOutlineHelper::ApplyExpensiveOutlineWithBlur(
    /* [in] */ IBitmap* srcDst,
    /* [in] */ ICanvas* srcDstCanvas,
    /* [in] */ Int32 color,
    /* [in] */ Int32 outlineColor,
    /* [in] */ Int32 thickness)
{
    return ApplyExpensiveOutlineWithBlur(srcDst, srcDstCanvas, color,
            outlineColor, TRUE, thickness);
}

ECode HolographicOutlineHelper::ApplyExpensiveOutlineWithBlur(
    /* [in] */ IBitmap* srcDst,
    /* [in] */ ICanvas* srcDstCanvas,
    /* [in] */ Int32 color,
    /* [in] */ Int32 outlineColor,
    /* [in] */ Boolean clipAlpha,
    /* [in] */ Int32 thickness)
{
    // We start by removing most of the alpha channel so as to ignore shadows, and
    // other types of partial transparency when defining the shape of the object
    if (clipAlpha) {
        Int32 width;
        srcDst->GetWidth(&width);
        Int32 height;
        srcDst->GetHeight(&height);
        AutoPtr<ArrayOf<Int32> > srcBuffer = ArrayOf<Int32>::Alloc(width * height);
        srcDst->GetPixels(srcBuffer, 0, width, 0, 0, width, height);
        for (Int32 i = 0; i < srcBuffer->GetLength(); i++) {
            assert(0 && "java >>> ?= c++ >>");
            const Int32 alpha = (*srcBuffer)[i] >> 24;
            if (alpha < 188) {
                (*srcBuffer)[i] = 0;
            }
        }
        srcDst->SetPixels(srcBuffer, 0, width, 0, 0, width, height);
    }
    AutoPtr<IBitmap> glowShape;
    srcDst->ExtractAlpha((IBitmap**)&glowShape);

    // calculate the outer blur first
    AutoPtr<IBlurMaskFilter> outerBlurMaskFilter;
    switch (thickness) {
        case EXTRA_THICK:
            outerBlurMaskFilter = sExtraThickOuterBlurMaskFilter;
            break;
        case THICK:
            outerBlurMaskFilter = sThickOuterBlurMaskFilter;
            break;
        case MEDIUM:
            outerBlurMaskFilter = sMediumOuterBlurMaskFilter;
            break;
        default:
            //throw new RuntimeException("Invalid blur thickness");
            Slogger::E("HolographicOutlineHelper", "Invalid blur thickness");
            return E_RUNTIME_EXCEPTION;
    }
    mBlurPaint->SetMaskFilter(IMaskFilter::Probe(outerBlurMaskFilter));
    AutoPtr<ArrayOf<Int32> > outerBlurOffset = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IBitmap> thickOuterBlur;
    glowShape->ExtractAlpha(mBlurPaint, outerBlurOffset, (IBitmap**)&thickOuterBlur);
    if (thickness == EXTRA_THICK) {
        mBlurPaint->SetMaskFilter(IMaskFilter::Probe(sMediumOuterBlurMaskFilter));
    }
    else {
        mBlurPaint->SetMaskFilter(IMaskFilter::Probe(sThinOuterBlurMaskFilter));
    }

    AutoPtr<ArrayOf<Int32> > brightOutlineOffset = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IBitmap> brightOutline;
    glowShape->ExtractAlpha(mBlurPaint, brightOutlineOffset, (IBitmap**)&brightOutline);

    // calculate the inner blur
    srcDstCanvas->SetBitmap(glowShape);
    srcDstCanvas->DrawColor(0xFF000000, PorterDuffMode_SRC_OUT);
    AutoPtr<IBlurMaskFilter> innerBlurMaskFilter;
    switch (thickness) {
        case EXTRA_THICK:
            innerBlurMaskFilter = sExtraThickInnerBlurMaskFilter;
            break;
        case THICK:
            innerBlurMaskFilter = sThickInnerBlurMaskFilter;
            break;
        case MEDIUM:
            innerBlurMaskFilter = sMediumInnerBlurMaskFilter;
            break;
        default:
            //throw new RuntimeException("Invalid blur thickness");
            Slogger::E("HolographicOutlineHelper", "Invalid blur thickness");
            return E_RUNTIME_EXCEPTION;
    }
    mBlurPaint->SetMaskFilter(IMaskFilter::Probe(innerBlurMaskFilter));
    AutoPtr<ArrayOf<Int32> > thickInnerBlurOffset = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IBitmap> thickInnerBlur;
    glowShape->ExtractAlpha(mBlurPaint, thickInnerBlurOffset, (IBitmap**)&thickInnerBlur);

    // mask out the inner blur
    srcDstCanvas->SetBitmap(thickInnerBlur);
    srcDstCanvas->DrawBitmap(glowShape, -(*thickInnerBlurOffset)[0],
            -(*thickInnerBlurOffset)[1], mErasePaint);
    Int32 height;
    thickInnerBlur->GetHeight(&height);
    srcDstCanvas->DrawRect(0, 0, -(*thickInnerBlurOffset)[0], height,
            mErasePaint);
    Int32 width;
    thickInnerBlur->GetWidth(&width);
    srcDstCanvas->DrawRect(0, 0, width, -(*thickInnerBlurOffset)[1],
            mErasePaint);

    // draw the inner and outer blur
    srcDstCanvas->SetBitmap(srcDst);
    srcDstCanvas->DrawColor(0, PorterDuffMode_CLEAR);
    mHolographicPaint->SetColor(color);
    srcDstCanvas->DrawBitmap(thickInnerBlur, (*thickInnerBlurOffset)[0], (*thickInnerBlurOffset)[1],
            mHolographicPaint);
    srcDstCanvas->DrawBitmap(thickOuterBlur, (*outerBlurOffset)[0], (*outerBlurOffset)[1],
            mHolographicPaint);

    // draw the bright outline
    mHolographicPaint->SetColor(outlineColor);
    srcDstCanvas->DrawBitmap(brightOutline, (*brightOutlineOffset)[0], (*brightOutlineOffset)[1],
            mHolographicPaint);

    // cleanup
    srcDstCanvas->SetBitmap(NULL);
    brightOutline->Recycle();
    thickOuterBlur->Recycle();
    thickInnerBlur->Recycle();
    return glowShape->Recycle();
}

ECode HolographicOutlineHelper::ApplyExtraThickExpensiveOutlineWithBlur(
    /* [in] */ IBitmap* srcDst,
    /* [in] */ ICanvas* srcDstCanvas,
    /* [in] */ Int32 color,
    /* [in] */ Int32 outlineColor)
{
    return ApplyExpensiveOutlineWithBlur(srcDst, srcDstCanvas, color, outlineColor, EXTRA_THICK);
}

ECode HolographicOutlineHelper::ApplyThickExpensiveOutlineWithBlur(
    /* [in] */ IBitmap* srcDst,
    /* [in] */ ICanvas* srcDstCanvas,
    /* [in] */ Int32 color,
    /* [in] */ Int32 outlineColor)
{
    return ApplyExpensiveOutlineWithBlur(srcDst, srcDstCanvas, color, outlineColor, THICK);
}

ECode HolographicOutlineHelper::ApplyMediumExpensiveOutlineWithBlur(
    /* [in] */ IBitmap* srcDst,
    /* [in] */ ICanvas* srcDstCanvas,
    /* [in] */ Int32 color,
    /* [in] */ Int32 outlineColor,
    /* [in] */ Boolean clipAlpha)
{
    return ApplyExpensiveOutlineWithBlur(srcDst, srcDstCanvas, color, outlineColor, clipAlpha,
            MEDIUM);
}

ECode HolographicOutlineHelper::ApplyMediumExpensiveOutlineWithBlur(
    /* [in] */ IBitmap* srcDst,
    /* [in] */ ICanvas* srcDstCanvas,
    /* [in] */ Int32 color,
    /* [in] */ Int32 outlineColor)
{
    return ApplyExpensiveOutlineWithBlur(srcDst, srcDstCanvas, color, outlineColor, MEDIUM);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos