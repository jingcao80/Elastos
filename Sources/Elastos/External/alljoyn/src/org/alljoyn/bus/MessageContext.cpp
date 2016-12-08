
#include "org/alljoyn/bus/MessageContext.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(MessageContext, Object, IMessageContext)

ECode MessageContext::IsUnreliable(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mIsUnreliable;
    return NOERROR;
}

ECode MessageContext::SetUnreliable(
    /* [in] */ Boolean value)
{
    mIsUnreliable = value;
    return NOERROR;
}

ECode MessageContext::SetObjectPath(
    /* [in] */ const String& value)
{
    mObjectPath = value;
    return NOERROR;
}

ECode MessageContext::GetObjectPath(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mObjectPath;
    return NOERROR;
}

ECode MessageContext::SetInterfaceName(
    /* [in] */ const String& value)
{
    mInterfaceName = value;
    return NOERROR;
}

ECode MessageContext::GetInterfaceName(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mInterfaceName;
    return NOERROR;
}

ECode MessageContext::SetMemberName(
    /* [in] */ const String& value)
{
    mMemberName = value;
    return NOERROR;
}

ECode MessageContext::GetMemberName(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mMemberName;
    return NOERROR;
}

ECode MessageContext::SetDestination(
    /* [in] */ const String& value)
{
    mDestination = value;
    return NOERROR;
}

ECode MessageContext::GetDestination(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mDestination;
    return NOERROR;
}

ECode MessageContext::SetSender(
    /* [in] */ const String& value)
{
    mSender = value;
    return NOERROR;
}

ECode MessageContext::GetSender(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSender;
    return NOERROR;
}

ECode MessageContext::SetSessionId(
    /* [in] */ Int32 value)
{
    mSessionId = value;
    return NOERROR;
}

ECode MessageContext::GetSessionId(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSessionId;
    return NOERROR;
}

ECode MessageContext::SetSerial(
    /* [in] */ Int32 value)
{
    mSerial = value;
    return NOERROR;
}

ECode MessageContext::GetSerial(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSerial;
    return NOERROR;
}

ECode MessageContext::SetSignature(
    /* [in] */ const String& value)
{
    mSignature = value;
    return NOERROR;
}

ECode MessageContext::GetSignature(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSignature;
    return NOERROR;
}

ECode MessageContext::SetAuthMechanism(
    /* [in] */ const String& value)
{
    mAuthMechanism = value;
    return NOERROR;
}

ECode MessageContext::GetAuthMechanism(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mAuthMechanism;
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
