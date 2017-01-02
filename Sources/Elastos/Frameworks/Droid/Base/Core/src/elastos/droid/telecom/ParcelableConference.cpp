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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/ParcelableConference.h"
#include "elastos/droid/telecom/Connection.h"
#include "elastos/droid/telecom/CPhoneCapabilities.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
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
    AutoPtr<IPhoneCapabilities> pc;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&pc);
    String str;
    pc->ToString(mCapabilities, &str);
    sb.Append(str);
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
} // namespace Droid
} // namespace Elastos