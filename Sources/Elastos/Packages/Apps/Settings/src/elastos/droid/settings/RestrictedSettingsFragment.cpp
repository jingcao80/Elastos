
#include "elastos/droid/settings/RestrictedSettingsFragment.h"
#include "R.h"

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Os::CPersistableBundle;
using Elastos::Droid::Os::IPersistableBundle;

namespace Elastos {
namespace Droid {
namespace Settings {

const String RestrictedSettingsFragment::RESTRICT_IF_OVERRIDABLE("restrict_if_overridable");
const Int32 RestrictedSettingsFragment::REQUEST_PIN_CHALLENGE = 12309;
const String RestrictedSettingsFragment::KEY_CHALLENGE_SUCCEEDED("chsc");
const String RestrictedSettingsFragment::KEY_CHALLENGE_REQUESTED("chrq");

//===============================================================================
//                  RestrictedSettingsFragment::MyBroadcastReceiver
//===============================================================================

RestrictedSettingsFragment::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ RestrictedSettingsFragment* host)
    : mHost(host)
{}

RestrictedSettingsFragment::MyBroadcastReceiver::~MyBroadcastReceiver()
{}

ECode RestrictedSettingsFragment::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (!mHost->mChallengeRequested) {
        mHost->mChallengeSucceeded = FALSE;
        mHost->mChallengeRequested = FALSE;
    }
    return NOERROR;
}

//===============================================================================
//                  RestrictedSettingsFragment
//===============================================================================

RestrictedSettingsFragment::RestrictedSettingsFragment(
    /* [in] */ const String& restrictionKey)
    : mChallengeSucceeded(FALSE)
    , mChallengeRequested(FALSE)
{
    mRestrictionKey = restrictionKey;
    mScreenOffReceiver = new MyBroadcastReceiver(this);
}

RestrictedSettingsFragment::~RestrictedSettingsFragment()
{}

ECode RestrictedSettingsFragment::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AutoPtr<IInterface> obj = GetSystemService(IContext::RESTRICTIONS_SERVICE);
    mRestrictionsManager = IRestrictionsManager::Probe(obj);
    obj = GetSystemService(IContext::USER_SERVICE);
    mUserManager = IUserManager::Probe(obj);

    if (icicle != NULL) {
        icicle->GetBoolean(KEY_CHALLENGE_SUCCEEDED, FALSE, &mChallengeSucceeded);
        icicle->GetBoolean(KEY_CHALLENGE_REQUESTED, FALSE, &mChallengeRequested);
    }

    AutoPtr<IIntentFilter> offFilter;
    CIntentFilter::New(IIntent::ACTION_SCREEN_OFF, (IIntentFilter**)&offFilter);
    offFilter->AddAction(IIntent::ACTION_USER_PRESENT);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> resIntent;
    IContext::Probe(activity)->RegisterReceiver(mScreenOffReceiver, offFilter, (IIntent**)&resIntent);
    return NOERROR;
}

ECode RestrictedSettingsFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    SettingsPreferenceFragment::OnSaveInstanceState(outState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    Boolean res;
    if (activity->IsChangingConfigurations(&res), res) {
        outState->PutBoolean(KEY_CHALLENGE_REQUESTED, mChallengeRequested);
        outState->PutBoolean(KEY_CHALLENGE_SUCCEEDED, mChallengeSucceeded);
    }
    return NOERROR;
}

ECode RestrictedSettingsFragment::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    if (ShouldBeProviderProtected(mRestrictionKey)) {
        EnsurePin();
    }
    return NOERROR;
}

ECode RestrictedSettingsFragment::OnDestroy()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver(mScreenOffReceiver);
    return SettingsPreferenceFragment::OnDestroy();
}

ECode RestrictedSettingsFragment::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (requestCode == REQUEST_PIN_CHALLENGE) {
        if (resultCode == IActivity::RESULT_OK) {
            mChallengeSucceeded = TRUE;
            mChallengeRequested = FALSE;
        }
        else {
            mChallengeSucceeded = FALSE;
        }
        return NOERROR;
    }

    return SettingsPreferenceFragment::OnActivityResult(requestCode, resultCode, data);
}

void RestrictedSettingsFragment::EnsurePin()
{
    Boolean res;
    if (!mChallengeSucceeded && !mChallengeRequested
            && (mRestrictionsManager->HasRestrictionsProvider(&res), res)) {
        AutoPtr<IIntent> intent;
        mRestrictionsManager->CreateLocalApprovalIntent((IIntent**)&intent);
        if (intent != NULL) {
            mChallengeRequested = TRUE;
            mChallengeSucceeded = FALSE;
            AutoPtr<IPersistableBundle> request;
            CPersistableBundle::New((IPersistableBundle**)&request);
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            String str;
            resources->GetString(R::string::restr_pin_enter_admin_pin, &str);
            IBaseBundle::Probe(request)->PutString(IRestrictionsManager::REQUEST_KEY_MESSAGE, str);
            intent->PutExtra(IRestrictionsManager::EXTRA_REQUEST_BUNDLE, IParcelable::Probe(request));
            StartActivityForResult(intent, REQUEST_PIN_CHALLENGE);
        }
    }
}

Boolean RestrictedSettingsFragment::IsRestrictedAndNotProviderProtected()
{
    if (mRestrictionKey == NULL || RESTRICT_IF_OVERRIDABLE.Equals(mRestrictionKey)) {
        return FALSE;
    }
    Boolean res1, res2;
    return (mUserManager->HasUserRestriction(mRestrictionKey, &res1), res1)
            && (mRestrictionsManager->HasRestrictionsProvider(&res2), !res2);
}

Boolean RestrictedSettingsFragment::HasChallengeSucceeded()
{
    return (mChallengeRequested && mChallengeSucceeded) || !mChallengeRequested;
}

Boolean RestrictedSettingsFragment::ShouldBeProviderProtected(
    /* [in] */ const String& restrictionKey)
{
    if (restrictionKey == NULL) {
        return FALSE;
    }

    Boolean res1, res2;

    Boolean restricted = RESTRICT_IF_OVERRIDABLE.Equals(restrictionKey)
            || (mUserManager->HasUserRestriction(mRestrictionKey, &res1), res1);
    return restricted && (mRestrictionsManager->HasRestrictionsProvider(&res2), res2);
}

Boolean RestrictedSettingsFragment::IsUiRestricted()
{
    return IsRestrictedAndNotProviderProtected() || !HasChallengeSucceeded();
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
