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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSECURITYVIEWFLIPPER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSECURITYVIEWFLIPPER_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSecurityViewFlipper.h"
#include <Elastos.Droid.Internal.h>
#include <elastos/droid/widget/ViewFlipper.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ViewFlipper;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Subclass of the current view flipper that allows us to overload dispatchTouchEvent() so
 * we can emulate {@link WindowManager.LayoutParams#FLAG_SLIPPERY} within a view hierarchy.
 *
 */
CarClass(CKeyguardSecurityViewFlipper)
    , public ViewFlipper
    , public IKeyguardSecurityViewFlipper
    , public IKeyguardSecurityView
{
public:
    class LayoutParams
        : public FrameLayout::FrameLayoutLayoutParams
        , public IKeyguardSecurityViewFlipperLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        LayoutParams();

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* other);

        CARAPI constructor(
            /* [in] */ IKeyguardSecurityViewFlipperLayoutParams* other);

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI SetMaxWidth(
            /* [in] */ Int32 value);

        CARAPI GetMaxWidth(
            /* [out] */ Int32* value);

        CARAPI SetMaxHeight(
            /* [in] */ Int32 value);

        CARAPI GetMaxHeight(
            /* [out] */ Int32* value);

    public:
        //@ViewDebug.ExportedProperty(category = "layout")
        Int32 mMaxWidth;

        //@ViewDebug.ExportedProperty(category = "layout")
        Int32 mMaxHeight;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardSecurityViewFlipper();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI GetSecurityView(
        /* [out] */ IKeyguardSecurityView** view);

    //@Override
    CARAPI SetKeyguardCallback(
        /* [in] */ IKeyguardSecurityCallback* callback);

    //@Override
    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    //@Override
    CARAPI Reset();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume(
        /* [in] */ Int32 reason);

    //@Override
    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetCallback(
        /* [out] */ IKeyguardSecurityCallback** callback);

    //@Override
    CARAPI ShowUsabilityHint();

    //@Override
    CARAPI ShowBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI HideBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI StartAppearAnimation();

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

protected:
    //@Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

private:
    CARAPI_(Int32) MakeChildMeasureSpec(
        /* [in] */ Int32 maxSize,
        /* [in] */ Int32 childDimen);

private:
    static const Boolean DEBUG;

    AutoPtr<IRect> mTempRect;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSECURITYVIEWFLIPPER_H__
