
#include "elastos/droid/phone/CGsmUmtsCallForwardOptions.h"
#include "elastos/droid/phone/CCallFeaturesSetting.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::IO::ICloseable;;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

const String CGsmUmtsCallForwardOptions::TAG("GsmUmtsCallForwardOptions");

static AutoPtr<ArrayOf<String> > initNUM_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, IContactsContractCommonDataKindsPhone::NUMBER);
    return array;
}

const AutoPtr<ArrayOf<String> > CGsmUmtsCallForwardOptions::NUM_PROJECTION = initNUM_PROJECTION();

const String CGsmUmtsCallForwardOptions::BUTTON_CFU_KEY("button_cfu_key");
const String CGsmUmtsCallForwardOptions::BUTTON_CFB_KEY("button_cfb_key");
const String CGsmUmtsCallForwardOptions::BUTTON_CFNRY_KEY("button_cfnry_key");
const String CGsmUmtsCallForwardOptions::BUTTON_CFNRC_KEY("button_cfnrc_key");

const String CGsmUmtsCallForwardOptions::KEY_TOGGLE("toggle");
const String CGsmUmtsCallForwardOptions::KEY_STATUS("status");
const String CGsmUmtsCallForwardOptions::KEY_NUMBER("number");

CAR_OBJECT_IMPL(CGsmUmtsCallForwardOptions)

CGsmUmtsCallForwardOptions::CGsmUmtsCallForwardOptions()
    : DBG(IPhoneGlobals::DBG_LEVEL >= 2)
    , mInitIndex(0)
    , mFirstResume(FALSE)
{
    CArrayList::New((IArrayList**)&mPreferences);
}

ECode CGsmUmtsCallForwardOptions::constructor()
{
    return TimeConsumingPreferenceActivity::constructor();
}

ECode CGsmUmtsCallForwardOptions::OnCreate(
    /* [in] */ IBundle* icicle)
{
    TimeConsumingPreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(Elastos::Droid::Server::Telephony::R::xml::callforward_options);

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    assert(0 && "android:enable = false is setted in *.xml about class CallForwardEditPreference");
    // AutoPtr<IPreference> preference;
    // prefSet->FindPreference(BUTTON_CFU_KEY, (IPreference**)&preference);
    // mButtonCFU = ICallForwardEditPreference::Probe(preference);

    // AutoPtr<IPreference> preference2;
    // prefSet->FindPreference(BUTTON_CFB_KEY, (IPreference**)&preference2);
    // mButtonCFB = ICallForwardEditPreference::Probe(preference2);

    // AutoPtr<IPreference> preference3;
    // prefSet->FindPreference(BUTTON_CFNRY_KEY, (IPreference**)&preference3);
    // mButtonCFNRy = ICallForwardEditPreference::Probe(preference3);

    // AutoPtr<IPreference> preference3;
    // prefSet->FindPreference(BUTTON_CFNRC_KEY, (IPreference**)&preference3);
    // mButtonCFNRc = ICallForwardEditPreference::Probe(preference3);

    // mButtonCFU->SetParentActivity(this, mButtonCFU.reason);
    // mButtonCFB->SetParentActivity(this, mButtonCFB.reason);
    // mButtonCFNRy->SetParentActivity(this, mButtonCFNRy.reason);
    // mButtonCFNRc->SetParentActivity(this, mButtonCFNRc.reason);

    // mPreferences->Add(mButtonCFU);
    // mPreferences->Add(mButtonCFB);
    // mPreferences->Add(mButtonCFNRy);
    // mPreferences->Add(mButtonCFNRc);

    // we wait to do the initialization until onResume so that the
    // TimeConsumingPreferenceActivity dialog can display as it
    // relies on onResume / onPause to maintain its foreground state.

    mFirstResume = TRUE;
    mIcicle = icicle;

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.R.id.home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
    return NOERROR;
}

ECode CGsmUmtsCallForwardOptions::OnResume()
{
    TimeConsumingPreferenceActivity::OnResume();

    assert(0 && "android:enable = false is setted in *.xml about class CallForwardEditPreference");
    // if (mFirstResume) {
    //     if (mIcicle == NULL) {
    //         if (DBG) Logger::D(TAG, "start to init ");
    //         AutoPtr<IInterface> obj;
    //         mPreferences->Get(mInitIndex, (IInterface**)&obj);
    //         AutoPtr<ICallForwardEditPreference> p = ICallForwardEditPreference::Probe(obj);
    //         p->Init(this, FALSE);
    //     }
    //     else {
    //         mPreferences->GetSize(&mInitIndex);

    //         for (Int32 i = 0; i < mInitIndex; i++) {
    //             AutoPtr<IInterface> obj;
    //             mPreferences->Get(i, (IInterface**)&obj);
    //             AutoPtr<ICallForwardEditPreference> pref = ICallForwardEditPreference::Probe(obj);

    //             String key;
    //             pref->GetKey(&key);
    //             AutoPtr<IParcelable> parcelable;
    //             mIcicle->GetParcelable(key, (IParcelable**)&parcelable);
    //             AutoPtr<IBundle> bundle = IBundle::Probe(parcelable);

    //             Boolean res;
    //             bundle->GetBoolean(KEY_TOGGLE, &res);
    //             pref->SetToggled(res);
    //             AutoPtr<ICallForwardInfo> cf;
    //             CCallForwardInfo::New((ICallForwardInfo**)&cf);

    //             String number;
    //             bundle->GetString(KEY_NUMBER, &number);
    //             cf->SetNumber(number);

    //             Int32 status;
    //             bundle->GetInt32(KEY_STATUS, &status);
    //             cf->SetStatus(status);
    //             pref->HandleCallForwardResult(cf);
    //             pref->Init(this, TRUE);
    //         }
    //     }
    //     mFirstResume = FALSE;
    //     mIcicle = NULL;
    // }
    return NOERROR;
}

ECode CGsmUmtsCallForwardOptions::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    TimeConsumingPreferenceActivity::OnSaveInstanceState(outState);

    assert(0 && "android:enable = false is setted in *.xml about class CallForwardEditPreference");
    // Int32 size;
    // mPreferences->GetSize(&size);
    // for (Int32 i = 0; i < mInitIndex; i++) {
    //     AutoPtr<IInterface> obj;
    //     mPreferences->Get(i, (IInterface**)&obj);
    //     AutoPtr<ICallForwardEditPreference> pref = ICallForwardEditPreference::Probe(obj);

    //     AutoPtr<IBundle> bundle;
    //     CBundle::New((IBundle**)&bundle);
    //     Boolean res;
    //     pref->IsToggled(&res);
    //     bundle->PutBoolean(KEY_TOGGLE, res);
    //     if (pref->mCallForwardInfo != NULL) {
    //         bundle->PutString(KEY_NUMBER, pref.callForwardInfo.number);
    //         bundle->PutInt(KEY_STATUS, pref.callForwardInfo.status);
    //     }
    //     String str;
    //     pref->GetKey(&str);
    //     outState->PutParcelable(str, bundle);
    // }
    return NOERROR;
}

ECode CGsmUmtsCallForwardOptions::OnFinished(
    /* [in] */ IPreference* preference,
    /* [in] */ Boolean reading)
{
    assert(0 && "android:enable = false is setted in *.xml about class CallForwardEditPreference");
    // Int32 size;
    // mPreferences->GetSize(&size);
    // if (mInitIndex < size - 1 && !IsFinishing()) {
    //     mInitIndex++;

    //     AutoPtr<IInterface> obj;
    //     mPreferences->Get(mInitIndex, (IInterface**)&obj);
    //     AutoPtr<ICallForwardEditPreference> pref = ICallForwardEditPreference::Probe(obj);

    //     pref->Init(this, FALSE);
    // }

    return TimeConsumingPreferenceActivity::OnFinished(preference, reading);
}

ECode CGsmUmtsCallForwardOptions::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (DBG) Logger::D(TAG, "onActivityResult: done");
    if (resultCode != RESULT_OK) {
        if (DBG) Logger::D(TAG, "onActivityResult: contact picker result not OK.");
        return NOERROR;
    }
    AutoPtr<ICursor> cursor;
    //try
    {
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<IUri> _data;
        data->GetData((IUri**)&_data);
        FAIL_GOTO(contentResolver->Query(_data, NUM_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&cursor), FINALLY)

        Boolean res;
        if ((cursor == NULL) || (cursor->MoveToFirst(&res), !res)) {
            if (DBG) Logger::D(TAG, "onActivityResult: bad contact data, no results found.");
            return NOERROR;
        }

        assert(0 && "android:enable = false is setted in *.xml about class CallForwardEditPreference");
        // switch (requestCode) {
        //     case ICommandsInterface::CF_REASON_UNCONDITIONAL:
        //     {
        //         String str;
        //         FAIL_GOTO(cursor->GetString(0, &str), FINALLY)
        //         mButtonCFU->OnPickActivityResult(str);
        //         break;
        //     }
        //     case ICommandsInterface::CF_REASON_BUSY:
        //     {
        //         String str;
        //         FAIL_GOTO(cursor->GetString(0, &str), FINALLY)
        //         mButtonCFB->OnPickActivityResult(str);
        //         break;
        //     }
        //     case ICommandsInterface::CF_REASON_NO_REPLY:
        //     {
        //         String str;
        //         FAIL_GOTO(cursor->GetString(0, &str), FINALLY)
        //         mButtonCFNRy->OnPickActivityResult(str);
        //         break;
        //     }
        //     case ICommandsInterface::CF_REASON_NOT_REACHABLE:
        //     {
        //         String str;
        //         FAIL_GOTO(cursor->GetString(0, &str), FINALLY)
        //         mButtonCFNRc->OnPickActivityResult(str);
        //         break;
        //     }
        //     default:
        //         // TODO: may need exception here.
        // }
    }
    //finally {
FINALLY:
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    //}
    return NOERROR;
}

ECode CGsmUmtsCallForwardOptions::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
        CCallFeaturesSetting::GoUpToTopLevelSetting(this);
        *result = TRUE;
        return NOERROR;
    }
    return TimeConsumingPreferenceActivity::OnOptionsItemSelected(item, result);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos