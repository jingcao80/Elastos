#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSSCROLLVIEWPERFORMANCEHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSSCROLLVIEWPERFORMANCEHELPER_H__

#include "Elastos.Droid.Core_server.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::SystemUI::Recent::IRecentsCallback;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Graphics::ICanvas;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

class RecentsScrollViewPerformanceHelper : public ElLightRefBase
{
public:
    RecentsScrollViewPerformanceHelper(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IView* scrollView,
        /* [in] */ Boolean isVertical);

    static CARAPI_(AutoPtr<RecentsScrollViewPerformanceHelper>) Create(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IView* scrollView,
        /* [in] */ Boolean isVertical);

    CARAPI_(void) OnAttachedToWindowCallback(
        /* [in] */ IRecentsCallback* callback,
        /* [in] */ ILinearLayout* layout,
        /* [in] */ Boolean hardwareAccelerated);

    CARAPI_(void) AddViewCallback(
        /* [in] */ IView* newLinearLayoutChild);

    CARAPI_(void) DrawCallback(
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
        /* [in] */ Float rightFadingEdgeStrength);

    CARAPI_(Int32) GetVerticalFadingEdgeLengthCallback();

    CARAPI_(Int32) GetHorizontalFadingEdgeLengthCallback();

public:
    static const Boolean OPTIMIZE_SW_RENDERED_RECENTS;
    static const Boolean USE_DARK_FADE_IN_HW_ACCELERATED_MODE;

private:
    // mScrollView has this's reference
    IView* mScrollView;

    Int32 mFadingEdgeLength;
    Boolean mIsVertical;
    Boolean mSoftwareRendered;
};

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSSCROLLVIEWPERFORMANCEHELPER_H__
