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

#include "elastos/droid/wifi/p2p/nsd/WifiP2pServiceRequest.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL_2(WifiP2pServiceRequest, Object, IWifiP2pServiceRequest, IParcelable)

WifiP2pServiceRequest::WifiP2pServiceRequest()
    : mProtocolType(0)
    , mLength(0)
    , mTransId(0)
{
}

ECode WifiP2pServiceRequest::constructor()
{
    return NOERROR;
}

ECode WifiP2pServiceRequest::constructor(
    /* [in] */ Int32 protocolType,
    /* [in] */ const String& query)
{
    FAIL_RETURN(ValidateQuery(query));

    mProtocolType = protocolType;
    mQuery = query;
    if (!query.IsNull()) {
        mLength = query.GetLength() / 2 + 2;
    }
    else {
        mLength = 2;
    }

    return NOERROR;
}

ECode WifiP2pServiceRequest::constructor(
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 length,
    /* [in] */ Int32 transId,
    /* [in] */ const String& query)
{
    mProtocolType = serviceType;
    mLength = length;
    mTransId = transId;
    mQuery = query;

    return NOERROR;
}

ECode WifiP2pServiceRequest::GetTransactionId(
    /* [out] */ Int32* transactionId)
{
    VALIDATE_NOT_NULL(transactionId);
    *transactionId = mTransId;
    return NOERROR;
}

ECode WifiP2pServiceRequest::SetTransactionId(
    /* [in] */ Int32 id)
{
    mTransId = id;
    return NOERROR;
}

ECode WifiP2pServiceRequest::GetSupplicantQuery(
    /* [out] */ String* supplicantQuery)
{
    VALIDATE_NOT_NULL(supplicantQuery);

    String sb;
    // length is retained as little endian format.
    assert(0);
    // TODO
    // sb.AppendFormat(Locale.US, "%02x", (mLength) & 0xff);
    // sb.AppendFormat(Locale.US, "%02x", (mLength >> 8) & 0xff);
    // sb.AppendFormat(Locale.US, "%02x", mProtocolType);
    // sb.AppendFormat(Locale.US, "%02x", mTransId);
    if (!mQuery.IsNull()) {
        sb += mQuery;
    }

    *supplicantQuery = sb;
    return NOERROR;
}

ECode WifiP2pServiceRequest::ValidateQuery(
    /* [in] */ const String& query)
{
    if (query.IsNull()) {
        return NOERROR;
    }

    Int32 UNSIGNED_SHORT_MAX = 0xffff;
    if (query.GetLength() % 2 == 1) {
        Slogger::E("WifiP2pServiceRequest", "query size is invalid. query=%s", query.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (query.GetLength() / 2 > UNSIGNED_SHORT_MAX) {
        Slogger::E("WifiP2pServiceRequest", "query size is too large. len=%d", query.GetLength());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // check whether query is hex string.
    String query1;// TODO = query.ToLowerCase(Locale.ROOT);
    Char32 c;
    AutoPtr<ArrayOf<Char32> > charArray = query1.GetChars();
    if (charArray) {
        for (Int32 i = 0; i < charArray->GetLength(); ++i) {
            c = (*charArray)[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))){
                Slogger::E("WifiP2pServiceRequest", "query should be hex string. query=%s", query.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

ECode WifiP2pServiceRequest::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (other == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }

    if (IWifiP2pServiceRequest::Probe(other) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    /*
     * Not compare transaction id.
     * Transaction id may be changed on each service discovery operation.
     */
    WifiP2pServiceRequest* psr = NULL;// TODO =
    // TODO reinterpret_cast<WifiP2pServiceRequest*>(other->Probe(EIID_WifiP2pServiceRequest));
    assert(psr != NULL);

    if (psr->mProtocolType != mProtocolType || psr->mLength != mLength) {
        *result = FALSE;
        return NOERROR;
    }

    if (psr->mQuery.IsNull() && mQuery.IsNull()){
        *result = TRUE;
        return NOERROR;
    }
    else if (!psr->mQuery.IsNull()){
        *result = psr->mQuery.Equals(mQuery);
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode WifiP2pServiceRequest::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    Int32 result = 17;
    result = 31 * result + mProtocolType;
    result = 31 * result + mLength;
    result = 31 * result + (mQuery.IsNull() ? 0 : mQuery.GetHashCode());

    *hashCode = result;
    return NOERROR;
}

ECode WifiP2pServiceRequest::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadInt32(&mProtocolType));
    FAIL_RETURN(source->ReadInt32(&mLength));
    FAIL_RETURN(source->ReadInt32(&mTransId));
    FAIL_RETURN(source->ReadString(&mQuery));

    return NOERROR;
}

ECode WifiP2pServiceRequest::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt32(mProtocolType));
    FAIL_RETURN(dest->WriteInt32(mLength));
    FAIL_RETURN(dest->WriteInt32(mTransId));
    FAIL_RETURN(dest->WriteString(mQuery));

    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
