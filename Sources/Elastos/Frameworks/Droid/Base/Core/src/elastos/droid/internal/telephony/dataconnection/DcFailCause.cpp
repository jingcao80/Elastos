
#include "elastos/droid/internal/telephony/dataconnection/DcFailCause.h"
#include "elastos/droid/R.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/content/res/CResources.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/droid/R.h>

using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::R;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DcFailCause, Object, IDcFailCause)

AutoPtr<IHashMap> DcFailCause::sErrorCodeToFailCauseMap = InitErrorCodeToFailCauseMap();

DcFailCause::DcFailCause()
{
    CResources::GetSystem()->GetBoolean(
            R::bool_::config_restart_radio_on_pdp_fail_regular_deactivation, &mRestartRadioOnRegularDeactivation);
}

ECode DcFailCause::constructor(
    /* [in] */ Int32 errorCode)
{
    mErrorCode = errorCode;
    return NOERROR;
}

ECode DcFailCause::GetErrorCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mErrorCode;
    return NOERROR;
}

ECode DcFailCause::IsRestartRadioFail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mErrorCode == DcFailCause_REGULAR_DEACTIVATION && mRestartRadioOnRegularDeactivation);
    return NOERROR;
}

ECode DcFailCause::IsPermanentFail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    assert(0 && "PhoneFactory");
    // AutoPtr<IContext> context = PhoneFactory::GetContext();
    // if (mErrorCode == ACTIVATION_REJECT_GGSN) {
    //     AutoPtr<IResources> res;
    //     context->GetResources((IResources**)&res);
    //     return (res->GetBoolean(R::bool_::config_reject_ggsn_perm_failure, result));
    // } else if (mErrorCode == PROTOCOL_ERRORS) {
    //     AutoPtr<IResources> res;
    //     context->GetResources((IResources**)&res);
    //     return (res->GetBoolean(R::bool_::config_protocol_errors_perm_failure, result));
    // } else {
    //     *result = (mErrorCode == OPERATOR_BARRED) || (mErrorCode == MISSING_UNKNOWN_APN) ||
    //             (mErrorCode == UNKNOWN_PDP_ADDRESS_TYPE) || (mErrorCode == USER_AUTHENTICATION) ||
    //             (mErrorCode == SERVICE_OPTION_NOT_SUPPORTED) ||
    //             (mErrorCode == SERVICE_OPTION_NOT_SUBSCRIBED) || (mErrorCode == NSAPI_IN_USE) ||
    //             (mErrorCode == ONLY_IPV4_ALLOWED) || (mErrorCode == ONLY_IPV6_ALLOWED) ||
    //             (mErrorCode == RADIO_POWER_OFF) || (mErrorCode == TETHERED_CALL_ACTIVE) ||
    //             (mErrorCode == RADIO_NOT_AVAILABLE) || (mErrorCode == UNACCEPTABLE_NETWORK_PARAMETER);
    //     return NOERROR;
    // }
    return NOERROR;
}

ECode DcFailCause::IsEventLoggable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mErrorCode == DcFailCause_OPERATOR_BARRED) || (mErrorCode == DcFailCause_INSUFFICIENT_RESOURCES) ||
            (mErrorCode == DcFailCause_UNKNOWN_PDP_ADDRESS_TYPE) || (mErrorCode == DcFailCause_USER_AUTHENTICATION) ||
            (mErrorCode == DcFailCause_ACTIVATION_REJECT_GGSN) || (mErrorCode == DcFailCause_ACTIVATION_REJECT_UNSPECIFIED) ||
            (mErrorCode == DcFailCause_SERVICE_OPTION_NOT_SUBSCRIBED) ||
            (mErrorCode == DcFailCause_SERVICE_OPTION_NOT_SUPPORTED) ||
            (mErrorCode == DcFailCause_SERVICE_OPTION_OUT_OF_ORDER) || (mErrorCode == DcFailCause_NSAPI_IN_USE) ||
            (mErrorCode == DcFailCause_ONLY_IPV4_ALLOWED) || (mErrorCode == DcFailCause_ONLY_IPV6_ALLOWED) ||
            (mErrorCode == DcFailCause_PROTOCOL_ERRORS) || (mErrorCode == DcFailCause_SIGNAL_LOST) ||
            (mErrorCode == DcFailCause_RADIO_POWER_OFF) || (mErrorCode == DcFailCause_TETHERED_CALL_ACTIVE) ||
            (mErrorCode == DcFailCause_UNACCEPTABLE_NETWORK_PARAMETER);
    return NOERROR;
}

ECode DcFailCause::FromInt32(
    /* [in] */ Int32 errorCode,
    /* [out] */ IDcFailCause** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInteger32> i32;
    CInteger32::New(errorCode, (IInteger32**)&i32);
    AutoPtr<IInterface> obj;
    sErrorCodeToFailCauseMap->Get(i32, (IInterface**)&obj);
    AutoPtr<IDcFailCause> fc = IDcFailCause::Probe(obj);
    if (fc == NULL) {
        FromInt32(DcFailCause_UNKNOWN, (IDcFailCause**)&fc);
    }
    *result = fc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IHashMap> DcFailCause::InitErrorCodeToFailCauseMap()
{
    AutoPtr<IHashMap> rev;
    CHashMap::New((IHashMap**)&rev);
    Int32 values[] = {
            DcFailCause_NONE,
            DcFailCause_OPERATOR_BARRED,
            DcFailCause_INSUFFICIENT_RESOURCES,
            DcFailCause_MISSING_UNKNOWN_APN,
            DcFailCause_UNKNOWN_PDP_ADDRESS_TYPE,
            DcFailCause_USER_AUTHENTICATION,
            DcFailCause_ACTIVATION_REJECT_GGSN,
            DcFailCause_ACTIVATION_REJECT_UNSPECIFIED,
            DcFailCause_SERVICE_OPTION_NOT_SUPPORTED,
            DcFailCause_SERVICE_OPTION_NOT_SUBSCRIBED,
            DcFailCause_SERVICE_OPTION_OUT_OF_ORDER,
            DcFailCause_NSAPI_IN_USE,
            DcFailCause_REGULAR_DEACTIVATION,
            DcFailCause_ONLY_IPV4_ALLOWED,
            DcFailCause_ONLY_IPV6_ALLOWED,
            DcFailCause_ONLY_SINGLE_BEARER_ALLOWED,
            DcFailCause_PROTOCOL_ERRORS,

            // Local errors generated by Vendor RIL
            // specified in ril.h
            DcFailCause_REGISTRATION_FAIL,
            DcFailCause_GPRS_REGISTRATION_FAIL,
            DcFailCause_SIGNAL_LOST,
            DcFailCause_PREF_RADIO_TECH_CHANGED,
            DcFailCause_RADIO_POWER_OFF,
            DcFailCause_TETHERED_CALL_ACTIVE,
            DcFailCause_ERROR_UNSPECIFIED,

            // Errors generated by the Framework
            // specified here
            DcFailCause_UNKNOWN,
            DcFailCause_RADIO_NOT_AVAILABLE,
            DcFailCause_UNACCEPTABLE_NETWORK_PARAMETER,
            DcFailCause_CONNECTION_TO_DATACONNECTIONAC_BROKEN,
            DcFailCause_LOST_CONNECTION,
            DcFailCause_RESET_BY_FRAMEWORK};

    for (Int32 i = 0; i < sizeof(values) / sizeof(Int32); ++i) {
        AutoPtr<IInteger32> i32;
        CInteger32::New(values[i], (IInteger32**)&i32);
        rev->Put(i32, i32);
    }
    return rev;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
