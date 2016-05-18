#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CIKEYGUARDSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CIKEYGUARDSERVICE_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CIKeyguardService.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Internal::Policy::IIKeyguardExitCallback;
using Elastos::Droid::Internal::Policy::IIKeyguardService;
using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardService;

CarClass(CIKeyguardService)
    , public Object
    , public IIKeyguardService
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CIKeyguardService();

    CARAPI constructor(
        /* [in] */ IKeyguardService* host);

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

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Boolean mIsOccluded;

    KeyguardService* mHost;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CIKEYGUARDSERVICE_H__