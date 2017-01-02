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

#include "elastos/droid/internal/telephony/RestrictedState.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  RestrictedState::
//==============================================================
CAR_INTERFACE_IMPL(RestrictedState, Object, IRestrictedState)

ECode RestrictedState::constructor()
{
    SetPsRestricted(FALSE);
    SetCsNormalRestricted(FALSE);
    SetCsEmergencyRestricted(FALSE);
    return NOERROR;
}

ECode RestrictedState::SetCsEmergencyRestricted(
    /* [in] */ Boolean csEmergencyRestricted)
{
    mCsEmergencyRestricted = csEmergencyRestricted;
    return NOERROR;
}

ECode RestrictedState::IsCsEmergencyRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCsEmergencyRestricted;
    return NOERROR;
}

ECode RestrictedState::SetCsNormalRestricted(
    /* [in] */ Boolean csNormalRestricted)
{
    mCsNormalRestricted = csNormalRestricted;
    return NOERROR;
}

ECode RestrictedState::IsCsNormalRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCsNormalRestricted;
    return NOERROR;
}

ECode RestrictedState::SetPsRestricted(
    /* [in] */ Boolean psRestricted)
{
    mPsRestricted = psRestricted;
    return NOERROR;
}

ECode RestrictedState::IsPsRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPsRestricted;
    return NOERROR;
}

ECode RestrictedState::IsCsRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCsNormalRestricted && mCsEmergencyRestricted;
    return NOERROR;
}

ECode RestrictedState::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<RestrictedState> s;

    // try {
    s = (RestrictedState*) IRestrictedState::Probe(o);
    // } Catch (ClassCastException ex) {
    //     return FALSE;
    // }

    if (o == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    *result = mPsRestricted == s->mPsRestricted
        && mCsNormalRestricted == s->mCsNormalRestricted
        && mCsEmergencyRestricted == s->mCsEmergencyRestricted;
    return NOERROR;
}

ECode RestrictedState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String csString("none");

    if (mCsEmergencyRestricted && mCsNormalRestricted) {
        csString = "all";
    }
    else if (mCsEmergencyRestricted && !mCsNormalRestricted) {
        csString = "emergency";
    }
    else if (!mCsEmergencyRestricted && mCsNormalRestricted) {
        csString = "normal call";
    }

    StringBuilder sb("Restricted State CS: ");
    sb += csString; sb += " PS:"; sb += mPsRestricted;
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

