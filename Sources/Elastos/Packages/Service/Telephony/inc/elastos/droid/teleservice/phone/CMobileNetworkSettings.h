#ifndef  __ELASTOS_DROID_PHONE_CMOBILENETWORKSETTINGS_H__
#define  __ELASTOS_DROID_PHONE_CMOBILENETWORKSETTINGS_H__

#include "_Elastos_Droid_TeleService_Phone_CMobileNetworkSettings.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::View::IMenuItem;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * "Mobile network settings" screen.  This preference screen lets you
 * enable/disable mobile data, and control data roaming and other
 * network-specific mobile data features.  It's used on non-voice-capable
 * tablets as well as regular phone devices.
 *
 * Note that this PreferenceActivity is part of the phone app, even though
 * you reach it from the "Wireless & Networks" section of the main
 * Settings app.  It's not part of the "Call settings" hierarchy that's
 * available from the Phone app (see CallFeaturesSetting for that.)
 */
CarClass(CMobileNetworkSettings)
    , public PreferenceActivity
    , public IMobileNetworkSettings
    , public IDialogInterfaceOnClickListener
    , public IDialogInterfaceOnDismissListener
    , public IPreferenceOnPreferenceChangeListener
{
private:
    class MyHandler
       : public Handler
    {
    public:
        TO_STRING_IMPL("CMobileNetworkSettings::MyHandler")

        MyHandler(
            CMobileNetworkSettings* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleGetPreferredNetworkTypeResponse(
            /* [in] */ IMessage* msg);

        CARAPI_(void) HandleSetPreferredNetworkTypeResponse(
            /* [in] */ IMessage* msg);

        CARAPI_(void) ResetNetworkModeToDefault();

    private:
        friend class CMobileNetworkSettings;

        CMobileNetworkSettings* mHost;

        static const Int32 MESSAGE_GET_PREFERRED_NETWORK_TYPE = 0;
        static const Int32 MESSAGE_SET_PREFERRED_NETWORK_TYPE = 1;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CMobileNetworkSettings();

    CARAPI constructor();

    //This is a method implemented for DialogInterface.OnClickListener.
    //  Used to dismiss the dialogs when they come up.
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    //@Override
    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

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

    /**
     * Implemented to support onPreferenceChangeListener to look for preference
     * changes specifically on CLIR.
     *
     * @param preference is the preference to be changed, should be mButtonCLIR.
     * @param objValue should be the value of the selection, NOT its localized
     * display value.
     */
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* objValue,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

private:
    CARAPI_(void) SetIMS(
        /* [in] */ Boolean turnOn);

    CARAPI_(void) UpdatePreferredNetworkModeSummary(
        /* [in] */ Int32 NetworkMode);

    CARAPI_(void) UpdateEnabledNetworksValueAndSummary(
        /* [in] */ Int32 NetworkMode);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

    static CARAPI_(void) Loge(
        /* [in] */ const String& msg);

private:
    // debug data
    static const String TAG;
    static const Boolean DBG;

    //String keys for preference lookup
    static const String BUTTON_PREFERED_NETWORK_MODE;
    static const String BUTTON_ROAMING_KEY;
    static const String BUTTON_CDMA_LTE_DATA_SERVICE_KEY;
    static const String BUTTON_ENABLED_NETWORKS_KEY;
    static const String BUTTON_4G_LTE_KEY;
    static const String BUTTON_CELL_BROADCAST_SETTINGS;

    static const Int32 sPreferredNetworkMode;

    //Information about logical "up" Activity
    static const String UP_ACTIVITY_PACKAGE;
    static const String UP_ACTIVITY_CLASS;

    //UI objects
    AutoPtr<IListPreference> mButtonPreferredNetworkMode;
    AutoPtr<IListPreference> mButtonEnabledNetworks;
    AutoPtr<ISwitchPreference> mButtonDataRoam;
    AutoPtr<ISwitchPreference> mButton4glte;
    AutoPtr<IPreference> mLteDataServicePref;

    static const String sIface; //TODO: this will go away

    AutoPtr<IUserManager> mUm;
    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
    Boolean mOkClicked;

    //GsmUmts options and Cdma options
    AutoPtr<IGsmUmtsOptions> mGsmUmtsOptions;
    AutoPtr<ICdmaOptions> mCdmaOptions;

    AutoPtr<IPreference> mClickedPreference;
    Boolean mShow4GForLTE;
    Boolean mIsGlobalCdma;
    Boolean mUnavailable;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CMOBILENETWORKSETTINGS_H__