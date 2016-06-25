
#include "elastos/droid/phone/CGsmUmtsCallOptions.h"
#include "elastos/droid/phone/PhoneGlobals.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Preference.h"
#include "R.h"

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;

namespace Elastos {
namespace Droid {
namespace Phone {

const String CGsmUmtsCallOptions::TAG("GsmUmtsCallOptions");

CAR_OBJECT_IMPL(CGsmUmtsCallOptions)

CGsmUmtsCallOptions::CGsmUmtsCallOptions()
    : DBG(IPhoneGlobals::DBG_LEVEL >= 2)
{
}

ECode CGsmUmtsCallOptions::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CGsmUmtsCallOptions::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(Elastos::Droid::Server::Telephony::R::xml::gsm_umts_call_options);

    AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
    Int32 type;
    phone->GetPhoneType(&type);
    if (type != IPhoneConstants::PHONE_TYPE_GSM) {
        //disable the entire screen
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreference::Probe(screen)->SetEnabled(FALSE);
    }
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos