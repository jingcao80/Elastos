
#include "Authenticator.h"
#include "AutoLock.h"

namespace Elastos {
namespace Net {

AutoPtr<Authenticator> Authenticator::sThisAuthenticator;
Object Authenticator::sLock;

CAR_INTERFACE_IMPL(Authenticator, Object, IAuthenticator)

Authenticator::Authenticator()
    : mPort(0)
{}

ECode Authenticator::GetPasswordAuthentication(
    /* [out] */ IPasswordAuthentication** passwordAuthentication)
{
    VALIDATE_NOT_NULL(passwordAuthentication)
    *passwordAuthentication = NULL;
    return NOERROR;
}

ECode Authenticator::GetRequestingPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)

    *port = mPort;
    return NOERROR;
}

ECode Authenticator::GetRequestingSite(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address)

    *address = mAddr;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode Authenticator::GetRequestingPrompt(
    /* [out] */ String* prompt)
{
    VALIDATE_NOT_NULL(prompt)

    *prompt = mPrompt;
    return NOERROR;
}

ECode Authenticator::GetRequestingProtocol(
    /* [out] */ String* protocol)
{
    VALIDATE_NOT_NULL(protocol)

    *protocol = mProtocol;
    return NOERROR;
}

ECode Authenticator::GetRequestingScheme(
    /* [out] */ String* scheme)
{
    VALIDATE_NOT_NULL(scheme)

    *scheme = mScheme;
    return NOERROR;
}

ECode Authenticator::RequestPasswordAuthentication(
    /* [in] */ IInetAddress* rAddr,
    /* [in] */ Int32 rPort,
    /* [in] */ const String& rProtocol,
    /* [in] */ const String& rPrompt,
    /* [in] */ const String& rScheme,
    /* [out] */ IPasswordAuthentication** passwordAuthentication)
{
    VALIDATE_NOT_NULL(passwordAuthentication)

    synchronized(sLock) {
        // SecurityManager sm = System.getSecurityManager();
        // if (sm != null) {
        //     sm.checkPermission(requestPasswordAuthenticationPermission);
        // }
        if (sThisAuthenticator == NULL) {
            *passwordAuthentication = NULL;
            return NOERROR;
        }
        // set the requester info so it knows what it is requesting
        // authentication for
        sThisAuthenticator->mAddr = rAddr;
        sThisAuthenticator->mPort = rPort;
        sThisAuthenticator->mProtocol = rProtocol;
        sThisAuthenticator->mPrompt = rPrompt;
        sThisAuthenticator->mScheme = rScheme;
        sThisAuthenticator->mRt = AuthenticatorRequestorType_SERVER;

        // returns the authentication info obtained by the registered
        // Authenticator
        return sThisAuthenticator->GetPasswordAuthentication(passwordAuthentication);
    }
    return NOERROR;
}

void Authenticator::SetDefault(
    /* [in] */ IAuthenticator* a)
{
    // SecurityManager sm = System.getSecurityManager();
    // if (sm != null) {
    //     sm.checkPermission(setDefaultAuthenticatorPermission);
    // }
    sThisAuthenticator = (Authenticator*)a;
}

ECode Authenticator::RequestPasswordAuthentication(
    /* [in] */ const String& rHost,
    /* [in] */ IInetAddress* rAddr,
    /* [in] */ Int32 rPort,
    /* [in] */ const String& rProtocol,
    /* [in] */ const String& rPrompt,
    /* [in] */ const String& rScheme,
    /* [out] */ IPasswordAuthentication** passwordAuthentication)
{
    VALIDATE_NOT_NULL(passwordAuthentication)

    synchronized(sLock) {
        // SecurityManager sm = System.getSecurityManager();
        // if (sm != null) {
        //     sm.checkPermission(requestPasswordAuthenticationPermission);
        // }
        if (sThisAuthenticator == NULL) {
            *passwordAuthentication = NULL;
            return NOERROR;
        }
        // set the requester info so it knows what it is requesting
        // authentication for
        sThisAuthenticator->mHost = rHost;
        sThisAuthenticator->mAddr = rAddr;
        sThisAuthenticator->mPort = rPort;
        sThisAuthenticator->mProtocol = rProtocol;
        sThisAuthenticator->mPrompt = rPrompt;
        sThisAuthenticator->mScheme = rScheme;
        sThisAuthenticator->mRt = AuthenticatorRequestorType_SERVER;

        // returns the authentication info obtained by the registered
        // Authenticator
        return sThisAuthenticator->GetPasswordAuthentication(passwordAuthentication);
    }
    return NOERROR;
}

ECode Authenticator::GetRequestingHost(
    /* [out] */ String* hostName)
{
    VALIDATE_NOT_NULL(hostName)

    *hostName = mHost;
    return NOERROR;
}

ECode Authenticator::RequestPasswordAuthentication(
    /* [in] */ const String& rHost,
    /* [in] */ IInetAddress* rAddr,
    /* [in] */ Int32 rPort,
    /* [in] */ const String& rProtocol,
    /* [in] */ const String& rPrompt,
    /* [in] */ const String& rScheme,
    /* [in] */ IURL* rURL,
    /* [in] */ AuthenticatorRequestorType reqType,
    /* [out] */ IPasswordAuthentication** passwordAuthentication)
{
    VALIDATE_NOT_NULL(passwordAuthentication)

    // SecurityManager sm = System.getSecurityManager();
    // if (null != sm) {
    //     sm.checkPermission(requestPasswordAuthenticationPermission);
    // }
    if (sThisAuthenticator == NULL) {
        *passwordAuthentication = NULL;
        return NOERROR;
    }
    // sets the requester info so it knows what it is requesting
    // authentication for
    sThisAuthenticator->mHost = rHost;
    sThisAuthenticator->mAddr = rAddr;
    sThisAuthenticator->mPort = rPort;
    sThisAuthenticator->mProtocol = rProtocol;
    sThisAuthenticator->mPrompt = rPrompt;
    sThisAuthenticator->mScheme = rScheme;
    sThisAuthenticator->mUrl = rURL;
    sThisAuthenticator->mRt = reqType;
    sThisAuthenticator->mRt = AuthenticatorRequestorType_SERVER;

    // returns the authentication info obtained by the registered
    // Authenticator
    return sThisAuthenticator->GetPasswordAuthentication(passwordAuthentication);
}

ECode Authenticator::GetRequestingURL(
    /* [out] */ IURL** url)
{
    VALIDATE_NOT_NULL(url)

    *url = mUrl;
    REFCOUNT_ADD(*url);
    return NOERROR;
}

ECode Authenticator::GetRequestorType(
    /* [out] */ AuthenticatorRequestorType* requestorType)
{
    VALIDATE_NOT_NULL(requestorType)

    *requestorType = mRt;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
