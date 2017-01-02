//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/teleservice/phone/CGsmUmtsCallOptions.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Preference.h"
#include "R.h"

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;

namespace Elastos {
namespace Droid {
namespace TeleService {
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

    AddPreferencesFromResource(Elastos::Droid::TeleService::R::xml::gsm_umts_call_options);

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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos