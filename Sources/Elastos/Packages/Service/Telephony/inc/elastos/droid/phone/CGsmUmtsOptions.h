#ifndef  __ELASTOS_DROID_PHONE_CGSMUMTSOPTIONS_H__
#define  __ELASTOS_DROID_PHONE_CGSMUMTSOPTIONS_H__

#include "_Elastos_Droid_Phone_CGsmUmtsOptions.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * List of Network-specific settings screens.
 */
CarClass(CGsmUmtsOptions)
    , public Object
    , public IGsmUmtsOptions
{
private:
    class MyPreferenceOnPreferenceClickListener
        : public Object
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        TO_STRING_IMPL("CGsmUmtsOptions::MyPreferenceOnPreferenceClickListener")

        MyPreferenceOnPreferenceClickListener(
            /* [in] */ CGsmUmtsOptions* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        CGsmUmtsOptions* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IPreferenceActivity* prefActivity,
        /* [in] */ IPreferenceScreen* prefScreen);

    CARAPI PreferenceTreeClick(
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

protected:
    CARAPI Create();

    CARAPI Log(
        /* [in] */ const String& s);

private:
    static const String LOG_TAG = "GsmUmtsOptions";

    AutoPtr<IPreferenceScreen> mButtonAPNExpand;
    AutoPtr<IPreferenceScreen> mButtonOperatorSelectionExpand;

    static const String BUTTON_APN_EXPAND_KEY = "button_apn_key";
    static const String BUTTON_OPERATOR_SELECTION_EXPAND_KEY = "button_carrier_sel_key";
    static const String BUTTON_CARRIER_SETTINGS_KEY = "carrier_settings_key";
    AutoPtr<IPreferenceActivity> mPrefActivity;
    AutoPtr<IPreferenceScreen> mPrefScreen;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CGSMUMTSOPTIONS_H__