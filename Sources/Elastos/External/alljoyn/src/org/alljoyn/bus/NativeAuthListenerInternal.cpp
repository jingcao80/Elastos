
#include "org/alljoyn/bus/NativeAuthListenerInternal.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeAuthListenerInternal::NativeAuthListenerInternal(
    /* [in] */ NativeBusAttachment* ba,
    /* [in] */ IAuthListenerInternal* listener)
    : mBusPtr(ba)
{
    mBusPtr->IncRef();

    IWeakReferenceSource::Probe(listener)->GetWeakReferce((IWeakReference**)&mAuthListener);
}

NativeAuthListenerInternal::~NativeAuthListenerInternal()
{
    mBusPtr->DecRef();
    mBusPtr = NULL;
}

bool NativeAuthListenerInternal::RequestCredentials(
    /* [in] */ const char* authMechanism,
    /* [in] */ const char* authPeer,
    /* [in] */ uint16_t authCount,
    /* [in] */ const char* userName,
    /* [in] */ uint16_t credMask,
    /* [in] */ Credentials& credentials)
{
    /*
     * Take the authentication changed lock to prevent clients from changing the
     * authListener out from under us while we are calling out into it.
     */
    {
        AutoLock lock(mBusPtr->mBaAuthenticationChangeLock);

        AutoPtr<IAuthListenerInternal> al;
        mAuthListener->Resolve(EIID_IAuthListenerInternal, (IInterface**)&al);
        if (al == NULL) {
            Logger::E("NativeAuthListenerInternal", "RequestCredentials(): Can't get new local reference to AuthListener");
            return false;
        }

        al->RequestCredentials(
            String(authMechanism), String(authPeer), authCount, String(userName), )
    }
}

bool NativeAuthListenerInternal::VerifyCredentials(
    /* [in] */ const char* authMechanism,
    /* [in] */ const char* peerName,
    /* [in] */ const Credentials& credentials);

void NativeAuthListenerInternal::SecurityViolation(
    /* [in] */ QStatus status,
    /* [in] */ const ajn::Message& msg);

void NativeAuthListenerInternal::AuthenticationComplete(
    /* [in] */ const char* authMechanism,
    /* [in] */ const char* peerName,
    /* [in] */ bool success);

private:
    NativeAuthListenerInternal(const NativeAuthListenerInternal& other);
    NativeAuthListenerInternal& operator =(const NativeAuthListenerInternal& other);

private:
    NativeBusAttachment* mBusPtr;
    AutoPtr<IWeakReference> mAuthListener;

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
