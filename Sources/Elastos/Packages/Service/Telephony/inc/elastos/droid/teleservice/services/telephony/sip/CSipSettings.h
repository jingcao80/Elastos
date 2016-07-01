#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPSETTING_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPSETTING_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_Sip_CSipSettings.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipProfileDb.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipSharedPreferences.h"
#include "elastos/droid/preference/Preference.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::Preference;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

CarClass(CSipSettings)
    , public PreferenceActivity
{
private:
    class SipPreference
       : public Elastos::Droid::Preference::Preference
    {
    public:
        TO_STRING_IMPL("CSipSettings::SipPreference")

        // SipPreference(
        //     /* [in] */ IContext* c,
        //     /* [in] */ SipProfile* p,
        //     /* [in] */ CSipSettings* host);

        // CARAPI_(AutoPtr<SipProfile>) GetProfile();

        // CARAPI SetProfile(
        //     /* [in] */ SipProfile* p);

        CARAPI UpdateSummary(
            /* [in] */ const String& registrationStatus);

    // public:
    //     AutoPtr<SipProfile> mProfile;
    private:
        CSipSettings* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CSipSettings::MyRunnable")

        MyRunnable(
            /* [in] */ CSipSettings* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CSipSettings* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CSipSettings::MyRunnable2")

        MyRunnable2(
            /* [in] */ CSipSettings* host,
            /* [in] */ const String& profileUri,
            /* [in] */ const String& message)
            : mHost(host)
            , mProfileUri(profileUri)
            , mMessage(message)
        {}

        //@Override
        CARAPI Run();

    private:
        CSipSettings* mHost;
        String mProfileUri;
        String mMessage;
    };

public:
    CAR_OBJECT_DECL();

    CSipSettings();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    // CARAPI DeleteProfile(
    //     /* [in] */ SipProfile* p);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean * result);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean * result);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean * result);

protected:
    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(String) GetPackageNameFromUid(
        /* [in] */ Int32 uid);

    CARAPI_(void) UpdateProfilesStatus();

    // CARAPI_(String) GetProfileName(
    //     /* [in] */ SipProfile* profile);

    CARAPI RetrieveSipLists();

    CARAPI_(void) ProcessActiveProfilesFromSipService();

    // CARAPI_(AutoPtr<SipProfile>) SipProfile GetProfileFromList(
    //     /* [in] */ SipProfile* activeProfile);

    // CARAPI_(void) AddPreferenceFor(
    //     /* [in] */ SipProfile* p);

    // CARAPI_(void) HandleProfileClick(
    //     /* [in] */ SipProfile* profile);

    // CARAPI_(void) UnregisterProfile(
    //     /* [in] */ SipProfile* p);

    // CARAPI AddProfile(
    //     /* [in] */ SipProfile* p);

    // CARAPI_(void) StartSipEditor(
    //     /* [in] */ SipProfile* profile);

    CARAPI_(void) ShowRegistrationMessage(
        /* [in] */ const String& profileUri,
        /* [in] */ const String& message);

    //CARAPI_(AutoPtr<ISipRegistrationListener>) CreateRegistrationListener();

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

public:
    static const String SIP_SHARED_PREFERENCES;

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */

    static const Int32 MENU_ADD_ACCOUNT;

    static const String KEY_SIP_PROFILE;

    static const String PREF_SIP_LIST;

    static const Int32 REQUEST_ADD_OR_EDIT_SIP_PROFILE;

    AutoPtr<IPackageManager> mPackageManager;
    //SipManager mSipManager;
    AutoPtr<SipProfileDb> mProfileDb;

    //SipProfile mProfile; // profile that's being edited

    AutoPtr<IPreferenceCategory> mSipListContainer;
    AutoPtr<IMap> mSipPreferenceMap;
    AutoPtr<IList> mSipProfileList;
    AutoPtr<SipSharedPreferences> mSipSharedPreferences;
    Int32 mUid;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPSETTING_H__