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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSERVICEDELEGATE_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSERVICEDELEGATE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Internal::Policy::IIKeyguardService;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IOnKeyguardExitResult;
using Elastos::Droid::View::IView;

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

/**
* A local class that keeps a cache of keyguard state that can be restored in the event
* keyguard crashes. It currently also allows runtime-selectable
* local or remote instances of keyguard.
*/
class KeyguardServiceDelegate
    : public Object
    , public IKeyguardServiceDelegate
{
private:
    class InnerKeyguardState
        : public Object
    {
    public:
        InnerKeyguardState();

    public:
        Boolean enabled;
        Boolean dismissable;
        Int32 offReason;
        Int32 currentUser;
        Boolean screenIsOn;
        Boolean bootCompleted;
        Boolean showing;
        Boolean showingAndNotOccluded;
        Boolean inputRestricted;
        Boolean occluded;
        Boolean secure;
        Boolean dreaming;
        Boolean systemIsReady;
    };

    class InnerServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InnerServiceConnection(
            /* [in] */ KeyguardServiceDelegate* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    private:
        KeyguardServiceDelegate* mHost;
    };

    class ShowScrimRunnable
        : public Runnable
    {
    public:
        ShowScrimRunnable(
             /* [in] */ KeyguardServiceDelegate* host);

        CARAPI Run();

    private:
        KeyguardServiceDelegate* mHost;
    };

    class HideScrimRunnable
        : public Runnable
    {
    public:
        HideScrimRunnable(
             /* [in] */ KeyguardServiceDelegate* host);

        CARAPI Run();

    private:
        KeyguardServiceDelegate* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    KeyguardServiceDelegate();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ ILockPatternUtils* lockPatternUtils);

    CARAPI BindService(
        /* [in] */ IContext* context);

    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    CARAPI IsShowingAndNotOccluded(
        /* [out] */ Boolean* result);

    CARAPI IsInputRestricted(
        /* [out] */ Boolean* result);

    CARAPI VerifyUnlock(
        /* [in] */ IOnKeyguardExitResult* onKeyguardExitResult);

    CARAPI KeyguardDone(
        /* [in] */ Boolean authenticated,
        /* [in] */ Boolean wakeup);

    CARAPI SetOccluded(
        /* [in] */ Boolean isOccluded,
        /* [out] */ Int32* result);

    CARAPI Dismiss();

    CARAPI IsSecure(
        /* [out] */ Boolean* result);

    CARAPI OnDreamingStarted();

    CARAPI OnDreamingStopped();

    CARAPI OnScreenTurnedOn(
        /* [in] */ IKeyguardServiceDelegateShowListener* showListener);

    CARAPI OnScreenTurnedOff(
        /* [in] */ Int32 why);

    CARAPI SetKeyguardEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsDismissable(
        /* [out] */ Boolean* result);

    CARAPI OnSystemReady();

    CARAPI DoKeyguardTimeout(
        /* [in] */ IBundle* options);

    CARAPI ShowAssistant();

    CARAPI SetCurrentUser(
        /* [in] */ Int32 newUserId);

    CARAPI StartKeyguardExitAnimation(
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 fadeoutDuration);

    CARAPI ShowScrim();

    CARAPI HideScrim();

    CARAPI OnBootCompleted();

    CARAPI OnActivityDrawn();

private:
    static CARAPI_(AutoPtr<IView>) CreateScrim(
        /* [in] */ IContext* context);

    CARAPI SendStateChangeBroadcast(
        /* [in] */ Boolean bound);

public:
    // TODO: propagate changes to these to {@link KeyguardTouchDelegate}
    static const String KEYGUARD_PACKAGE;// = "com.android.systemui";
    static const String KEYGUARD_CLASS;// = "com.android.systemui.keyguard.KeyguardService";

protected:
    AutoPtr<IIKeyguardService> mKeyguardService;

private:
    static const Boolean DEBUG;

    static const String ACTION_STATE_CHANGE;
    static const String EXTRA_ACTIVE;

    AutoPtr<IView> mScrim; // shown if keyguard crashes

    AutoPtr<InnerKeyguardState> mKeyguardState;

    AutoPtr<IServiceConnection> mKeyguardConnection;

};

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_CKEYGUARDSERVICEDELEGATE_H__
