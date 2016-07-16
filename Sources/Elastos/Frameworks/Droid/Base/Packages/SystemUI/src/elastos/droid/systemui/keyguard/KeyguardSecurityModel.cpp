
#include "elastos/droid/systemui/keyguard/KeyguardSecurityModel.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

KeyguardSecurityModel::KeyguardSecurityModel(
    /* [in] */ IContext* context)
    : mContext(context)
{
    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
}

ECode KeyguardSecurityModel::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    return NOERROR;
}

ECode KeyguardSecurityModel::IsBiometricUnlockEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res1, res2;

    *result = (mLockPatternUtils->UsingBiometricWeak(&res1), res1)
                && (mLockPatternUtils->IsBiometricWeakInstalled(&res2), res2);
    return NOERROR;
}

Boolean KeyguardSecurityModel::IsBiometricUnlockSuppressed()
{
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Boolean res;
    Boolean backupIsTimedOut = (monitor->GetFailedUnlockAttempts(&res), res) >=
            ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT;

    Boolean res1, res2;
    Int32 state;

    return (monitor->GetMaxBiometricUnlockAttemptsReached(&res1), res1) || backupIsTimedOut
            || (monitor->IsAlternateUnlockEnabled(&res2), !res1)
            || (monitor->GetPhoneState(&state), state) != ITelephonyManager::CALL_STATE_IDLE;
}

ECode KeyguardSecurityModel::GetSecurityMode(
    /* [out] */ SecurityMode* outmode)
{
    VALIDATE_NOT_NULL(outmode);

    AutoPtr<KeyguardUpdateMonitor> updateMonitor = KeyguardUpdateMonitor::GetInstance(mContext);
    IccCardConstantsState simState;
    updateMonitor->GetSimState(&simState);
    SecurityMode mode = None;

    Boolean res;
    if (simState == IccCardConstantsState_PIN_REQUIRED) {
        mode = SimPin;
    }
    else if (simState == IccCardConstantsState_PUK_REQUIRED
            && (mLockPatternUtils->IsPukUnlockScreenEnable(&res), res)) {
        mode = SimPuk;
    }
    else {
        Int32 security;
        mLockPatternUtils->GetKeyguardStoredPasswordQuality(&security);
        switch (security) {
            case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
            {
                Boolean res;
                mode = (mLockPatternUtils->IsLockPasswordEnabled(&res), res) ?
                        PIN : None;
                break;
            }
            case IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX:
            {
                Boolean res;
                mode = (mLockPatternUtils->IsLockPasswordEnabled(&res), res) ?
                        Password : None;
                break;
            }
            case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
            case IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED:
            {
                Boolean res;
                if (mLockPatternUtils->IsLockPatternEnabled(&res), res) {
                    mode = (mLockPatternUtils->IsPermanentlyLocked(&res), res) ?
                        Account : Pattern;
                }
                break;
            }
            default:
                //throw new IllegalStateException("Unknown security quality:" + security);
                return IllegalStateException;
        }
    }
    *outmode = mode;
    return NOERROR;
}

ECode KeyguardSecurityModel::GetAlternateFor(
    /* [in] */ SecurityMode mode,
    /* [out] */ SecurityMode* outmode)
{
    Boolean res;
    if ((IsBiometricUnlockEnabled(&res), res) && !IsBiometricUnlockSuppressed()
            && (mode == Password
                    || mode == PIN
                    || mode == Pattern)) {
        *outmode = Biometric;
        return NOERROR;
    }
    *outmode = mode; // no alternate, return what was given
    return NOERROR;
}

ECode KeyguardSecurityModel::GetBackupSecurityMode(
    /* [in] */ SecurityMode* mode,
    /* [out] */ SecurityMode* outmode)
{
    VALIDATE_NOT_NULL(outmode)

    switch(mode) {
        case Biometric:
            return GetSecurityMode(outmode);
        case Pattern:
            *outmode = Account;
            return NOERROR;
    }
    *outmode = mode; // no backup, return current security mode
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
