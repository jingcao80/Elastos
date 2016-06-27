
#ifndef  __ELASTOS_DROID_PHONE_CCDMACALLOPTIONS_H__
#define  __ELASTOS_DROID_PHONE_CCDMACALLOPTIONS_H__

#include "_Elastos_Droid_TeleService_Phone_CCdmaCallOptions.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/preference/PreferenceActivity.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::PreferenceActivity;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CCdmaCallOptions)
    , public PreferenceActivity
{
public:
    CAR_OBJECT_DECL();

    CCdmaCallOptions();

    CARAPI constructor();

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    static const String TAG;
    Boolean DBG;

    static const String BUTTON_VP_KEY;
    AutoPtr<ICheckBoxPreference> mButtonVoicePrivacy;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CCDMACALLOPTIONS_H__
