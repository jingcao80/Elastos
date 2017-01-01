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

#include "org/alljoyn/bus/AuthListener.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(Credentials, Object, ICredentials)

Credentials::Credentials()
{}

ECode Credentials::GetPassword(
    /* [out, callee] */ ArrayOf<Byte>** password)
{
    VALIDATE_NOT_NULL(password)
    *password = mPassword;
    REFCOUNT_ADD(*password)
    return NOERROR;
}

ECode Credentials::GetUserName(
    /* [out] */ String* userName)
{
    VALIDATE_NOT_NULL(userName)
    *userName = mUserName;
    return NOERROR;
}

ECode Credentials::GetCertificateChain(
    /* [out] */ String* certificateChain)
{
    VALIDATE_NOT_NULL(certificateChain)
    *certificateChain = mCertificateChain;
    return NOERROR;
}

ECode Credentials::GetPrivateKey(
    /* [out] */ String* privateKey)
{
    VALIDATE_NOT_NULL(privateKey)
    *privateKey = mPrivateKey;
    return NOERROR;
}

ECode Credentials::GetLogonEntry(
    /* [out, callee] */ ArrayOf<Byte>** logonEntry)
{
    VALIDATE_NOT_NULL(logonEntry)
    *logonEntry = mLogonEntry;
    REFCOUNT_ADD(*logonEntry)
    return NOERROR;
}

ECode Credentials::GetExpiration(
    /* [out] */ IInteger32** expiration)
{
    VALIDATE_NOT_NULL(expiration)
    *expiration = mExpiration;
    REFCOUNT_ADD(*expiration)
    return NOERROR;
}

CAR_INTERFACE_IMPL(AuthRequest, Object, IAuthRequest)

CAR_INTERFACE_IMPL(PasswordRequest, AuthRequest, IPasswordRequest)

PasswordRequest::PasswordRequest(
    /* [in] */ Credentials* credentials,
    /* [in] */ Boolean isNew,
    /* [in] */ Boolean isOneTime)
    : mIsNew(isNew)
    , mIsOneTime(isOneTime)
{
    mCredentials = credentials;
}

ECode PasswordRequest::IsNewPassword(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mIsNew;
    return NOERROR;
}

ECode PasswordRequest::IsOneTimePassword(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mIsOneTime;
    return NOERROR;
}

ECode PasswordRequest::SetPassword(
    /* [in] */ ArrayOf<Char32>* password)
{
    mCredentials->mPassword = CBusAttachment::Encode(password);
    return NOERROR;
}

CAR_INTERFACE_IMPL(UserNameRequest, AuthRequest, IUserNameRequest)

UserNameRequest::UserNameRequest(
    /* [in] */ Credentials* credentials)
{
    mCredentials = credentials;
}

ECode UserNameRequest::SetUserName(
    /* [in] */ const String& userName)
{
    mCredentials->mUserName = userName;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CertificateRequest, AuthRequest, ICertificateRequest)

CertificateRequest::CertificateRequest(
    /* [in] */ Credentials* credentials)
{
    mCredentials = credentials;
}

ECode CertificateRequest::SetCertificateChain(
    /* [in] */ const String& certificateChain)
{
    mCredentials->mCertificateChain = certificateChain;
    return NOERROR;
}

CAR_INTERFACE_IMPL(PrivateKeyRequest, AuthRequest, IPrivateKeyRequest)

PrivateKeyRequest::PrivateKeyRequest(
    /* [in] */ Credentials* credentials)
{
    mCredentials = credentials;
}

ECode PrivateKeyRequest::SetPrivateKey(
    /* [in] */ const String& privateKey)
{
    mCredentials->mPrivateKey = privateKey;
    return NOERROR;
}

CAR_INTERFACE_IMPL(LogonEntryRequest, AuthRequest, ILogonEntryRequest)

LogonEntryRequest::LogonEntryRequest(
    /* [in] */ Credentials* credentials)
{
    mCredentials = credentials;
}

ECode LogonEntryRequest::SetLogonEntry(
    /* [in] */ ArrayOf<Char32>* logonEntry)
{
    mCredentials->mLogonEntry = CBusAttachment::Encode(logonEntry);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ExpirationRequest, AuthRequest, IExpirationRequest)

ExpirationRequest::ExpirationRequest(
    /* [in] */ Credentials* credentials)
{
    mCredentials = credentials;
}

ECode ExpirationRequest::SetExpiration(
    /* [in] */ Int32 expiration)
{
    mCredentials->mExpiration = CoreUtils::Convert(expiration);
    return NOERROR;
}

CAR_INTERFACE_IMPL(VerifyRequest, AuthRequest, IVerifyRequest)

VerifyRequest::VerifyRequest(
    /* [in] */ const String& certificateChain)
{
    mCertificateChain = certificateChain;
}

ECode VerifyRequest::GetCertificateChain(
    /* [out] */ String* certificateChain)
{
    VALIDATE_NOT_NULL(certificateChain)
    *certificateChain = mCertificateChain;
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
