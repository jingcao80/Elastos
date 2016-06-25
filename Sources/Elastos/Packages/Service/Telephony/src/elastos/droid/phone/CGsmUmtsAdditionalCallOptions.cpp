
#include "elastos/droid/phone/CGsmUmtsAdditionalCallOptions.h"
#include "elastos/droid/phone/CCallFeaturesSetting.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Preference.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

const String CGsmUmtsAdditionalCallOptions::TAG("GsmUmtsAdditionalCallOptions");

const String CGsmUmtsAdditionalCallOptions::BUTTON_CLIR_KEY("button_clir_key");
const String CGsmUmtsAdditionalCallOptions::BUTTON_CW_KEY("button_cw_key");

CAR_OBJECT_IMPL(CGsmUmtsAdditionalCallOptions)

CGsmUmtsAdditionalCallOptions::CGsmUmtsAdditionalCallOptions()
    : DBG(IPhoneGlobals::DBG_LEVEL >= 2)
    , mInitIndex(0)
{
    CArrayList::New((IArrayList**)&mPreferences);
}

ECode CGsmUmtsAdditionalCallOptions::constructor()
{
    return TimeConsumingPreferenceActivity::constructor();
}

ECode CGsmUmtsAdditionalCallOptions::OnCreate(
    /* [in] */ IBundle* icicle)
{
    TimeConsumingPreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(Elastos::Droid::Server::Telephony::R::xml::gsm_umts_additional_options);

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<IPreference> preference;
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(BUTTON_CLIR_KEY);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar, (IPreference**)&preference);
    mCLIRButton = ICLIRListPreference::Probe(preference);

    AutoPtr<IPreference> preference2;
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(BUTTON_CW_KEY);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar2, (IPreference**)&preference2);
    assert(0 && "android:enable = false is setted in *.xml about class CallWaitingCheckBoxPreference");
    //mCWButton = ICallWaitingCheckBoxPreference::Probe(preference2);

    mPreferences->Add(mCLIRButton);
    assert(0 && "android:enable = false is setted in *.xml about class CallWaitingCheckBoxPreference");
    //mPreferences->Add(mCWButton);

    if (icicle == NULL) {
        if (DBG) Logger::D(TAG, "start to init ");
        mCLIRButton->Init(this, FALSE);
    }
    else {
        if (DBG) Logger::D(TAG, "restore stored states");
        mPreferences->GetSize(&mInitIndex);
        mCLIRButton->Init(this, TRUE);
        assert(0 && "android:enable = false is setted in *.xml about class CallWaitingCheckBoxPreference");
        //mCWButton->Init(this, TRUE);

        String key;
        IPreference::Probe(mCLIRButton)->GetKey(&key);
        AutoPtr<ArrayOf<Int32> > clirArray;
        icicle->GetInt32Array(key, (ArrayOf<Int32>**)&clirArray);
        if (clirArray != NULL) {
            if (DBG) {
                StringBuilder sb;
                sb += "onCreate:  clirArray[0]=";
                sb += (*clirArray)[0];
                sb += ", clirArray[1]=";
                sb += (*clirArray)[1];
                Logger::D(TAG, sb.ToString());
            }
            mCLIRButton->HandleGetCLIRResult(clirArray);
        }
        else {
            mCLIRButton->Init(this, FALSE);
        }
    }

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.R.id.home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
    return NOERROR;
}

ECode CGsmUmtsAdditionalCallOptions::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    TimeConsumingPreferenceActivity::OnSaveInstanceState(outState);

    assert(0);
    // AutoPtr<CCLIRListPreference> preference = (CCLIRListPreference*)mCLIRButton;
    // if (preference->mClirArray != NULL) {
    //     String key;
    //     IPreference::Probe(mCLIRButton)->GetKey(&key);
    //     outState->PutInt32Array(key, preference->mClirArray);
    // }
    return NOERROR;
}

ECode CGsmUmtsAdditionalCallOptions::OnFinished(
    /* [in] */ IPreference* preference,
    /* [in] */ Boolean reading)
{
    Int32 size;
    mPreferences->GetSize(&size);
    Boolean res;
    if (mInitIndex < size - 1 && (IsFinishing(&res), !res)) {
        mInitIndex++;

        AutoPtr<IInterface> obj;
        mPreferences->Get(mInitIndex, (IInterface**)&obj);
        AutoPtr<IPreference> pref = IPreference::Probe(obj);
        assert(0 && "android:enable = false is setted in *.xml about class CallWaitingCheckBoxPreference");
        // if (ICallWaitingCheckBoxPreference::Probe(pref) != NULL) {
        //     AutoPtr<ICallWaitingCheckBoxPreference> p = ICallWaitingCheckBoxPreference::Probe(pref);
        //     p->Init(this, FALSE);
        // }
    }
    return TimeConsumingPreferenceActivity::OnFinished(preference, reading);
}

ECode CGsmUmtsAdditionalCallOptions::OnOptionsItemSelected(
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