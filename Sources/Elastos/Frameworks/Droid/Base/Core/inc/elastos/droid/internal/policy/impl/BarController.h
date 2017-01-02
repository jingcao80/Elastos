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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_BARCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_BARCONTROLLER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IWindowState;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
  * Controls state/behavior specific to a system bar window.
  */
class BarController
    : public Object
    , public IBarController
{
public:
    class InnerRunnable1
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable1(
            /* [in] */ BarController* owner,
            /* [in] */ Int32 state);

        // @Override
        CARAPI Run();

    private:
        BarController* mOwner;
        Int32 mState;
    };

public:
    CAR_INTERFACE_DECL()

    BarController();

    CARAPI constructor(
        /* [in] */ const String& tag,
        /* [in] */ Int32 transientFlag,
        /* [in] */ Int32 unhideFlag,
        /* [in] */ Int32 translucentFlag,
        /* [in] */ Int32 statusBarManagerId,
        /* [in] */ Int32 translucentWmFlag);

    virtual CARAPI SetWindow(
        /* [in] */ IWindowState* win);

    virtual CARAPI ShowTransient();

    virtual CARAPI IsTransientShowing(
        /* [out] */ Boolean* result);

    virtual CARAPI IsTransientShowRequested(
        /* [out] */ Boolean* result);

    virtual CARAPI WasRecentlyTranslucent(
        /* [out] */ Boolean* result);

    virtual CARAPI AdjustSystemUiVisibilityLw(
        /* [in] */ Int32 oldVis,
        /* [in] */ Int32 vis);

    virtual CARAPI ApplyTranslucentFlagLw(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 oldVis,
        /* [out] */ Int32* result);

    virtual CARAPI SetBarShowingLw(
        /* [in] */ Boolean show,
        /* [out] */ Boolean* result);

    virtual CARAPI CheckHiddenLw(
        /* [out] */ Boolean* result);

    virtual CARAPI CheckShowTransientBarLw(
        /* [out] */ Boolean* result);

    virtual CARAPI UpdateVisibilityLw(
        /* [in] */ Boolean transientAllowed,
        /* [in] */ Int32 oldVis,
        /* [in] */ Int32 vis,
        /* [out] */ Int32* result);

    virtual CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

private:
    CARAPI_(Int32) ComputeStateLw(
        /* [in] */ Boolean wasVis,
        /* [in] */ Boolean wasAnim,
        /* [in] */ IWindowState* win,
        /* [in] */ Boolean change);

    CARAPI_(Boolean) UpdateStateLw(
        /* [in] */ Int32 state);

    CARAPI_(void) SetTransientBarState(
        /* [in] */ Int32 state);

    CARAPI_(AutoPtr<IIStatusBarService>) GetStatusBarService();

    static CARAPI_(String) TransientBarStateToString(
        /* [in] */ Int32 state);

private:
    static const Boolean DEBUG;
    static const Int32 TRANSIENT_BAR_NONE = 0;
    static const Int32 TRANSIENT_BAR_SHOW_REQUESTED = 1;
    static const Int32 TRANSIENT_BAR_SHOWING = 2;
    static const Int32 TRANSIENT_BAR_HIDING = 3;
    static const Int32 TRANSLUCENT_ANIMATION_DELAY_MS = 1000;
    /*const*/ String mTag;
    /*const*/ Int32 mTransientFlag;
    /*const*/ Int32 mUnhideFlag;
    /*const*/ Int32 mTranslucentFlag;
    /*const*/ Int32 mStatusBarManagerId;
    /*const*/ Int32 mTranslucentWmFlag;
    /*const*/ AutoPtr<IHandler> mHandler;
    /*const*/ Object mServiceAquireLock;
    AutoPtr<IIStatusBarService> mStatusBarService;
    AutoPtr<IWindowState> mWin;
    Int32 mState;
    Int32 mTransientBarState;
    Boolean mPendingShow;
    Int64 mLastTranslucent;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_BARCONTROLLER_H__

