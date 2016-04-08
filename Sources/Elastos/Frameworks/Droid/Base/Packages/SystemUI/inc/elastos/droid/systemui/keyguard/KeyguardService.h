
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__

#include "_SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/systemui/keyguard/CKeyguardViewMediator.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Policy::IIKeyguardExitCallback;
using Elastos::Droid::Internal::Policy::IIKeyguardService;
using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardService
    : public Elastos::Droid::App::Service
    , public IKeyguardService
{
public:
    class KeyguardServiceBinder
        : public Object
        , public IIKeyguardService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        KeyguardServiceBinder();

        KeyguardServiceBinder(
            /* [in] */ KeyguardService* host);

        CARAPI constructor(
            /* [in] */ Handle32 host);

        // @Override
        CARAPI IsShowing(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI IsSecure(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI IsShowingAndNotOccluded(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI IsInputRestricted(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI VerifyUnlock(
            /* [in] */ IIKeyguardExitCallback* callback);

        // @Override
        CARAPI KeyguardDone(
            /* [in] */ Boolean authenticated,
            /* [in] */ Boolean wakeup);

        // @Override
        CARAPI SetOccluded(
            /* [in] */ Boolean isOccluded,
            /* [out] */ Int32* result);

        // @Override
        CARAPI Dismiss();

        // @Override
        CARAPI OnDreamingStarted();

        // @Override
        CARAPI OnDreamingStopped();

        // @Override
        CARAPI OnScreenTurnedOff(
            /* [in] */ Int32 reason);

        // @Override
        CARAPI OnScreenTurnedOn(
            /* [in] */ IIKeyguardShowCallback* callback);

        // @Override
        CARAPI SetKeyguardEnabled(
            /* [in] */ Boolean enabled);

        // @Override
        CARAPI IsDismissable(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnSystemReady();

        // @Override
        CARAPI DoKeyguardTimeout(
            /* [in] */ IBundle* options);

        // @Override
        CARAPI SetCurrentUser(
            /* [in] */ Int32 userId);

        // @Override
        CARAPI ShowAssistant();

        // @Override
        CARAPI Dispatch(
            /* [in] */ IMotionEvent* event);

        // @Override
        CARAPI LaunchCamera();

        // @Override
        CARAPI OnBootCompleted();

        // @Override
        CARAPI StartKeyguardExitAnimation(
            /* [in] */ Int64 startTime,
            /* [in] */ Int64 fadeoutDuration);

        // @Override
        CARAPI OnActivityDrawn();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        Boolean mIsOccluded;

        KeyguardService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    KeyguardService();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

    CARAPI CheckPermission();

public:
    const static String TAG;
    const static String PERMISSION;

private:
    AutoPtr<CKeyguardViewMediator> mKeyguardViewMediator;

    AutoPtr<KeyguardServiceBinder> mBinder;

};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__