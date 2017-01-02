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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CBALLOONHINT_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CBALLOONHINT_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "_Elastos_Droid_InputMethod_Pinyin_CBalloonHint.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonView.h"
#include <elastos/droid/os/HandlerRunnable.h>
#include <elastos/droid/widget/PopupWindow.h>

using Elastos::Droid::Os::HandlerRunnable;
// using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::PopupWindow;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Subclass of PopupWindow used as the feedback when user presses on a soft key
 * or a candidate.
 */
CarClass(CBalloonHint)
    , public PopupWindow
    , public IBalloonHint
{
private:
    class BalloonTimer
        : public HandlerRunnable
    {
    public:
        BalloonTimer(
            /* [in] */ CBalloonHint* host);

        CARAPI_(void) StartTimer(
            /* [in] */ Int64 tm,
            /* [in] */ Int32 action,
            /* [in] */ ArrayOf<Int32>* positionInParent,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI_(Boolean) IsPending();

        CARAPI_(Boolean) RemoveTimer();

        CARAPI_(Int32) GetAction();

        CARAPI Run();

    public:
        static const Int32 ACTION_SHOW = 1;
        static const Int32 ACTION_HIDE = 2;
        static const Int32 ACTION_UPDATE = 3;

    private:
        /**
         * The pending action.
         */
        Int32 mAction;

        AutoPtr< ArrayOf<Int32> > mPositionInParent;
        Int32 mWidth;
        Int32 mHeight;

        Boolean mTimerPending;
        CBalloonHint* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CBalloonHint();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IView* parent,
        /* [in] */ Int32 measureSpecMode);

    CARAPI_(AutoPtr<IContext>) GetContext();

    CARAPI_(AutoPtr<IRect>) GetPadding();

    CARAPI_(void) SetBalloonBackground(
        /* [in] */ IDrawable* drawable);

    /**
     * Set configurations to show text label in this balloon.
     *
     * @param label The text label to show in the balloon.
     * @param textSize The text size used to show label.
     * @param textBold Used to indicate whether the label should be bold.
     * @param textColor The text color used to show label.
     * @param width The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     * @param height The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     */
    CARAPI_(void) SetBalloonConfig(
        /* [in] */ const String& label,
        /* [in] */ Float textSize,
        /* [in] */ Boolean textBold,
        /* [in] */ Int32 textColor,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Set configurations to show text label in this balloon.
     *
     * @param icon The icon used to shown in this balloon.
     * @param width The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     * @param height The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     */
    CARAPI_(void) SetBalloonConfig(
        /* [in] */ IDrawable* icon,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(Boolean) NeedForceDismiss();

    CARAPI_(Int32) GetPaddingLeft();

    CARAPI_(Int32) GetPaddingTop();

    CARAPI_(Int32) GetPaddingRight();

    CARAPI_(Int32) GetPaddingBottom();

    CARAPI_(void) DelayedShow(
        /* [in] */ Int64 delay,
        /* [in] */ ArrayOf<Int32>* locationInParent);

    CARAPI_(void) DelayedUpdate(
        /* [in] */ Int64 delay,
        /* [in] */ ArrayOf<Int32>* locationInParent,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) DelayedDismiss(
        /* [in] */ Int64 delay);

    CARAPI_(void) RemoveTimer();

private:
    CARAPI_(void) SetBalloonSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

public:
    /**
     * Delayed time to show the balloon hint.
     */
    static const Int32 TIME_DELAY_SHOW;

    /**
     * Delayed time to dismiss the balloon hint.
     */
    static const Int32 TIME_DELAY_DISMISS;

private:
    /**
     * The padding information of the balloon. Because PopupWindow's background
     * can not be changed unless it is dismissed and shown again, we set the
     * real background drawable to the content view, and make the PopupWindow's
     * background transparent. So actually this padding information is for the
     * content view.
     */
    AutoPtr<IRect> mPaddingRect;

    /**
     * The context used to create this balloon hint object.
     */
    AutoPtr<IContext> mContext;

    /**
     * Parent used to show the balloon window.
     */
    AutoPtr<IView> mParent;

    /**
     * The content view of the balloon.
     */
    AutoPtr<CBalloonView> mBalloonView;

    /**
     * The measuring specification used to determine its size. Key-press
     * balloons and candidates balloons have different measuring specifications.
     */
    Int32 mMeasureSpecMode;

    /**
     * Used to indicate whether the balloon needs to be dismissed forcibly.
     */
    Boolean mForceDismiss;

    /**
     * Timer used to show/dismiss the balloon window with some time delay.
     */
    AutoPtr<BalloonTimer> mBalloonTimer;

    AutoPtr< ArrayOf<Int32> > mParentLocationInWindow;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CBALLOONHINT_H__
