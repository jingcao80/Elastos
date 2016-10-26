
#include "Elastos.Droid.Content.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "elastos/droid/settings/CConfirmLockPattern.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String ChooseLockSettingsHelper::EXTRA_KEY_TYPE("type");
const String ChooseLockSettingsHelper::EXTRA_KEY_PASSWORD("password");

ChooseLockSettingsHelper::ChooseLockSettingsHelper()
{}

ECode ChooseLockSettingsHelper::constructor(
    /* [in] */ IActivity* activity)
{
    mActivity = activity;
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&mLockPatternUtils);
    return NOERROR;
}

ECode ChooseLockSettingsHelper::constructor(
    /* [in] */ IActivity* activity,
    /* [in] */ IFragment* fragment)
{
    constructor(activity);
    mFragment = fragment;
    return NOERROR;
}

AutoPtr<ILockPatternUtils> ChooseLockSettingsHelper::Utils()
{
    return mLockPatternUtils;
}

Boolean ChooseLockSettingsHelper::LaunchConfirmationActivity(
    /* [in] */ Int32 request,
    /* [in] */ ICharSequence* message,
    /* [in] */ ICharSequence* details)
{
    return LaunchConfirmationActivity(request, message, details, FALSE);
}

Boolean ChooseLockSettingsHelper::LaunchConfirmationActivity(
    /* [in] */ Int32 request,
    /* [in] */ ICharSequence* message,
    /* [in] */ ICharSequence* details,
    /* [in] */ Boolean returnCredentials)
{
    Boolean launched = FALSE;
    Int32 data;
    mLockPatternUtils->GetKeyguardStoredPasswordQuality(&data);
    switch (data) {
        case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
            launched = ConfirmPattern(request, message, details, returnCredentials);
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX:
            // TODO: update UI layout for ConfirmPassword to show message and details
            launched = ConfirmPassword(request, message, returnCredentials);
            break;
    }
    return launched;
}

Boolean ChooseLockSettingsHelper::ConfirmPattern(
    /* [in] */ Int32 request,
    /* [in] */ ICharSequence* message,
    /* [in] */ ICharSequence* details,
    /* [in] */ Boolean returnCredentials)
{
    Boolean res1, res2;
    mLockPatternUtils->IsLockPatternEnabled(&res1);
    if (!res1 || (mLockPatternUtils->SavedPatternExists(&res2), !res2)) {
        return FALSE;
    }
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    // supply header and footer text in the intent
    intent->PutExtra(CConfirmLockPattern::HEADER_TEXT, message);
    intent->PutExtra(CConfirmLockPattern::FOOTER_TEXT, details);
    intent->SetClassName(String("com.android.settings"),
                        returnCredentials
                        ? String("Elastos.Droid.Settings.CConfirmLockPatternInternalActivity")
                        : String("Elastos.Droid.Settings.CConfirmLockPattern"));
    if (mFragment != NULL) {
        mFragment->StartActivityForResult(intent, request);
    }
    else {
        mActivity->StartActivityForResult(intent, request);
    }
    return TRUE;
}

Boolean ChooseLockSettingsHelper::ConfirmPassword(
    /* [in] */ Int32 request,
    /* [in] */ ICharSequence* message,
    /* [in] */ Boolean returnCredentials)
{
    Boolean res;
    mLockPatternUtils->IsLockPasswordEnabled(&res);
    if (!res) return FALSE;

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    // supply header text in the intent
    intent->PutExtra(CConfirmLockPattern::HEADER_TEXT, message);
    intent->SetClassName(String("com.android.settings"),
                        returnCredentials
                        ? String("Elastos.Droid.Settings.CConfirmLockPasswordInternalActivity")
                        : String("Elastos.Droid.Settings.CConfirmLockPassword"));
    if (mFragment != NULL) {
        mFragment->StartActivityForResult(intent, request);
    }
    else {
        mActivity->StartActivityForResult(intent, request);
    }
    return TRUE;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos