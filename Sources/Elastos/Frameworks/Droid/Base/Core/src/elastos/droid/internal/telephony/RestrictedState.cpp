
#include "elastos/droid/internal/telephony/RestrictedState.h"

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

    String res("Restricted State CS: ");
    res += csString;
    res += " PS:";
    res += mPsRestricted;
    *str = res;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

