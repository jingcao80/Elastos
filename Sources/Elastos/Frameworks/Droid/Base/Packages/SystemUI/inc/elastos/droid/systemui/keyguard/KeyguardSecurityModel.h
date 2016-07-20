
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYMODE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYMODE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardSecurityModel
    : public Object
    , public IKeyguardSecurityModel
{
public:
    TO_STRING_IMPL("CKeyguardAccountView::MyRunnable")

    CAR_INTERFACE_DECL()

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
        /* [in] */ SecurityMode mode,
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