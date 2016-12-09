
#ifndef __ORG_ALLJOYN_BUS_AUTHLISTENER_H__
#define __ORG_ALLJOYN_BUS_AUTHLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::IInteger32;
using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

/** Authentication credentials set via authentication requests. */
class Credentials
    : public Object
    , public ICredentials
{
public:
    Credentials();

    CAR_INTERFACE_DECL()

    CARAPI GetPassword(
        /* [out, callee] */ ArrayOf<Byte>** password);

    CARAPI GetUserName(
        /* [out] */ String* userName);

    CARAPI GetCertificateChain(
        /* [out] */ String* certificateChain);

    CARAPI GetPrivateKey(
        /* [out] */ String* privateKey);

    CARAPI GetLogonEntry(
        /* [out, callee] */ ArrayOf<Byte>** logonEntry);

    CARAPI GetExpiration(
        /* [out] */ IInteger32** expiration);

 public:
    AutoPtr<ArrayOf<Byte> > mPassword;
    String mUserName;
    String mCertificateChain;
    String mPrivateKey;
    AutoPtr<ArrayOf<Byte> > mLogonEntry;
    AutoPtr<IInteger32> mExpiration;
};

/** Authentication request. */
class AuthRequest
    : public Object
    , public IAuthRequest
{
public:
    CAR_INTERFACE_DECL()

protected:
    AuthRequest() {}

protected:
    AutoPtr<Credentials> mCredentials;
};

/** Authentication request for a password, pincode, or passphrase. */
class PasswordRequest
    : public AuthRequest
    , public IPasswordRequest
{
public:
    PasswordRequest(
        /* [in] */ Credentials* credentials,
        /* [in] */ Boolean isNew,
        /* [in] */ Boolean isOneTime);

    CAR_INTERFACE_DECL()

    /**
     * Indicates request is for a newly created password.
     *
     * @return {@code true} if request is for a newly created password
     */
    CARAPI IsNewPassword(
        /* [out] */ Boolean* res);

    /**
     * Indicates a request is for a one time use password.
     *
     * @return {@code true} if request is for a one time use password
     */
    CARAPI IsOneTimePassword(
        /* [out] */ Boolean* res);

    /**
     * Sets a requested password, pincode, or passphrase.
     *
     * @param password the password to set
     */
    CARAPI SetPassword(
        /* [in] */ ArrayOf<Char32>* password);

private:
    Boolean mIsNew;
    Boolean mIsOneTime;
};

/** Authentication request for a user name. */
class UserNameRequest
    : public AuthRequest
    , public IUserNameRequest
{
public:
    UserNameRequest(
        /* [in] */ Credentials* credentials);

    CAR_INTERFACE_DECL()

    /**
     * Sets a requested user name.
     *
     * @param userName the user name to set
     */
    CARAPI SetUserName(
        /* [in] */ const String& userName);
};

/** Authentication request for a chain of PEM-encoded X509 certificates. */
class CertificateRequest
    : public AuthRequest
    , public ICertificateRequest
{
public:
    CertificateRequest(
        /* [in] */ Credentials* credentials);

    CAR_INTERFACE_DECL()

    /**
     * Sets a requested public key certificate chain. The certificate must
     * be PEM encoded.
     *
     * @param certificateChain the certificate to chain to set
     */
    CARAPI SetCertificateChain(
        /* [in] */ const String& certificateChain);
};

/** Authentication request for a PEM encoded private key. */
class PrivateKeyRequest
    : public AuthRequest
    , public IPrivateKeyRequest
{
public:
    PrivateKeyRequest(
        /* [in] */ Credentials* credentials);

    CAR_INTERFACE_DECL()

    /**
     * Sets a requested private key. The private key must be PEM encoded and
     * may be encrypted.
     *
     * @param privateKey the private key to set
     */
    CARAPI SetPrivateKey(
        /* [in] */ const String& privateKey);
};

/** Authentication request for a logon entry. */
class LogonEntryRequest
    : public AuthRequest
    , public ILogonEntryRequest
{
public:
    LogonEntryRequest(
        /* [in] */ Credentials* credentials);

    CAR_INTERFACE_DECL()

    /**
     * Sets a logon entry. For example for the Secure Remote
     * Password protocol in RFC 5054, a logon entry encodes the
     * N,g, s and v parameters. An SRP logon entry has the form
     * N:g:s:v where N,g,s, and v are ASCII encoded hexadecimal
     * strings and are seperated by colons.
     *
     * @param logonEntry the logon entry to set
     */
    CARAPI SetLogonEntry(
        /* [in] */ ArrayOf<Char32>* logonEntry);
};

/** Authentication request to set an expiration time for the credentials. */
class ExpirationRequest
    : public AuthRequest
    , public IExpirationRequest
{
public:
    ExpirationRequest(
        /* [in] */ Credentials* credentials);

    CAR_INTERFACE_DECL()

    /**
     * Sets an expiration time in seconds relative to the current time for
     * the credentials. This value is optional and can be set on any
     * response to a credentials request. After the specified expiration
     * time has elapsed any secret keys based on the provided credentials
     * are invalidated and a new authentication exchange will be
     * required. If an expiration is not set the default expiration time for
     * the requested authentication mechanism is used.
     *
     * @param expiration the expiration time in seconds
     */
    CARAPI SetExpiration(
        /* [in] */ Int32 expiration);
};

/**
 * Authentication request for verification of a certificate chain from a
 * remote peer.
 */
class VerifyRequest
    : public AuthRequest
    , public IVerifyRequest
{
public:
    VerifyRequest(
        /* [in] */ const String& certificateChain);

    CAR_INTERFACE_DECL()

    /**
     * Gets the PEM encoded X509 certificate chain to verify.
     *
     * @return an X509 certificate chain
     */
    CARAPI GetCertificateChain(
        /* [out] */ String* certificateChain);

private:
    String mCertificateChain;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_AUTHLISTENER_H__
