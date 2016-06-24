
#include "elastos/droid/phone/CCdmaCallOptions.h"
#include "R.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Preference.h"

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Server::Telephony::R;

namespace Elastos {
namespace Droid {
namespace Phone {

const String CCdmaCallOptions::TAG("CdmaCallOptions");
const String CCdmaCallOptions::BUTTON_VP_KEY("button_voice_privacy_key");

CAR_OBJECT_IMPL(CCdmaCallOptions)

CCdmaCallOptions::CCdmaCallOptions()
    : DBG(IPhoneGlobals::DBG_LEVEL >= 2)
{
}

CCdmaCallOptions::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CCdmaCallOptions::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(R::xml::cdma_call_privacy);

    AutoPtr<IPreference> preference;
    FindPreference(BUTTON_VP_KEY, (IPreference**)&preference);
    mButtonVoicePrivacy = ICheckBoxPreference::Probe(preference);

    AutoPtr<IPhone> phone;
    assert(0 && "TODO need PhoneGlobals");
    // phone = PhoneGlobals::GetPhone();
    Int32 type;
    phone->GetPhoneType(&type);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(R::bool_::config_voice_privacy_disable, &res);
    if (type != IPhoneConstants::PHONE_TYPE_CDMA || res) {
        //disable the entire screen
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreference::Probe(screen)->SetEnabled(FALSE);
    }
    return NOERROR;
}

ECode CCdmaCallOptions::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String key;
    preference->GetKey(&key);
    if (key.Equals(BUTTON_VP_KEY)) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos