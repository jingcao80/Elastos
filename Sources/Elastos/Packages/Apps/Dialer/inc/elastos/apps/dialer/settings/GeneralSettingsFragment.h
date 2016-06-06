#ifndef __ELASTOS_APPS_DIALER_SETTINGS_GENERALSETTINGSFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_SETTINGS_GENERALSETTINGSFRAGMENT_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Settings {

class GeneralSettingsFragment
    : public PreferenceFragment
    , public IGeneralSettingsFragment
    , public IPreferenceOnPreferenceChangeListener
{
private:
    class RingtoneLookupCompleteHandler
        : public Handler
    {
    public:
        RingtoneLookupCompleteHandler(
            /* [in] */ GeneralSettingsFragment* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GeneralSettingsFragment* mHost;
    }

    class RingtoneLookupRunnable
        : public Runnable
    {
    public:
        RingtoneLookupRunnable(
            /* [in] */ GeneralSettingsFragment* host);

        // @Override
        CARAPI Run();

    private:
        GeneralSettingsFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GeneralSettingsFragment();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Supports onPreferenceChangeListener to look for preference changes.
     *
     * @param preference The preference to be changed
     * @param objValue The value of the selection, NOT its localized display value.
     */
    // @Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* objValue,
        /* [out] */ Boolean* result);

    /**
     * Click listener for toggle events.
     */
    // @Override
    CARAPI OnPreferenceTreeClick(
        /*[in]*/ IPreferenceScreen* preferenceScreen,
        /*[in]*/ IPreference* preference,
        /*[out]*/ Boolean* result);

    // @Override
    CARAPI OnResume();

private:
    static const String CATEGORY_SOUNDS_KEY; //    = "dialer_general_sounds_category_key";
    static const String BUTTON_RINGTONE_KEY; //    = "button_ringtone_key";
    static const String BUTTON_VIBRATE_ON_RING; // = "button_vibrate_on_ring";
    static const String BUTTON_PLAY_DTMF_TONE; //  = "button_play_dtmf_tone";
    static const String BUTTON_RESPOND_VIA_SMS_KEY; // = "button_respond_via_sms_key";

    static const Int32 MSG_UPDATE_RINGTONE_SUMMARY; // = 1;

    AutoPtr<IContext> mContext;

    AutoPtr<IPreference> mRingtonePreference;
    AutoPtr<ICheckBoxPreference> mVibrateWhenRinging;
    AutoPtr<ICheckBoxPreference> mPlayDtmfTone;
    AutoPtr<IPreference> mRespondViaSms;

    AutoPtr<IRunnable> mRingtoneLookupRunnable;
    AutoPtr<IHandler> mRingtoneLookupComplete;
};

} // Settings
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_SETTINGS_GENERALSETTINGSFRAGMENT_H__
