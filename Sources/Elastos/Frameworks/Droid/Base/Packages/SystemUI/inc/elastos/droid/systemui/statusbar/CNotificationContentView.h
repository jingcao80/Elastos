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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONCONTENTVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONCONTENTVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CNotificationContentView.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * A frame layout containing the actual payload of the notification, including the contracted and
 * expanded layout. This class is responsible for clipping the content and and switching between the
 * expanded and contracted view depending on its clipped size.
 */
CarClass(CNotificationContentView)
    , public FrameLayout
    , public INotificationContentView
{
private:
    class ViewRunnable : public Runnable
    {
    public:
        ViewRunnable(
            /* [in] */ CNotificationContentView* host,
            /* [in] */ Boolean showContractedChild);

        // @Override
        CARAPI Run();

    private:
        CNotificationContentView* mHost;
        Boolean mShowContractedChild;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNotificationContentView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Reset();

    CARAPI GetContractedChild(
        /* [out] */ IView** view);

    CARAPI GetExpandedChild(
        /* [out] */ IView** view);

    CARAPI SetContractedChild(
        /* [in] */ IView* child);

    CARAPI SetExpandedChild(
        /* [in] */ IView* child);

    CARAPI SetActualHeight(
        /* [in] */ Int32 actualHeight);

    CARAPI GetMaxHeight(
        /* [out] */ Int32* height);

    CARAPI GetMinHeight(
        /* [out] */ Int32* height);

    CARAPI SetClipTopAmount(
        /* [in] */ Int32 clipTopAmount);

    CARAPI NotifyContentUpdated();

    CARAPI IsContentExpandable(
        /* [out] */ Boolean* result);

    CARAPI SetDark(
        /* [in] */ Boolean dark,
        /* [in] */ Boolean fade);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* has);

protected:
    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(void) UpdateClipping();

    CARAPI_(void) SanitizeContractedLayoutParams(
        /* [in] */ IView* contractedChild);

    CARAPI_(void) SelectLayout(
        /* [in] */ Boolean animate,
        /* [in] */ Boolean force);

    CARAPI_(void) RunSwitchAnimation(
        /* [in] */ Boolean showContractedChild);

    CARAPI_(Boolean) ShowContractedChild();

    CARAPI_(void) SetImageViewDark(
        /* [in] */ Boolean dark,
        /* [in] */ Boolean fade,
        /* [in] */ Int32 imageViewId);

    static CARAPI_(AutoPtr<IPaint>) CreateInvertPaint();

private:
    static const Int64 ANIMATION_DURATION_LENGTH;
    static AutoPtr<IPaint> INVERT_PAINT;
    static AutoPtr<IColorFilter> NO_COLOR_FILTER;

    AutoPtr<IRect> mClipBounds;

    AutoPtr<IView> mContractedChild;
    AutoPtr<IView> mExpandedChild;

    Int32 mSmallHeight;
    Int32 mClipTopAmount;
    Int32 mActualHeight;

    AutoPtr<IInterpolator> mLinearInterpolator;

    Boolean mContractedVisible;
    Boolean mDark;

    AutoPtr<IPaint> mFadePaint;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONCONTENTVIEW_H__
