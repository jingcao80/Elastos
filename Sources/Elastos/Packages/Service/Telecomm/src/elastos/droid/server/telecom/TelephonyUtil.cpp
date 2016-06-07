
#include "elastos/droid/server/telecom/TelephonyUtil.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telecomm.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Telecomm::Telecom::CPhoneAccountBuilder;
using Elastos::Droid::Telecomm::Telecom::CPhoneAccountHandle;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountBuilder;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String TAG("TelephonyUtil");
const String TelephonyUtil::TELEPHONY_PACKAGE_NAME("com.android.phone");
const String TelephonyUtil::PSTN_CALL_SERVICE_CLASS_NAME("com.android.services.telephony.TelephonyConnectionService");
AutoPtr<IPhoneAccountHandle> TelephonyUtil::DEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE = InitDEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE();

TelephonyUtil::TelephonyUtil()
{}

ECode TelephonyUtil::GetDefaultEmergencyPhoneAccount(
    /* [out] */ IPhoneAccount** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhoneAccountBuilder> builder;
    CPhoneAccountBuilder::New(DEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE, StringUtils::ParseCharSequence(String("E")), (IPhoneAccountBuilder**)&builder);
    builder->SetCapabilities(IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION |
            IPhoneAccount::CAPABILITY_CALL_PROVIDER |
            IPhoneAccount::CAPABILITY_PLACE_EMERGENCY_CALLS);
    builder->Build(result);
    return NOERROR;
}

ECode TelephonyUtil::IsPstnComponentName(
    /* [in] */ IComponentName* componentName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IComponentName> pstnComponentName;
    CComponentName::New(
            TELEPHONY_PACKAGE_NAME, PSTN_CALL_SERVICE_CLASS_NAME, (IComponentName**)&pstnComponentName);
    return IObject::Probe(pstnComponentName)->Equals(componentName, result);
}

ECode TelephonyUtil::ShouldProcessAsEmergency(
    /* [in] */ IContext* context,
    /* [in] */ IUri* handle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    String schemeSpecificPart;
    handle->GetSchemeSpecificPart(&schemeSpecificPart);
    Boolean isPotentialLocalEmergencyNumber;
    phoneNumberUtilsHelper->IsPotentialLocalEmergencyNumber(context, schemeSpecificPart, &isPotentialLocalEmergencyNumber);
    *result = handle != NULL && isPotentialLocalEmergencyNumber;
    return NOERROR;
}

AutoPtr<IPhoneAccountHandle> TelephonyUtil::InitDEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE()
{
    AutoPtr<IPhoneAccountHandle> rev;
    AutoPtr<IComponentName> componentName;
    CComponentName::New(TELEPHONY_PACKAGE_NAME, PSTN_CALL_SERVICE_CLASS_NAME, (IComponentName**)&componentName);
    CPhoneAccountHandle::New(componentName , String("E"), (IPhoneAccountHandle**)&rev);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
