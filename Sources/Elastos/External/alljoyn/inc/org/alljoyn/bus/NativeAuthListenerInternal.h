
#ifndef __ORG_ALLJOYN_BUS_NATIVEAUTHLISTENERINTERNAL_H__
#define __ORG_ALLJOYN_BUS_NATIVEAUTHLISTENERINTERNAL_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/AuthListener.h>
#include <alljoyn/Message.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment;

class NativeAuthListenerInternal : public ajn::AuthListener
{
public:
    NativeAuthListenerInternal(
        /* [in] */ NativeBusAttachment* ba,
        /* [in] */ IAuthListenerInternal* listener);

    ~NativeAuthListenerInternal();

    bool RequestCredentials(
        /* [in] */ const char* authMechanism,
        /* [in] */ const char* authPeer,
        /* [in] */ uint16_t authCount,
        /* [in] */ const char* userName,
        /* [in] */ uint16_t credMask,
        /* [in] */ Credentials& credentials);

    bool VerifyCredentials(
        /* [in] */ const char* authMechanism,
        /* [in] */ const char* peerName,
        /* [in] */ const Credentials& credentials);

    void SecurityViolation(
        /* [in] */ QStatus status,
        /* [in] */ const ajn::Message& msg);

    void AuthenticationComplete(
        /* [in] */ const char* authMechanism,
        /* [in] */ const char* peerName,
        /* [in] */ bool success);

private:
    NativeAuthListenerInternal(const NativeAuthListenerInternal& other);
    NativeAuthListenerInternal& operator =(const NativeAuthListenerInternal& other);

private:
    NativeBusAttachment* mBusPtr;
    AutoPtr<IWeakReference> mAuthListener;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEAUTHLISTENERINTERNAL_H__
