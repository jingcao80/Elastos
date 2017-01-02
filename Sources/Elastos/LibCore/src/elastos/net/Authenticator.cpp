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

#include "Authenticator.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

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

    {    AutoLock syncLock(sLock);
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

    {    AutoLock syncLock(sLock);
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
