#ifndef  __ELASTOS_DROID_PHONE_CPHONEAPP_H__
#define  __ELASTOS_DROID_PHONE_CPHONEAPP_H__

#include "_Elastos_Droid_Phone_CCdmaCallOptions.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CCdmaCallOptions)
    , public PreferenceActivity
    , public ICdmaCallOptions
{
public:
    CAR_INTERFACE_DECL()

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
    static const String LOG_TAG;
    Boolean DBG;

    static const String BUTTON_VP_KEY;
    AutoPtr<ICheckBoxPreference> mButtonVoicePrivacy;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CPHONEAPP_H__