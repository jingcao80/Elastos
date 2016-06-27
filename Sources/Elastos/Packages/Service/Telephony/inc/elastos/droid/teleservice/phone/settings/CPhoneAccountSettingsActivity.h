#ifndef  __ELASTOS_DROID_PHONE_SETTINGS_CPHONEACCOUNTSETTINGSACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_SETTINGS_CPHONEACCOUNTSETTINGSACTIVITY_H__

#include "_Elastos_Droid_TeleService_Phone_Settings_CPhoneAccountSettingsActivity.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::View::IMenuItem;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {
namespace Settings {

CarClass(CPhoneAccountSettingsActivity)
    , public PreferenceActivity
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);
};

} // namespace Settings
} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_SETTINGS_CPHONEACCOUNTSETTINGSACTIVITY_H__