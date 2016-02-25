
#include "elastos/droid/systemui/recent/RecentsScrollViewPerformanceHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/systemui/recent/RecentsPanelView.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CLinearGradient.h"
#include <elastos/core/Math.h>
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::Math;
using Elastos::Droid::R;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::Recent::RecentsPanelView;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::ILinearGradient;
using Elastos::Droid::Graphics::CLinearGradient;
using Elastos::Droid::Graphics::ShaderTileMode;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

const Boolean RecentsScrollViewPerformanceHelper::OPTIMIZE_SW_RENDERED_RECENTS = TRUE;
const Boolean RecentsScrollViewPerformanceHelper::USE_DARK_FADE_IN_HW_ACCELERATED_MODE = TRUE;

RecentsScrollViewPerformanceHelper::RecentsScrollViewPerformanceHelper(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IView* scrollView,
    /* [in] */ Boolean isVertical)
    : mFadingEdgeLength(0)
    , mIsVertical(FALSE)
    , mSoftwareRendered(FALSE)
{
    mScrollView = scrollView;
    AutoPtr<ArrayOf<Int32> > attrViews = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::View),
            ArraySize(R::styleable::View));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrViews, (ITypedArray**)&a);
    AutoPtr<IViewConfiguration> vc;
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    helper->Get(context, (IViewConfiguration**)&vc);
    Int32 len;
    vc->GetScaledFadingEdgeLength(&len);
    a->GetDimensionPixelSize(R::styleable::View_fadingEdgeLength, len, &mFadingEdgeLength);
    mIsVertical = isVertical;
}

AutoPtr<RecentsScrollViewPerformanceHelper> RecentsScrollViewPerformanceHelper::Create(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IView* scrollView,
    /* [in] */ Boolean isVertical)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean isTablet;
    res->GetBoolean(SystemUIR::bool_::config_recents_interface_for_tablets, &isTablet);
    if (!isTablet && (OPTIMIZE_SW_RENDERED_RECENTS || USE_DARK_FADE_IN_HW_ACCELERATED_MODE)) {
        AutoPtr<RecentsScrollViewPerformanceHelper> helper;
        helper = new RecentsScrollViewPerformanceHelper(context, attrs, scrollView, isVertical);
        return helper;
    }
    else {
        return NULL;
    }
}

void RecentsScrollViewPerformanceHelper::OnAttachedToWindowCallback(
    /* [in] */ IRecentsCallback* callback,
    /* [in] */ ILinearLayout* layout,
    /* [in] */ Boolean hardwareAccelerated)
{
    mSoftwareRendered = !hardwareAccelerated;
    if ((mSoftwareRendered && OPTIMIZE_SW_RENDERED_RECENTS)
            || USE_DARK_FADE_IN_HW_ACCELERATED_MODE) {
        mScrollView->SetVerticalFadingEdgeEnabled(FALSE);
        mScrollView->SetHorizontalFadingEdgeEnabled(FALSE);
    }
}

void RecentsScrollViewPerformanceHelper::AddViewCallback(
    /* [in] */ IView* newLinearLayoutChild)
{
    if (mSoftwareRendered && OPTIMIZE_SW_RENDERED_RECENTS) {
        AutoPtr<IInterface> tag;
        newLinearLayoutChild->GetTag((IInterface**)&tag);
        AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();
        holder->mLabelView->SetDrawingCacheEnabled(TRUE);
        holder->mLabelView->BuildDrawingCache();
    }
}

void RecentsScrollViewPerformanceHelper::DrawCallback(
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
    /* [in] */ Float rightFadingEdgeStrength)
{
    if ((mSoftwareRendered && OPTIMIZE_SW_RENDERED_RECENTS)
            || USE_DARK_FADE_IN_HW_ACCELERATED_MODE) {
        AutoPtr<IPaint> p;
        CPaint::New((IPaint**)&p);
        AutoPtr<IMatrix> matrix;
        CMatrix::New((IMatrix**)&matrix);
        // use use a height of 1, and then wack the matrix each time we
        // actually use it.
        AutoPtr<IShader> fade;
        CLinearGradient::New(0, 0, 0, 1, 0xCC000000, 0, ShaderTileMode_CLAMP, (ILinearGradient**)&fade);
        // PULL OUT THIS CONSTANT

        p->SetShader(fade);

        // draw the fade effect
        Boolean drawTop = FALSE;
        Boolean drawBottom = FALSE;
        Boolean drawLeft = FALSE;
        Boolean drawRight = FALSE;

        Float topFadeStrength = 0.0;
        Float bottomFadeStrength = 0.0;
        Float leftFadeStrength = 0.0;
        Float rightFadeStrength = 0.0;

        Float fadeHeight = mFadingEdgeLength;
        Int32 length = (Int32)fadeHeight;

        Boolean b;
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
            topFadeStrength = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(1.0f, topFadingEdgeStrength));
            drawTop = topFadeStrength * fadeHeight > 1.0f;
            bottomFadeStrength = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(1.0f, bottomFadingEdgeStrength));
            drawBottom = bottomFadeStrength * fadeHeight > 1.0f;
        }

        if (!mIsVertical) {
            leftFadeStrength = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(1.0f, leftFadingEdgeStrength));
            drawLeft = leftFadeStrength * fadeHeight > 1.0f;
            rightFadeStrength = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(1.0f, rightFadingEdgeStrength));
            drawRight = rightFadeStrength * fadeHeight > 1.0f;
        }

        if (drawTop) {
            matrix->SetScale(1, fadeHeight * topFadeStrength);
            matrix->PostTranslate(left, top, &b);
            fade->SetLocalMatrix(matrix);
            canvas->DrawRect(left, top, right, top + length, p);
        }

        if (drawBottom) {
            matrix->SetScale(1, fadeHeight * bottomFadeStrength);
            matrix->PostRotate(180, &b);
            matrix->PostTranslate(left, bottom, &b);
            fade->SetLocalMatrix(matrix);
            canvas->DrawRect(left, bottom - length, right, bottom, p);
        }

        if (drawLeft) {
            matrix->SetScale(1, fadeHeight * leftFadeStrength);
            matrix->PostRotate(-90, &b);
            matrix->PostTranslate(left, top, &b);
            fade->SetLocalMatrix(matrix);
            canvas->DrawRect(left, top, left + length, bottom, p);
        }

        if (drawRight) {
            matrix->SetScale(1, fadeHeight * rightFadeStrength);
            matrix->PostRotate(90, &b);
            matrix->PostTranslate(right, top, &b);
            fade->SetLocalMatrix(matrix);
            canvas->DrawRect(right - length, top, right, bottom, p);
        }
    }
}

Int32 RecentsScrollViewPerformanceHelper::GetVerticalFadingEdgeLengthCallback()
{
    return mFadingEdgeLength;
}

Int32 RecentsScrollViewPerformanceHelper::GetHorizontalFadingEdgeLengthCallback()
{
    return mFadingEdgeLength;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
