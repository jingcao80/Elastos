
#ifndef __ORG_ALLJOYN_BUS_MESSAGECONTEXT_H__
#define __ORG_ALLJOYN_BUS_MESSAGECONTEXT_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

class MessageContext
    : public Object
    , public IMessageContext
{
public:
    CAR_INTERFACE_DECL()

    MessageContext()
        : mIsUnreliable(0)
        , mSessionId(0)
        , mSerial(0)
    {}

    MessageContext(
        /* [in] */ Boolean isUnreliable,
        /* [in] */ const String& objectPath,
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& memberName,
        /* [in] */ const String& destination,
        /* [in] */ const String& sender,
        /* [in] */ Int32 sessionId,
        /* [in] */ const String& signature,
        /* [in] */ const String& authMechanism,
        /* [in] */ Int32 serial)
        : mIsUnreliable(isUnreliable)
        , mObjectPath(objectPath)
        , mInterfaceName(interfaceName)
        , mMemberName(memberName)
        , mDestination(destination)
        , mSender(sender)
        , mSessionId(sessionId)
        , mSerial(serial)
        , mSignature(signature)
        , mAuthMechanism(authMechanism)
    {}

    virtual ~MessageContext()
    {}

    CARAPI IsUnreliable(
        /* [out] */ Boolean* value);

    CARAPI SetUnreliable(
        /* [in] */ Boolean value);

    CARAPI SetObjectPath(
        /* [in] */ const String& value);

    CARAPI GetObjectPath(
        /* [out] */ String* value);

    CARAPI SetInterfaceName(
        /* [in] */ const String& value);

    CARAPI GetInterfaceName(
        /* [out] */ String* value);

    CARAPI SetMemberName(
        /* [in] */ const String& value);

    CARAPI GetMemberName(
        /* [out] */ String* value);

    CARAPI SetDestination(
        /* [in] */ const String& value);

    CARAPI GetDestination(
        /* [out] */ String* value);

    CARAPI SetSender(
        /* [in] */ const String& value);

    CARAPI GetSender(
        /* [out] */ String* value);

    CARAPI SetSessionId(
        /* [in] */ Int32 value);

    CARAPI GetSessionId(
        /* [out] */ Int32* value);

    CARAPI SetSerial(
        /* [in] */ Int32 value);

    CARAPI GetSerial(
        /* [out] */ Int32* value);

    CARAPI SetSignature(
        /* [in] */ const String& value);

    CARAPI GetSignature(
        /* [out] */ String* value);

    CARAPI SetAuthMechanism(
        /* [in] */ const String& value);

    CARAPI GetAuthMechanism(
        /* [out] */ String* value);

public:
    /**
     * {@code true} if the message is unreliable.  Unreliable messages have a non-zero
     * time-to-live and may be silently discarded.
     */
    Boolean mIsUnreliable;

    /**
     * The object path for this message.  An empty string if unable to find the
     * AllJoyn object path.
     */
    String mObjectPath;

    /**
     * The interface for this message.  An empty string if unable to find the
     * AllJoyn interface.
     */
    String mInterfaceName;

    /**
     * The member (method/signal) name for this message.  An empty string if
     * unable to find the member name.
     */
    String mMemberName;

    /**
     * The destination for this message.  An empty string if unable to find the
     * message destination.
     */
    String mDestination;

    /**
     * The sender for this message.  An empty string if the message did not
     * specify a sender.
     */
    String mSender;

    /**
     * The session ID that the sender used to send the message.
     */
    Int32 mSessionId;

    /**
     * The serial number of the message.
     */
    Int32 mSerial;

    /**
     * The signature for this message.  An empty string if unable to find the
     * AllJoyn signature.
     */
    String mSignature;

    /**
     * The authentication mechanism in use for this message.
     */
    String mAuthMechanism;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_MESSAGECONTEXT_H__
