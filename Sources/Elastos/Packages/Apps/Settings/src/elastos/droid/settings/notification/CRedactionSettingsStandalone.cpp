
#include "elastos/droid/settings/notification/CRedactionSettingsStandalone.h"
#include "../R.h"

using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CAR_OBJECT_IMPL(CRedactionSettingsStandalone)

CRedactionSettingsStandalone::CRedactionSettingsStandalone()
{}

CRedactionSettingsStandalone::~CRedactionSettingsStandalone()
{}

ECode CRedactionSettingsStandalone::constructor()
{
    return SettingsActivity::constructor();
}

ECode CRedactionSettingsStandalone::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;

    AutoPtr<IIntent> tmpIntent;
    SettingsActivity::GetIntent((IIntent**)&tmpIntent);
    AutoPtr<IIntent> modIntent;
    CIntent::New(tmpIntent, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.Notification.CRedactionInterstitialFragment"));
    modIntent->PutBooleanExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, TRUE);
    modIntent->PutExtra(EXTRA_PREFS_SET_BACK_TEXT, String(NULL));
    String str;
    GetString(R::string::app_notifications_dialog_done, &str);
    modIntent->PutExtra(EXTRA_PREFS_SET_NEXT_TEXT, str);

    *intent = modIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

Boolean CRedactionSettingsStandalone::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    return String("Elastos.Droid.Settings.Notification.CRedactionInterstitialFragment").Equals(fragmentName);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos