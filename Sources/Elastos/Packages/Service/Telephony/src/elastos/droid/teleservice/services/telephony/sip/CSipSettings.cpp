
#include "elastos/droid/teleservice/services/telephony/sip/CSipSettings.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecom::CTelecomManagerHelper;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

// SipPreference(
//     /* [in] */ IContext* c,
//     /* [in] */ SipProfile* p,
//     /* [in] */ CSipSettings* host);

// CARAPI_(AutoPtr<SipProfile>) GetProfile();

// CARAPI SetProfile(
//     /* [in] */ SipProfile* p);

ECode CSipSettings::SipPreference::UpdateSummary(
    /* [in] */ const String& registrationStatus)
{
    assert(0);
    Int32 profileUid;// = mProfile.getCallingUid();
    if (VERBOSE) {
        StringBuilder sb;
        sb += "SipPreference.updateSummary, profile uid: ";
        sb += profileUid;
        sb += " registration: ";
        sb += registrationStatus;
        sb += " status: ";
        sb += registrationStatus;
        Log(sb.ToString());
    }
    String summary("");
    if ((profileUid > 0) && (profileUid != mHost->mUid)) {
        // from third party apps
        String str = mHost->GetPackageNameFromUid(profileUid);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, TO_IINTERFACE(cchar));
        mHost->GetString(R::string::third_party_account_summary, array, &summary);
    }
    else {
        summary = registrationStatus;
    }
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(summary);
    SetSummary(cchar);
}

ECode CSipSettings::MyRunnable::Run()
{
    //try {
    ECode ec = NOERROR;
    if(FAILED(ec = mHost->RetrieveSipLists())) {
    //} catch (Exception e) {
        Log(String("updateProfilesStatus, exception: ") + StringUtils::ToString(ec));
    }
    return NOERROR;
}

ECode CSipSettings::MyRunnable2::Run()
{
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mProfileUri);
    AutoPtr<IInterface> obj;
    mHost->mSipPreferenceMap->Get(TO_IINTERFACE(cchar), (IInterface**)&obj);
    AutoPtr<SipPreference> pref = (SipPreference*)IObject::Probe(obj);
    if (pref != NULL) {
        pref->UpdateSummary(mMessage);
    }
    return NOERROR;
}

const String CSipSettings::SIP_SHARED_PREFERENCES("SIP_PREFERENCES");

const String CSipSettings::PREFIX("[SipSettings] ");
const Boolean CSipSettings::VERBOSE = FALSE; /* STOP SHIP if true */

const Int32 CSipSettings::MENU_ADD_ACCOUNT = IMenu::FIRST;

const String CSipSettings::KEY_SIP_PROFILE("sip_profile");

const String CSipSettings::PREF_SIP_LIST("sip_account_list");

const Int32 CSipSettings::REQUEST_ADD_OR_EDIT_SIP_PROFILE = 1;

CAR_OBJECT_IMPL(CSipSettings)

CSipSettings::CSipSettings()
{
    mUid = Process::MyUid();
}

String CSipSettings::GetPackageNameFromUid(
    /* [in] */ Int32 uid)
{
    //try
    ECode ec = NOERROR;
    {
        AutoPtr<ArrayOf<String> > pkgs;
        FAIL_GOTO(ec = mPackageManager->GetPackagesForUid(uid, (ArrayOf<String>**)&pkgs), ERROR)
        AutoPtr<IApplicationInfo> ai;
        FAIL_GOTO(ec = mPackageManager->GetApplicationInfo((*pkgs)[0], 0, (IApplicationInfo**)&ai), ERROR)
        AutoPtr<ICharSequence> cchar;
        IPackageItemInfo::Probe(ai)->LoadLabel(mPackageManager, (ICharSequence**)&cchar);
        String str;
        cchar->ToString(&str);
        return str;
    }
    // catch (PackageManager.NameNotFoundException e) {
ERROR:
    if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
        StringBuilder sb;
        sb += "getPackageNameFromUid, cannot find name of uid: ";
        sb += uid;
        sb += ", exception: ";
        sb += ec;
        Log(sb.ToString());
    }
    return String("uid:") + StringUtils::ToString(uid);
}

ECode CSipSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PreferenceActivity::OnCreate(savedInstanceState);

    assert(0);
    //mSipManager = SipManager.newInstance(this);
    mSipSharedPreferences = new SipSharedPreferences((IContext*)this);
    mProfileDb = new SipProfileDb((IContext*)this);

    GetPackageManager((IPackageManager**)&mPackageManager);
    SetContentView(R::layout::sip_settings_ui);
    AddPreferencesFromResource(R::xml::sip_setting);
    AutoPtr<IPreference> p;
    FindPreference(PREF_SIP_LIST, (IPreference**)&p);
    mSipListContainer = IPreferenceCategory::Probe(p);

    UpdateProfilesStatus();

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
}

ECode CSipSettings::OnResume()
{
    return PreferenceActivity::OnResume();
}

ECode CSipSettings::OnDestroy()
{
    PreferenceActivity::OnDestroy();

    AutoPtr<IListView> view;
    GetListView((IListView**)&view);
    return UnregisterForContextMenu(IView::Probe(view));
}

ECode CSipSettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* intent)
{
    if (resultCode != RESULT_OK && resultCode != RESULT_FIRST_USER) return NOERROR;
    assert(0);
    // new Thread() {
    //     @Override
    //     public void run() {
    //         try {
    //             if (mProfile != null) {
    //                 if (VERBOSE) log("onActivityResult, remove: " + mProfile.getProfileName());
    //                 deleteProfile(mProfile);
    //             }

    //             SipProfile profile = intent.getParcelableExtra(KEY_SIP_PROFILE);
    //             if (resultCode == RESULT_OK) {
    //                 if (VERBOSE) log("onActivityResult, new: " + profile.getProfileName());
    //                 addProfile(profile);
    //             }
    //             updateProfilesStatus();
    //         } catch (IOException e) {
    //             log("onActivityResult, can not handle the profile:  " + e);
    //         }
    //     }
    // }.start();
}

void CSipSettings::UpdateProfilesStatus()
{
    AutoPtr<IRunnable> r = new MyRunnable(this);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
}

// CARAPI_(String) GetProfileName(
//     /* [in] */ SipProfile* profile);

ECode CSipSettings::RetrieveSipLists()
{
    CLinkedHashMap::New((IMap**)&mSipPreferenceMap);
    mProfileDb->RetrieveSipProfileList((IList**)&mSipProfileList);
    ProcessActiveProfilesFromSipService();
    assert(0);
    // Collections.sort(mSipProfileList, new Comparator<SipProfile>() {
    //     @Override
    //     public int compare(SipProfile p1, SipProfile p2) {
    //         return getProfileName(p1).compareTo(getProfileName(p2));
    //     }

    //     public boolean equals(SipProfile p) {
    //         // not used
    //         return false;
    //     }
    // });
    IPreferenceGroup::Probe(mSipListContainer)->RemoveAll();
    Boolean res;
    if (mSipProfileList->IsEmpty(&res), res) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(IPreference::Probe(mSipListContainer), &res);
    }
    else {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->AddPreference(IPreference::Probe(mSipListContainer), &res);
        assert(0);
        // for (SipProfile p : mSipProfileList) {
        //     addPreferenceFor(p);
        // }
    }

    if (mSipSharedPreferences->IsReceivingCallsEnabled(&res), !res) return NOERROR;
    assert(0);
    // for (SipProfile p : mSipProfileList) {
    //     if (mUid == p.getCallingUid()) {
    //         try {
    //             mSipManager.setRegistrationListener(
    //                     p.getUriString(), createRegistrationListener());
    //         } catch (SipException e) {
    //             log("retrieveSipLists, cannot set registration listener: " + e);
    //         }
    //     }
    // }
    return NOERROR;
}

void CSipSettings::ProcessActiveProfilesFromSipService()
{
    assert(0);
    // SipProfile[] activeList = mSipManager.getListOfProfiles();
    // for (SipProfile activeProfile : activeList) {
    //     SipProfile profile = getProfileFromList(activeProfile);
    //     if (profile == null) {
    //         mSipProfileList.add(activeProfile);
    //     }
    //     else {
    //         profile.setCallingUid(activeProfile.getCallingUid());
    //     }
    // }
}

// CARAPI_(AutoPtr<SipProfile>) SipProfile GetProfileFromList(
//     /* [in] */ SipProfile* activeProfile);

// CARAPI_(void) AddPreferenceFor(
//     /* [in] */ SipProfile* p);

// CARAPI_(void) HandleProfileClick(
//     /* [in] */ SipProfile* profile);

// CARAPI_(void) UnregisterProfile(
//     /* [in] */ SipProfile* p);

// CARAPI DeleteProfile(
//     /* [in] */ SipProfile* p);

// CARAPI AddProfile(
//     /* [in] */ SipProfile* p);

// CARAPI_(void) StartSipEditor(
//     /* [in] */ SipProfile* profile);

void CSipSettings::ShowRegistrationMessage(
    /* [in] */ const String& profileUri,
    /* [in] */ const String& message)
{
    AutoPtr<IRunnable> r = new MyRunnable2(this, profileUri, message);
    RunOnUiThread(r);
}

//CARAPI_(AutoPtr<ISipRegistrationListener>) CreateRegistrationListener();

ECode CSipSettings::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    PreferenceActivity::OnCreateOptionsMenu(menu, &res);
    AutoPtr<IMenuItem> item;
    menu->Add(0, MENU_ADD_ACCOUNT, 0, R::string::add_sip_account, (IMenuItem**)&item);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);
    *result = TRUE;
    return NOERROR;
}

ECode CSipSettings::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMenuItem> item;
    menu->FindItem(MENU_ADD_ACCOUNT, (IMenuItem**)&item);
    item->SetEnabled(SipUtil::IsPhoneIdle(this));
    return PreferenceActivity::OnPrepareOptionsMenu(menu, result);
}

ECode CSipSettings::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    switch (itemId) {
        case MENU_ADD_ACCOUNT: {
            assert(0);
            //StartSipEditor(NULL);
            *result = TRUE;
            return NOERROR;
        }
        case Elastos::Droid::R::id::home: {
            OnBackPressed();
            *result = TRUE;
            return NOERROR;
        }
    }
    return PreferenceActivity::OnOptionsItemSelected(item, result);
}

void CSipSettings::Log(
    /* [in] */ const String& msg)
{
    Logger::D(SipUtil::TAG, PREFIX + msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos