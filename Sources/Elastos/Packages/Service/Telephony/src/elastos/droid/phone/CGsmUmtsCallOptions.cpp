
#include "elastos/droid/phone/CGsmUmtsCallOptions.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const String CGsmUmtsCallOptions::TAG("GsmUmtsCallOptions");

CAR_INTERFACE_IMPL(CGsmUmtsCallOptions, PreferenceActivity, IGsmUmtsCallOptions)

CAR_OBJECT_IMPL(CGsmUmtsCallOptions)

CGsmUmtsCallOptions::CGsmUmtsCallOptions()
    : DBG(PhoneGlobals::DBG_LEVEL >= 2);
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

    AddPreferencesFromResource(R.xml.gsm_umts_call_options);

    AutoPtr<IPhone> phone;
    PhoneGlobals::GetPhone((IPhone**)&phone);
    Int32 type;
    phone->GetPhoneType(&type);
    if (type != IPhoneConstants::PHONE_TYPE_GSM) {
        //disable the entire screen
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        screen->SetEnabled(FALSE);
    }
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos