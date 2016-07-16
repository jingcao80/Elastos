
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYMODE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYMODE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardSecurityModel
    : public Object
{
public:
    /**
     * The different types of security available for {@link Mode#UnlockScreen}.
     * @see com.android.internal.policy.impl.LockPatternKeyguardView#getUnlockMode()
     */
    enum SecurityMode {
        Invalid, // NULL state
        None, // No security enabled
        Pattern, // Unlock by drawing a pattern.
        Password, // Unlock by entering an alphanumeric password
        PIN, // Strictly numeric password
        Biometric, // Unlock with a biometric key (e.g. finger print or face unlock)
        Account, // Unlock by entering an account's login and password.
        SimPin, // Unlock by entering a sim pin.
        SimPuk // Unlock by entering a sim puk
    };

public:
    TO_STRING_IMPL("CKeyguardAccountView::MyRunnable")

    KeyguardSecurityModel(
        /* [in] */ IContext* context);

    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    /**
     * Returns true if biometric unlock is installed and selected.  If this returns false there is
     * no need to even construct the biometric unlock.
     */
    CARAPI IsBiometricUnlockEnabled(
        /* [out] */ Boolean* result);

    CARAPI GetSecurityMode(
        /* [out] */ SecurityMode* mode);

    /**
     * Some unlock methods can have an alternate, such as biometric unlocks (e.g. face unlock).
     * This function decides if an alternate unlock is available and returns it. Otherwise,
     * returns @param mode.
     *
     * @param mode the mode we want the alternate for
     * @return alternate or the given mode
     */
    CARAPI GetAlternateFor(
        /* [in] */ SecurityMode mode,
        /* [out] */ SecurityMode* outmode);

    /**
     * Some unlock methods can have a backup which gives the user another way to get into
     * the device. This is currently only supported for Biometric and Pattern unlock.
     *
     * @return backup method or current security mode
     */
    CARAPI GetBackupSecurityMode(
        /* [in] */ SecurityMode* mode,
        /* [out] */ SecurityMode* outmode);

private:
    /**
     * Returns true if a condition is currently suppressing the biometric unlock.  If this returns
     * true there is no need to even construct the biometric unlock.
     */
    CARAPI_(Boolean) IsBiometricUnlockSuppressed();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYMODE_H__