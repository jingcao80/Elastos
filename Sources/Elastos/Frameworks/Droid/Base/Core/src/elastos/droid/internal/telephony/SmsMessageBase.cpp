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

#include "elastos/droid/internal/telephony/SmsMessageBase.h"
#include "elastos/droid/provider/Telephony.h"
#include <elastos/utility/Arrays.h>
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"

using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(SmsMessageBase::SubmitPduBase, Object, ISmsMessageBaseSubmitPduBase)
SmsMessageBase::SubmitPduBase::~SubmitPduBase()
{}

ECode SmsMessageBase::SubmitPduBase::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("SubmitPdu: encodedScAddress = ")
            + Arrays::ToString(mEncodedScAddress)
            + ", encodedMessage = "
            + Arrays::ToString(mEncodedMessage);
    return NOERROR;
}

/**
 * Base class declaring the specific methods and members for SmsMessage.
 * {@hide}
 */
CAR_INTERFACE_IMPL(SmsMessageBase, Object, ISmsMessageBase)

SmsMessageBase::SmsMessageBase()
    : mStatusOnIcc(-1)
    , mIndexOnIcc(-1)
{
}

ECode SmsMessageBase::GetServiceCenterAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mScAddress;
    return NOERROR;
}

ECode SmsMessageBase::GetOriginatingAddress(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mOriginatingAddress == NULL) {
        *result = String(NULL);
        return NOERROR;
    }

    return mOriginatingAddress->GetAddressString(result);
}

ECode SmsMessageBase::GetDisplayOriginatingAddress(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mIsEmail) {
        *result = mEmailFrom;
        return NOERROR;
    } else {
        return GetOriginatingAddress(result);
    }
}

ECode SmsMessageBase::GetMessageBody(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMessageBody;
    return NOERROR;
}

ECode SmsMessageBase::GetDisplayMessageBody(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mIsEmail) {
        *result = mEmailBody;
        return NOERROR;
    } else {
        return GetMessageBody(result);
    }
}

ECode SmsMessageBase::GetPseudoSubject(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    mPseudoSubject == NULL ? *result = String("") : *result = mPseudoSubject;
    return NOERROR;
}

ECode SmsMessageBase::GetTimestampMillis(
        /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mScTimeMillis;
    return NOERROR;
}

ECode SmsMessageBase::IsEmail(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsEmail;
    return NOERROR;
}

ECode SmsMessageBase::GetEmailBody(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEmailBody;
    return NOERROR;
}

ECode SmsMessageBase::GetEmailFrom(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEmailFrom;
    return NOERROR;
}

ECode SmsMessageBase::GetUserData(
        /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUserData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsMessageBase::GetUserDataHeader(
        /* [out] */ ISmsHeader** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUserDataHeader;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsMessageBase::GetPdu(
        /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPdu;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsMessageBase::GetStatusOnIcc(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusOnIcc;
    return NOERROR;
}

ECode SmsMessageBase::GetIndexOnIcc(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndexOnIcc;
    return NOERROR;
}

void SmsMessageBase::ParseMessageBody()
{
    // originatingAddress could be NULL if this message is from a status
    // report.
    Boolean res;
    if (mOriginatingAddress != NULL && (mOriginatingAddress->CouldBeEmailGateway(&res), res)) {
        ExtractEmailAddressFromMessageBody();
    }
}

void SmsMessageBase::ExtractEmailAddressFromMessageBody()
{
    /* Some carriers may use " /" delimiter as below
     *
     * 1. [x@y][ ]/[subject][ ]/[body]
     * -or-
     * 2. [x@y][ ]/[body]
     */
     AutoPtr<ArrayOf<String> > parts;
     StringUtils::Split(mMessageBody, String("( /)|( )"), 2, (ArrayOf<String>**)&parts);
     if (parts->GetLength() < 2) return;
     mEmailFrom = (*parts)[0];
     mEmailBody = (*parts)[1];
     Elastos::Droid::Provider::Telephony::Mms::IsEmailAddress(mEmailFrom, &mIsEmail);
}

ECode SmsMessageBase::GetRecipientAddress(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mRecipientAddress == NULL) {
        *result = String(NULL);
        return NOERROR;
    }

    return mRecipientAddress->GetAddressString(result);
}

ECode SmsMessageBase::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb;
    sb.Append(String("SMSC address:"));
    sb.Append(mScAddress);
    if (mOriginatingAddress != NULL) {
        String oa;
        sb.Append(String(";sender address:"));
        IObject::Probe(mOriginatingAddress)->ToString(&oa);
        sb.Append(oa);
    }
    if (mRecipientAddress != NULL) {
        String ra;
        IObject::Probe(mRecipientAddress)->ToString(&ra);
        sb.Append(String(";receiver address:"));
        sb.Append(ra);
    }
    sb.Append(String(";MessageBode:"));
    sb.Append(mMessageBody);
    *result = sb.ToString();

    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

