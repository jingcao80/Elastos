
#include "elastos/droid/internal/telephony/SmsMessageBase.h"

using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Utility::IArrayList;

using Elastos::Droid::Provider::ITelephony;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

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

//TODO
// TODO(): This class is duplicated in SmsMessage.java. Refactor accordingly.
//public static abstract class SubmitPduBase  {
//    public Byte[] encodedScAddress; // Null if not applicable.
//    public Byte[] encodedMessage;
//
//    //@Override
//    CARAPI ToString(
//    /* [out] */ String* str)
//{
//    return "SubmitPdu: encodedScAddress = "
//            + Arrays->ToString(encodedScAddress)
//            + ", encodedMessage = "
//            + Arrays->ToString(encodedMessage);
//    }
//}

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
    assert(0 && "TODO");
//    // originatingAddress could be NULL if this message is from a status
//    // report.
//    if (mOriginatingAddress != NULL && mOriginatingAddress->CouldBeEmailGateway()) {
//        ExtractEmailAddressFromMessageBody();
//    }
}

void SmsMessageBase::ExtractEmailAddressFromMessageBody()
{
    assert(0 && "TODO");
//    /* Some carriers may use " /" delimiter as below
//     *
//     * 1. [x@y][ ]/[subject][ ]/[body]
//     * -or-
//     * 2. [x@y][ ]/[body]
//     */
//     String[] parts = mMessageBody->Split("( /)|( )", 2);
//     if (parts.length < 2) return;
//     mEmailFrom = parts[0];
//     mEmailBody = parts[1];
//     mIsEmail = Telephony.Mms->IsEmailAddress(mEmailFrom);
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

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

