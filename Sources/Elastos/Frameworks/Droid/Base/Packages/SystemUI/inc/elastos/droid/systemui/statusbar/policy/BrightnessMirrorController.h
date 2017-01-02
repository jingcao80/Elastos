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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BRIGHTNESSMIRRORCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BRIGHTNESSMIRRORCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Core::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::SystemUI::StatusBar::IScrimView;
using Elastos::Droid::SystemUI::StatusBar::Phone::IStatusBarWindowView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Controls showing and hiding of the brightness mirror.
 */
class BrightnessMirrorController
    : public Object
    , public IBrightnessMirrorController
{
private:
    class Runnable1 : public Runnable
    {
    public:
        Runnable1(
            /* [in] */ BrightnessMirrorController* host);

        // @Override
        CARAPI Run();

    private:
        BrightnessMirrorController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("BrightnessMirrorController")

    BrightnessMirrorController();

    CARAPI constructor(
        /* [in] */ IStatusBarWindowView* statusBarWindow);

    CARAPI ShowMirror();

    CARAPI HideMirror();

    CARAPI SetLocation(
        /* [in] */ IView* original);

    CARAPI GetMirror(
        /* [out] */ IView** view);

    CARAPI UpdateResources();

private:
    CARAPI_(AutoPtr<IViewPropertyAnimator>) OutAnimation(
        /* [in] */ IViewPropertyAnimator* a);

    CARAPI_(AutoPtr<IViewPropertyAnimator>) InAnimation(
        /* [in] */ IViewPropertyAnimator* a);

public:
    Int64 TRANSITION_DURATION_OUT;
    Int64 TRANSITION_DURATION_IN;

private:
    AutoPtr<IScrimView> mScrimBehind;
    AutoPtr<IView> mBrightnessMirror;
    AutoPtr<IView> mPanelHolder;
    AutoPtr<ArrayOf<Int32> > mInt2Cache;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BRIGHTNESSMIRRORCONTROLLER_H__
