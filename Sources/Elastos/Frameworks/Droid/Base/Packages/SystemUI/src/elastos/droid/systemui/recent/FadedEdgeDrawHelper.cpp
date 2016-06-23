#include "elastos/droid/systemui/recent/FadedEdgeDrawHelper.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CLinearGradient;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::R;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

const Boolean FadedEdgeDrawHelper::OPTIMIZE_SW_RENDERED_RECENTS = TRUE;
const Boolean FadedEdgeDrawHelper::USE_DARK_FADE_IN_HW_ACCELERATED_MODE = TRUE;

AutoPtr<FadedEdgeDrawHelper> FadedEdgeDrawHelper::Create(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IView* scrollView,
    /* [in] */ Boolean isVertical)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean isTablet;
    res->GetBoolean(R::bool_::config_recents_interface_for_tablets, &isTablet);
    if (!isTablet && (OPTIMIZE_SW_RENDERED_RECENTS || USE_DARK_FADE_IN_HW_ACCELERATED_MODE)) {
        return new FadedEdgeDrawHelper(context, attrs, scrollView, isVertical);
    }
    else {
        return NULL;
    }
}

FadedEdgeDrawHelper::FadedEdgeDrawHelper(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IView* scrollView,
    /* [in] */ Boolean isVertical)
    : mScrollView(scrollView)
    , mIsVertical(isVertical)
    , mSoftwareRendered(FALSE)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(Elastos::Droid::R::styleable::View);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    AutoPtr<IViewConfigurationHelper> vch;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vch);
    AutoPtr<IViewConfiguration> vc;
    vch->Get(context, (IViewConfiguration**)&vc);
    Int32 sfel;
    vc->GetScaledFadingEdgeLength(&sfel);
    a->GetDimensionPixelSize(Elastos::Droid::R::styleable::View_fadingEdgeLength, sfel, &mFadingEdgeLength);
}

ECode FadedEdgeDrawHelper::OnAttachedToWindowCallback(
    /* [in] */ ILinearLayout* layout,
    /* [in] */ Boolean hardwareAccelerated)
{
    mSoftwareRendered = !hardwareAccelerated;
    if ((mSoftwareRendered && OPTIMIZE_SW_RENDERED_RECENTS)
            || USE_DARK_FADE_IN_HW_ACCELERATED_MODE) {
        mScrollView->SetVerticalFadingEdgeEnabled(FALSE);
        mScrollView->SetHorizontalFadingEdgeEnabled(FALSE);
    }
    return NOERROR;
}

ECode FadedEdgeDrawHelper::AddViewCallback(
    /* [in] */ IView* newLinearLayoutChild)
{
    if (mSoftwareRendered && OPTIMIZE_SW_RENDERED_RECENTS) {
        //TODO
        // final RecentsPanelView.ViewHolder holder =
        //         (RecentsPanelView.ViewHolder) newLinearLayoutChild.getTag();
        // holder.labelView.setDrawingCacheEnabled(true);
        // holder.labelView.buildDrawingCache();
    }
    return NOERROR;
}

ECode FadedEdgeDrawHelper::DrawCallback(
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
    /* [in] */ Int32 mPaddingTop)
{
    if ((mSoftwareRendered && OPTIMIZE_SW_RENDERED_RECENTS)
        || USE_DARK_FADE_IN_HW_ACCELERATED_MODE) {
        if (mFadePaint == NULL) {
            CPaint::New((IPaint**)&mFadePaint);
            CMatrix::New((IMatrix**)&mFadeMatrix);
            // use use a height of 1, and then wack the matrix each time we
            // actually use it.
            CLinearGradient::New(0.0f, 0.0f, 0.0f, 1.0f, 0xCC000000, 0,
                ShaderTileMode_CLAMP, (ILinearGradient**)&mFade);
            // PULL OUT THIS CONSTANT
            mFadePaint->SetShader(IShader::Probe(mFade));
        }

        // draw the fade effect
        Boolean drawTop = FALSE;
        Boolean drawBottom = FALSE;
        Boolean drawLeft = FALSE;
        Boolean drawRight = FALSE;

        Float topFadeStrength = 0.0f;
        Float bottomFadeStrength = 0.0f;
        Float leftFadeStrength = 0.0f;
        Float rightFadeStrength = 0.0f;

        const Float fadeHeight = mFadingEdgeLength;
        Int32 length = (Int32) fadeHeight;

        // clip the fade length if top and bottom fades overlap
        // overlapping fades produce odd-looking artifacts
        if (mIsVertical && (top + length > bottom - length)) {
            length = (bottom - top) / 2;
        }

        // also clip horizontal fades if necessary
        if (!mIsVertical && (left + length > right - length)) {
            length = (right - left) / 2;
        }

        if (mIsVertical) {
            topFadeStrength = Elastos::Core::Math::Max(0.0f,
                Elastos::Core::Math::Min(1.0f, topFadingEdgeStrength));
            drawTop = topFadeStrength * fadeHeight > 1.0f;
            bottomFadeStrength = Elastos::Core::Math::Max(0.0f,
                Elastos::Core::Math::Min(1.0f, bottomFadingEdgeStrength));
            drawBottom = bottomFadeStrength * fadeHeight > 1.0f;
        }

        if (!mIsVertical) {
            leftFadeStrength = Elastos::Core::Math::Max(0.0f,
                Elastos::Core::Math::Min(1.0f, leftFadingEdgeStrength));
            drawLeft = leftFadeStrength * fadeHeight > 1.0f;
            rightFadeStrength = Elastos::Core::Math::Max(0.0f,
                Elastos::Core::Math::Min(1.0f, rightFadingEdgeStrength));
            drawRight = rightFadeStrength * fadeHeight > 1.0f;
        }

        if (drawTop) {
            mFadeMatrix->SetScale(1, fadeHeight * topFadeStrength);
            Boolean b;
            mFadeMatrix->PostTranslate(left, top, &b);
            IShader::Probe(mFade)->SetLocalMatrix(mFadeMatrix);
            mFadePaint->SetShader(IShader::Probe(mFade));
            canvas->DrawRect(left, top, right, top + length, mFadePaint);

            if (mBlackPaint == NULL) {
                // Draw under the status bar at the top
                CPaint::New((IPaint**)&mBlackPaint);
                mBlackPaint->SetColor(0xFF000000);
            }
            canvas->DrawRect(left, top - mPaddingTop, right, top, mBlackPaint);
        }

        if (drawBottom) {
            mFadeMatrix->SetScale(1, fadeHeight * bottomFadeStrength);
            Boolean isPost;
            mFadeMatrix->PostRotate(180.0f, &isPost);
            mFadeMatrix->PostTranslate(left, bottom, &isPost);
            IShader::Probe(mFade)->SetLocalMatrix(mFadeMatrix);
            mFadePaint->SetShader(IShader::Probe(mFade));
            canvas->DrawRect(left, bottom - length, right, bottom, mFadePaint);
        }

        if (drawLeft) {
            mFadeMatrix->SetScale(1, fadeHeight * leftFadeStrength);
            Boolean isPost;
            mFadeMatrix->PostRotate(-90.0f, &isPost);
            mFadeMatrix->PostTranslate(left, top, &isPost);
            IShader::Probe(mFade)->SetLocalMatrix(mFadeMatrix);
            mFadePaint->SetShader(IShader::Probe(mFade));
            canvas->DrawRect(left, top, left + length, bottom, mFadePaint);
        }

        if (drawRight) {
            mFadeMatrix->SetScale(1, fadeHeight * rightFadeStrength);
            Boolean isPost;
            mFadeMatrix->PostRotate(90.0f, &isPost);
            mFadeMatrix->PostTranslate(right, top, &isPost);
            IShader::Probe(mFade)->SetLocalMatrix(mFadeMatrix);
            mFadePaint->SetShader(IShader::Probe(mFade));
            canvas->DrawRect(right - length, top, right, bottom, mFadePaint);
        }
    }
    return NOERROR;
}

ECode FadedEdgeDrawHelper::GetVerticalFadingEdgeLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mFadingEdgeLength;
    return NOERROR;
}

ECode FadedEdgeDrawHelper::GetHorizontalFadingEdgeLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mFadingEdgeLength;
    return NOERROR;
}

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos