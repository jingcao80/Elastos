#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPEDITOR_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPEDITOR_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_Sip_CSipEditor.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipSharedPreferences.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipProfileDb.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipAccountRegistry.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::IButton;
using Elastos::Core::Object;
using Elastos::Core::IThrowable;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

CarClass(CSipEditor)
    , public PreferenceActivity
    , public ISipEditor
{
private:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        TO_STRING_IMPL("CSipEditor::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CSipEditor* host);

        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* pref,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);
    private:
        CSipEditor* mHost;
    };

    class AdvancedSettings
        : public Object
    {
    private:
        class InnerListener
            : public Object
            , public IPreferenceOnPreferenceClickListener
        {
        public:
            TO_STRING_IMPL("CSipEditor::AdvancedSettings::InnerListener")

            CAR_INTERFACE_DECL()

            InnerListener(
                /* [in] */ AdvancedSettings* host);

            CARAPI OnPreferenceClick(
                /* [in] */ IPreference* pref,
                /* [out] */ Boolean* result);
        private:
            AdvancedSettings* mHost;
        };

    public:
        TO_STRING_IMPL("CSipEditor::AdvancedSettings")

        AdvancedSettings(
            /* [in] */ CSipEditor* host);

        CARAPI Show();

        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        CARAPI_(void) LoadAdvancedPreferences();

        CARAPI_(void) Hide();

    private:
        CSipEditor* mHost;
        AutoPtr<IPreference> mAdvancedSettingsTrigger;
        AutoPtr<ArrayOf<IPreference*> > mPreferences;
        Boolean mShowing;
    };

    class PreferenceKey
        : public Object
    {
    public:
        TO_STRING_IMPL("CSipEditor::PreferenceKey")

        /**
         * @param key The key name of the preference.
         * @param initValue The initial value of the preference.
         * @param defaultSummary The default summary value of the preference
         *        when the preference value is empty.
         */
        PreferenceKey(
            /* [in] */ Int32 text,
            /* [in] */ Int32 initValue,
            /* [in] */ Int32 defaultSummary)
            : mText()
            , mInitValue()
            , mDefaultSummary()
        {}

        CARAPI GetValue(
            /* [out] */ String* value);

        CARAPI_(void) SetValue(
            /* [in] */ const String& value);

    private:
        friend class CSipEditor;
        Int32 mText;
        Int32 mInitValue;
        Int32 mDefaultSummary;
        AutoPtr<IPreference> mPreference;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CSipEditor::MyRunnable")

        MyRunnable(
            /* [in] */ CSipEditor* host,
            /* [in] */ const String& message)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CSipEditor* mHost;
        String mMessage;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL()

    CSipEditor();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* pref,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

private:
    /**
     * Saves a {@link SipProfile} and registers the associated
     * {@link android.telecom.PhoneAccount}.
     *
     * @param p The {@link SipProfile} to register.
     * @throws IOException Exception resulting from profile save.
     */
    // CARAPI SaveAndRegisterProfile(
    //     /* [in] */ SipProfile* p);

    /**
     * Deletes a {@link SipProfile} and un-registers the associated
     * {@link android.telecom.PhoneAccount}.
     *
     * @param p The {@link SipProfile} to delete.
     */
    // CARAPI_(void) DeleteAndUnregisterProfile(
    //     /* [in] */ SipProfile* p);

    CARAPI_(void) SetRemovedProfileAndFinish();

    CARAPI_(void) ShowAlert(
        /* [in] */ IThrowable* e);

    CARAPI_(void) ShowAlert(
        /* [in] */ const String& message);

    CARAPI_(Boolean) IsEditTextEmpty(
        /* [in] */ PreferenceKey* key);

    CARAPI_(void) ValidateAndSetResult();

    // CARAPI_(void) ReplaceProfile(
    //     /* [in] */ SipProfile* oldProfile,
    //     /* [in] */ SipProfile* newProfile);

    CARAPI_(String) GetProfileName();

    // CARAPI CreateSipProfile(
    //     /* [out] */ SipProfile** profile);

    CARAPI GetPreferenceKey(
        /* [in] */ IPreference* pref,
        /* [out] */ PreferenceKey** key);

    // CARAPI_(void) LoadPreferencesFromProfile(
    //     /* [in] */ SipProfile* p);

    CARAPI_(Boolean) IsAlwaysSendKeepAlive();

    CARAPI_(void) SetCheckBox(
        /* [in] */ PreferenceKey* key,
        /* [in] */ Boolean checked);

    CARAPI_(void) SetupPreference(
        /* [in] */ IPreference* pref);

    CARAPI_(void) CheckIfDisplayNameSet();

    static CARAPI_(String) GetDefaultDisplayName();

    static CARAPI_(String) Scramble(
        /* [in] */ const String& s);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */

    static const Int32 MENU_SAVE;
    static const Int32 MENU_DISCARD;
    static const Int32 MENU_REMOVE;

    static const String KEY_PROFILE;
    static const String GET_METHOD_PREFIX;
    static const Char32 SCRAMBLED;
    static const Int32 NA;

    AutoPtr<AdvancedSettings> mAdvancedSettings;
    AutoPtr<SipSharedPreferences> mSharedPreferences;
    Boolean mDisplayNameSet;
    Boolean mHomeButtonClicked;
    Boolean mUpdateRequired;

    AutoPtr<SipProfileDb> mProfileDb;
    //SipProfile mOldProfile;
    AutoPtr<IButton> mRemoveButton;
    AutoPtr<SipAccountRegistry> mSipAccountRegistry;

    AutoPtr<PreferenceKey> mUsername;
    AutoPtr<PreferenceKey> mPassword;
    AutoPtr<PreferenceKey> mDomainAddress;
    AutoPtr<PreferenceKey> mDisplayName;
    AutoPtr<PreferenceKey> mProxyAddress;
    AutoPtr<PreferenceKey> mPort;
    AutoPtr<PreferenceKey> mTransport;
    AutoPtr<PreferenceKey> mSendKeepAlive;
    AutoPtr<PreferenceKey> mAuthUserName;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPEDITOR_H__