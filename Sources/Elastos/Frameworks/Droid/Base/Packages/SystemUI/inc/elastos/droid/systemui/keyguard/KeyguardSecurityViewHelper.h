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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYVIEWHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYVIEWHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Some common functions that are useful for KeyguardSecurityViews.
 */
class KeyguardSecurityViewHelper
    : public Object
{
private:
    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("KeyguardSecurityViewHelper::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ IView* ecaView)
            : mEcaView(ecaView)
        {}

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mEcaView;

        Boolean mCanceled;
    };

public:
    static CARAPI ShowBouncer(
        /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
        /* [in] */ IView* ecaView,
        /* [in] */ IDrawable* bouncerFrame,
        /* [in] */ Int32 duration);

    static CARAPI HideBouncer(
        /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
        /* [in] */ IView* ecaView,
        /* [in] */ IDrawable* bouncerFrame,
        /* [in] */ Int32 duration);
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYVIEWHELPER_H__