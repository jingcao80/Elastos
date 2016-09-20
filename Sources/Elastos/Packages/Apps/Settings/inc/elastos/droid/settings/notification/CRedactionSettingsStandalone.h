#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONSETTINGSSTANDALONE_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONSETTINGSSTANDALONE_H__

#include "_Elastos_Droid_Settings_Notification_CRedactionSettingsStandalone.h"
#include "elastos/droid/settings/SettingsActivity.h"

using Elastos::Droid::Settings::SettingsActivity;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

/** Wrapper to allow external activites to jump directly to the {@link RedactionInterstitial} */
CarClass(CRedactionSettingsStandalone)
    , public SettingsActivity
{
public:
    CAR_OBJECT_DECL()

    CRedactionSettingsStandalone();

    ~CRedactionSettingsStandalone();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

protected:
    //@Override
    CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);
};

} // namespace NOtification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONSETTINGSSTANDALONE_H__