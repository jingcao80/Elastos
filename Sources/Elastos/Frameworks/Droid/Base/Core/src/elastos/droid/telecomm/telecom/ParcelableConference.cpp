
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/ParcelableConference.h"
#include "elastos/droid/telecomm/telecom/Connection.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// ParcelableConference::
//===============================================================
CAR_INTERFACE_IMPL_2(ParcelableConference, Object, IParcelableConference, IParcelable)

ECode ParcelableConference::constructor()
{
    return NOERROR;
}

ECode ParcelableConference::constructor(
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [in] */ Int32 state,
    /* [in] */ Int32 capabilities,
    /* [in] */ IList* connectionIds)
{
    mPhoneAccount = phoneAccount;
    mState = state;
    mCapabilities = capabilities;
    mConnectionIds = connectionIds;
    return NOERROR;
}

ECode ParcelableConference::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuffer sb;
    sb.Append("account: ");
    sb.Append(mPhoneAccount);
    sb.Append(", state: ");
    String ss;
    Connection::StateToString(mState, &ss);
    sb.Append(ss);
    sb.Append(", capabilities: ");
    // sb.Append(PhoneCapabilities.toString(mCapabilities));
    sb.Append(", children: ");
    sb.Append(mConnectionIds);
    *result = sb.ToString();
    return NOERROR;
}

ECode ParcelableConference::GetPhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneAccount;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConference::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode ParcelableConference::GetCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCapabilities;
    return NOERROR;
}

ECode ParcelableConference::GetConnectionIds(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectionIds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConference::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode ParcelableConference::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    IParcelable::Probe(mPhoneAccount)->WriteToParcel(destination);
    destination->WriteInt32(mState);
    destination->WriteInt32(mCapabilities);
    IParcelable::Probe(mConnectionIds)->WriteToParcel(destination);
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos