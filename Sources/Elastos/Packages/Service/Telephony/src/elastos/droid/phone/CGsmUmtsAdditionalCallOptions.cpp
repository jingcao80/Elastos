
#include "elastos/droid/phone/CGsmUmtsAdditionalCallOptions.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const String CGsmUmtsAdditionalCallOptions::LOG_TAG("GsmUmtsAdditionalCallOptions");

const String CGsmUmtsAdditionalCallOptions::BUTTON_CLIR_KEY("button_clir_key");
const String CGsmUmtsAdditionalCallOptions::BUTTON_CW_KEY("button_cw_key");

CAR_INTERFACE_IMPL(CGsmUmtsAdditionalCallOptions, TimeConsumingPreferenceActivity, IGsmUmtsAdditionalCallOptions)

CAR_OBJECT_IMPL(CGsmUmtsAdditionalCallOptions)

CGsmUmtsAdditionalCallOptions::CGsmUmtsAdditionalCallOptions()
    : DBG(IPhoneGlobals::DBG_LEVEL >= 2)
    , mInitIndex(0)
{
    CArrayList::New((ArrayList**)&mPreferences);
}

ECode CGsmUmtsAdditionalCallOptions::constructor()
{
    return TimeConsumingPreferenceActivity::constructor();
}

ECode CGsmUmtsAdditionalCallOptions::OnCreate(
    /* [in] */ IBundle* icicle)
{
    TimeConsumingPreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(R.xml.gsm_umts_additional_options);

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<IPreference> preference;
    prefSet->FindPreference(BUTTON_CLIR_KEY, &preference);
    mCLIRButton = ICLIRListPreference::Probe(preference);

    AutoPtr<IPreference> preference2;
    prefSet->FindPreference(BUTTON_CW_KEY, (IPreference**)&preference2);
    mCWButton = ICallWaitingCheckBoxPreference::Probe(preference2);

    mPreferences->Add(mCLIRButton);
    mPreferences->Add(mCWButton);

    if (icicle == NULL) {
        if (DBG) LogGER::D(LOG_TAG, "start to init ");
        mCLIRButton->Init(this, FALSE);
    }
    else {
        if (DBG) Logger::D(LOG_TAG, "restore stored states");
        mPreferences->GetSize(&mInitIndex);
        mCLIRButton->Init(this, TRUE);
        mCWButton->Init(this, TRUE);

        String key;
        mCLIRButton->GetKey(&key);
        AutoPtr<ArrayOf<Int32> > clirArray;
        icicle->GetIntArray(key, (ArrayOf<Int32>**)&clirArray);
        if (clirArray != NULL) {
            if (DBG) {
                StringBuileder sb;
                sb += "onCreate:  clirArray[0]=";
                sb += (*clirArray)[0];
                sb += ", clirArray[1]=";
                sb += (*clirArray)[1];
                Logger::D(LOG_TAG, sb.ToString());
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

    if (mCLIRButton->mClirArray != NULL) {
        String key;
        mCLIRButton->GetKey(&key);
        outState->PutIntArray(key, mCLIRButton->mClirArray);
    }
    return NOERROR;
}

ECode CGsmUmtsAdditionalCallOptions::OnFinished(
    /* [in] */ IPreference* preference,
    /* [in] */ Boolean reading)
{
    Int32 size;
    mPreferences->GetSize(&size);
    if (mInitIndex < size - 1 && !IsFinishing()) {
        mInitIndex++;

        AutoPtr<IInterface> obj;
        mPreferences->Get(mInitIndex, (IInterface**)&obj);
        AutoPtr<IPreference> pref = IPreference::Probe(obj);
        if (ICallWaitingCheckBoxPreference::Probe(pref) != NULL) {
            AutoPtr<ICallWaitingCheckBoxPreference> p = ICallWaitingCheckBoxPreference::Probe(pref);
            p->Init(this, FALSE);
        }
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
    if (itemId == android.R.id.home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
        CallFeaturesSetting::GoUpToTopLevelSetting(this);
        *result = TRUE;
        return NOERROR;
    }
    return TimeConsumingPreferenceActivity::OnOptionsItemSelected(item, result);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos