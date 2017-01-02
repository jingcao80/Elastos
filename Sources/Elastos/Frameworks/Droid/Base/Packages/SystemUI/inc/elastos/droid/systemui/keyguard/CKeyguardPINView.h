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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPINVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPINVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardPINView.h"
#include "elastos/droid/systemui/keyguard/KeyguardPinBasedInputView.h"
#include "elastos/droid/systemui/keyguard/AppearAnimationUtils.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::Runnable;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Displays a PIN pad for unlocking.
 */
CarClass(CKeyguardPINView)
    , public KeyguardPinBasedInputView
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardPINView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardPINView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CKeyguardPINView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CKeyguardPINView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI ShowUsabilityHint();

    //@Override
    CARAPI GetWrongPasswordStringId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI StartAppearAnimation();

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    CARAPI ResetState();

    //@Override
    CARAPI GetPasswordTextViewId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) EnableClipping(
        /* [in] */ Boolean enable);

private:
    AutoPtr<AppearAnimationUtils> mAppearAnimationUtils;
    AutoPtr<IViewGroup> mKeyguardBouncerFrame;
    AutoPtr<IViewGroup> mRow0;
    AutoPtr<IViewGroup> mRow1;
    AutoPtr<IViewGroup> mRow2;
    AutoPtr<IViewGroup> mRow3;
    AutoPtr<IView> mDivider;
    Int32 mDisappearYTranslation;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPINVIEW_H__
