
#include "org/apache/http/impl/auth/NTLMScheme.h"
#include "org/apache/http/message/CBufferedHeader.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Auth::IAUTH;
using Org::Apache::Http::Auth::INTCredentials;
using Org::Apache::Http::Message::CBufferedHeader;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

NTLMScheme::NTLMScheme(
    /* [in] */ INTLMEngine* engine)
    : AuthSchemeBase()
{
    if (engine == NULL) {
        Logger::E("NTLMScheme", "NTLM engine may not be null");
        assert(0);
        // throw new IllegalArgumentException("NTLM engine may not be null");
    }
    mEngine = engine;
    mState = UNINITIATED;
    mChallenge = NULL;
}

ECode NTLMScheme::GetSchemeName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("ntlm");
    return NOERROR;
}

ECode NTLMScheme::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ String* param)
{
    VALIDATE_NOT_NULL(param)
    // String parameters not supported
    *param = String(NULL);
    return NOERROR;
}

ECode NTLMScheme::GetRealm(
    /* [out] */ String* realm)
{
    VALIDATE_NOT_NULL(realm)
    *realm = String(NULL);
    return NOERROR;
}

ECode NTLMScheme::IsConnectionBased(
    /* [out] */ Boolean* connectionBased)
{
    VALIDATE_NOT_NULL(connectionBased)
    *connectionBased = TRUE;
    return NOERROR;
}

ECode NTLMScheme::ParseChallenge(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ Int32 pos,
    /* [in] */ Int32 len)
{
    String challenge;
    buffer->SubstringTrimmed(pos, len, &challenge);
    if (challenge.GetLength() == 0) {
        if (mState == UNINITIATED) {
            mState = CHALLENGE_RECEIVED;
        }
        else {
            mState = FAILED;
        }
        mChallenge = String(NULL);
    }
    else {
        mState = MSG_TYPE2_RECEVIED;
        mChallenge = challenge;
    }
    return NOERROR;
}

ECode NTLMScheme::Authenticate(
    /* [in] */ ICredentials* credentials,
    /* [in] */ IHttpRequest* request,
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;

    AutoPtr<INTCredentials> ntcredentials;
    // try {
    ntcredentials = INTCredentials::Probe(credentials);
    if (ntcredentials == NULL) {
        Logger::E("NTLMScheme", "Credentials cannot be used for NTLM authentication");
        return E_INVALID_CREDENTIALS_EXCEPTION;
    }
    // } catch (ClassCastException e) {
    //     throw new InvalidCredentialsException(
    //      "Credentials cannot be used for NTLM authentication: "
    //       + credentials.getClass().getName());
    // }
    String response = String(NULL);
    if (mState == CHALLENGE_RECEIVED || mState == FAILED) {
        String domain, station;
        ntcredentials->GetDomain(&domain);
        ntcredentials->GetWorkstation(&station);
        mEngine->GenerateType1Msg(domain, station, &response);
        mState = MSG_TYPE1_GENERATED;
    }
    else if (mState == MSG_TYPE2_RECEVIED) {
        String userName, password, domain, station;
        ntcredentials->GetUserName(&userName);
        ICredentials::Probe(ntcredentials)->GetPassword(&password);
        ntcredentials->GetDomain(&domain);
        ntcredentials->GetWorkstation(&station);
        mEngine->GenerateType3Msg(userName, password, domain, station,
                mChallenge, &response);
        mState = MSG_TYPE3_GENERATED;
    }
    else {
        Logger::E("NTLMScheme", "Unexpected state: %d", mState);
        return E_AUTHENTICATION_EXCEPTION;
    }
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(32, (ICharArrayBuffer**)&buffer);
    if (IsProxy()) {
        buffer->Append(IAUTH::PROXY_AUTH_RESP);
    }
    else {
        buffer->Append(IAUTH::WWW_AUTH_RESP);
    }
    buffer->Append(String(": NTLM "));
    buffer->Append(response);
    return CBufferedHeader::New(buffer, header);
}

ECode NTLMScheme::IsComplete(
    /* [out] */ Boolean* isComplete)
{
    VALIDATE_NOT_NULL(isComplete)
    *isComplete = mState == MSG_TYPE3_GENERATED || mState == FAILED;
    return NOERROR;
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org