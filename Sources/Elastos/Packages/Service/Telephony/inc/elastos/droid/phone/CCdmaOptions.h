#ifndef  __ELASTOS_DROID_PHONE_CCDMAOPTIONS_H__
#define  __ELASTOS_DROID_PHONE_CCDMAOPTIONS_H__

#include "_Elastos_Droid_Phone_CCdmaOptions.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * List of Phone-specific settings screens.
 */
CarClass(CCdmaOptions)
    , public Object
    , public ICdmaOptions
{
private:
    class MyOnPreferenceClickListener
        : public Object
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        TO_STRING_IMPL("CCdmaOptions::MyOnPreferenceClickListener")

        CAR_INTERFACE_DECL()

        MyOnPreferenceClickListener(
            /* [in] */ CCdmaOptions* host)
            : mHost(host)
        {}

        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        CCdmaOptions* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CCdmaOptions() {}

    CARAPI constructor(
        /* [in] */ IPreferenceActivity* prefActivity,
        /* [in] */ IPreferenceScreen* prefScreen,
        /* [in] */ IPhone* phone);

    CARAPI Create();

    CARAPI PreferenceTreeClick(
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    CARAPI ShowDialog(
        /* [in] */ IPreference* preference);

protected:
    CARAPI_(void) Log(
        /* [in] */ const String& s);

private:
    CARAPI_(Boolean) DeviceSupportsNvAndRuim();

private:
    static const String LOG_TAG = "CdmaOptions";

    AutoPtr<ICdmaSystemSelectListPreference> mButtonCdmaSystemSelect;
    AutoPtr<ICdmaSubscriptionListPreference> mButtonCdmaSubscription;
    AutoPtr<IPreferenceScreen> mButtonAPNExpand;

    static const String BUTTON_CDMA_SYSTEM_SELECT_KEY = "cdma_system_select_key";
    static const String BUTTON_CDMA_SUBSCRIPTION_KEY = "cdma_subscription_key";
    static const String BUTTON_CDMA_ACTIVATE_DEVICE_KEY = "cdma_activate_device_key";
    static const String BUTTON_CARRIER_SETTINGS_KEY = "carrier_settings_key";
    static const String BUTTON_APN_EXPAND_KEY = "button_apn_key";

    AutoPtr<IPreferenceActivity> mPrefActivity;
    AutoPtr<IPreferenceScreen> mPrefScreen;
    AutoPtr<IPhone> mPhone;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCDMAOPTIONS_H__