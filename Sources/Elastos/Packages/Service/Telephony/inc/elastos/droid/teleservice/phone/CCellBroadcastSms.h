#ifndef  __ELASTOS_DROID_PHONE_CCELLBROADCASTSMS_H__
#define  __ELASTOS_DROID_PHONE_CCELLBROADCASTSMS_H__

#include "_Elastos_Droid_TeleService_Phone_CCellBroadcastSms.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Preference.h"

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::PreferenceActivity;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * List of Phone-specific settings screens.
 */
CarClass(CCellBroadcastSms)
    , public PreferenceActivity
{
private:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CCellBroadcastSms* host);

        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* objValue,
            /* [out] */ Boolean* result);

    private:
        CCellBroadcastSms* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCellBroadcastSms::MyHandler")

        MyHandler(
            /* [in] */ CCellBroadcastSms* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CCellBroadcastSms* mHost;
    };

    class CellBroadcastSmsConfig
        : public Object
    {
    public:
        TO_STRING_IMPL("CCellBroadcastSms::CellBroadcastSmsConfig")

        CellBroadcastSmsConfig(
            /* [in] */ CCellBroadcastSms* host)
            : mHost(host)
        {}

        static CARAPI_(void) SetCbSmsConfig(
            /* [in] */ ArrayOf<Int32>* configData);

        static CARAPI_(void) SetCbSmsBSelectedValue(
            /* [in] */ Boolean value,
            /* [in] */ Int32 pos);

        static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetCbSmsBselectedValues();

        // TODO: Change the return value to a RIL_BroadcastSMSConfig
        static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetCbSmsAllValues();

        static CARAPI_(void) SetCbSmsNoOfStructs(
            /* [in] */ Int32 value);

        static CARAPI_(void) SetConfigDataCompleteBSelected(
            /* [in] */ Boolean value,
            /* [in] */ Int32 serviceCategory);

        static CARAPI_(void) SetConfigDataCompleteLanguage(
            /* [in] */ Int32 language);

        static CARAPI_(Int32) GetConfigDataLanguage();

    private:
        CCellBroadcastSms* mHost;

        //The values in this array are stored in a particular order. This order
        //is calculated in the setCbSmsConfig method of this class.
        //For more information see comments below...
        //NO_OF_SERVICE_CATEGORIES +1 is used, because we will leave the first array entry 0
        static AutoPtr<ArrayOf<Int32> > mBSelected;
        static AutoPtr<ArrayOf<Int32> > mConfigDataComplete;
    };

public:
    CAR_OBJECT_DECL();

    CCellBroadcastSms() {}

    CARAPI constructor();

    /**
     * Invoked on each preference click in this hierarchy, overrides
     * PreferenceActivity's implementation.  Used to make sure we track the
     * preference click events.
     */
    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* objValue,
        /* [out] */ Boolean* result);

    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

protected:
    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

private:
    CARAPI_(void) EnableDisableAllCbConfigButtons(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetAllCbConfigButtons(
        /* [in] */ ArrayOf<Int32>* configArray);

private:
    // debug data
    static const String TAG;
    static const Boolean DBG;

    //String keys for preference lookup
    static const String BUTTON_ENABLE_DISABLE_BC_SMS_KEY;
    static const String LIST_LANGUAGE_KEY;
    static const String BUTTON_EMERGENCY_BROADCAST_KEY;
    static const String BUTTON_ADMINISTRATIVE_KEY;
    static const String BUTTON_MAINTENANCE_KEY;
    static const String BUTTON_LOCAL_WEATHER_KEY;
    static const String BUTTON_ATR_KEY;
    static const String BUTTON_LAFS_KEY;
    static const String BUTTON_RESTAURANTS_KEY;
    static const String BUTTON_LODGINGS_KEY;
    static const String BUTTON_RETAIL_DIRECTORY_KEY;
    static const String BUTTON_ADVERTISEMENTS_KEY;
    static const String BUTTON_STOCK_QUOTES_KEY;
    static const String BUTTON_EO_KEY;
    static const String BUTTON_MHH_KEY;
    static const String BUTTON_TECHNOLOGY_NEWS_KEY;
    static const String BUTTON_MULTI_CATEGORY_KEY;

    static const String BUTTON_LOCAL_GENERAL_NEWS_KEY;
    static const String BUTTON_REGIONAL_GENERAL_NEWS_KEY;
    static const String BUTTON_NATIONAL_GENERAL_NEWS_KEY;
    static const String BUTTON_INTERNATIONAL_GENERAL_NEWS_KEY;

    static const String BUTTON_LOCAL_BF_NEWS_KEY;
    static const String BUTTON_REGIONAL_BF_NEWS_KEY;
    static const String BUTTON_NATIONAL_BF_NEWS_KEY;
    static const String BUTTON_INTERNATIONAL_BF_NEWS_KEY;

    static const String BUTTON_LOCAL_SPORTS_NEWS_KEY;
    static const String BUTTON_REGIONAL_SPORTS_NEWS_KEY;
    static const String BUTTON_NATIONAL_SPORTS_NEWS_KEY;
    static const String BUTTON_INTERNATIONAL_SPORTS_NEWS_KEY;

    static const String BUTTON_LOCAL_ENTERTAINMENT_NEWS_KEY;
    static const String BUTTON_REGIONAL_ENTERTAINMENT_NEWS_KEY;
    static const String BUTTON_NATIONAL_ENTERTAINMENT_NEWS_KEY;
    static const String BUTTON_INTERNATIONAL_ENTERTAINMENT_NEWS_KEY;

    //Class constants
    //These values are related to the C structs. See the comments in  method
    //setCbSmsConfig for more information.
    static const Int32 NO_OF_SERVICE_CATEGORIES;
    static const Int32 NO_OF_INTS_STRUCT_1;
    static const Int32 MAX_LENGTH_RESULT;
    //Handler keys
    static const Int32 MESSAGE_ACTIVATE_CB_SMS;
    static const Int32 MESSAGE_GET_CB_SMS_CONFIG;
    static const Int32 MESSAGE_SET_CB_SMS_CONFIG;

    //UI objects
    AutoPtr<ICheckBoxPreference> mButtonBcSms;

    AutoPtr<IListPreference> mListLanguage;

    AutoPtr<ICheckBoxPreference> mButtonEmergencyBroadcast;
    AutoPtr<ICheckBoxPreference> mButtonAdministrative;
    AutoPtr<ICheckBoxPreference> mButtonMaintenance;
    AutoPtr<ICheckBoxPreference> mButtonLocalWeather;
    AutoPtr<ICheckBoxPreference> mButtonAtr;
    AutoPtr<ICheckBoxPreference> mButtonLafs;
    AutoPtr<ICheckBoxPreference> mButtonRestaurants;
    AutoPtr<ICheckBoxPreference> mButtonLodgings;
    AutoPtr<ICheckBoxPreference> mButtonRetailDirectory;
    AutoPtr<ICheckBoxPreference> mButtonAdvertisements;
    AutoPtr<ICheckBoxPreference> mButtonStockQuotes;
    AutoPtr<ICheckBoxPreference> mButtonEo;
    AutoPtr<ICheckBoxPreference> mButtonMhh;
    AutoPtr<ICheckBoxPreference> mButtonTechnologyNews;
    AutoPtr<ICheckBoxPreference> mButtonMultiCategory;

    AutoPtr<ICheckBoxPreference> mButtonLocal1;
    AutoPtr<ICheckBoxPreference> mButtonRegional1;
    AutoPtr<ICheckBoxPreference> mButtonNational1;
    AutoPtr<ICheckBoxPreference> mButtonInternational1;

    AutoPtr<ICheckBoxPreference> mButtonLocal2;
    AutoPtr<ICheckBoxPreference> mButtonRegional2;
    AutoPtr<ICheckBoxPreference> mButtonNational2;
    AutoPtr<ICheckBoxPreference> mButtonInternational2;

    AutoPtr<ICheckBoxPreference> mButtonLocal3;
    AutoPtr<ICheckBoxPreference> mButtonRegional3;
    AutoPtr<ICheckBoxPreference> mButtonNational3;
    AutoPtr<ICheckBoxPreference> mButtonInternational3;

    AutoPtr<ICheckBoxPreference> mButtonLocal4;
    AutoPtr<ICheckBoxPreference> mButtonRegional4;
    AutoPtr<ICheckBoxPreference> mButtonNational4;
    AutoPtr<ICheckBoxPreference> mButtonInternational4;

    //Member variables
    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCELLBROADCASTSMS_H__