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

#include "org/alljoyn/bus/NativeAuthListenerInternal.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeMessageContext.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeAuthListenerInternal::NativeAuthListenerInternal(
    /* [in] */ NativeBusAttachment* ba,
    /* [in] */ IAuthListenerInternal* listener)
    : mBusPtr(ba)
{
    mBusPtr->IncRef();

    IWeakReferenceSource::Probe(listener)->GetWeakReference((IWeakReference**)&mAuthListener);
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
    AutoPtr<ICredentials> cs;
    {
        AutoLock lock(mBusPtr->mBaAuthenticationChangeLock);

        AutoPtr<IAuthListenerInternal> al;
        mAuthListener->Resolve(EIID_IAuthListenerInternal, (IInterface**)&al);
        if (al == NULL) {
            Logger::E("NativeAuthListenerInternal", "RequestCredentials(): Can't get new local reference to AuthListener");
            return false;
        }

        al->RequestCredentials(String(authMechanism), String(authPeer),
                (Int32)authCount, String(userName), (Int32)credMask, (ICredentials**)&cs);
    }

    if (cs == NULL) {
        Logger::E("NativeAuthListenerInternal", "RequestCredentials(): Null return from method");
        return false;
    }

    AutoPtr< ArrayOf<Byte> > password;
    cs->GetPassword((ArrayOf<Byte>**)&password);
    if (password != NULL) {
        credentials.SetPassword(qcc::String((const char*)password->GetPayload(), password->GetLength()));
    }

    String strUserName;
    cs->GetUserName(&strUserName);
    if (!strUserName.IsNull()) {
        credentials.SetUserName(strUserName.string());
    }

    String certificateChain;
    cs->GetCertificateChain(&certificateChain);
    if (!certificateChain.IsNull()) {
        credentials.SetCertChain(certificateChain.string());
    }

    String privateKey;
    cs->GetPrivateKey(&privateKey);
    if (!privateKey.IsNull()) {
        credentials.SetPrivateKey(privateKey.string());
    }

    AutoPtr< ArrayOf<Byte> > logonEntry;
    cs->GetLogonEntry((ArrayOf<Byte>**)&logonEntry);
    if (logonEntry != NULL) {
        credentials.SetLogonEntry(qcc::String((const char*)logonEntry->GetPayload(), logonEntry->GetLength()));
    }

    AutoPtr<IInteger32> expiration;
    cs->GetExpiration((IInteger32**)&expiration);
    if (expiration != NULL) {
        Int32 seconds;
        expiration->GetValue(&seconds);
        credentials.SetExpiration(seconds);
    }

    return true;
}

bool NativeAuthListenerInternal::VerifyCredentials(
    /* [in] */ const char* authMechanism,
    /* [in] */ const char* peerName,
    /* [in] */ const Credentials& credentials)
{
    String userName = credentials.IsSet(AuthListener::CRED_USER_NAME) ?
            String(credentials.GetUserName().c_str()) : String(NULL);

    String cert = credentials.IsSet(AuthListener::CRED_CERT_CHAIN) ?
            String(credentials.GetCertChain().c_str()) : String(NULL);

    ECode ec;
    Boolean acceptable;
    {
        AutoLock lock(mBusPtr->mBaAuthenticationChangeLock);

        AutoPtr<IAuthListenerInternal> al;
        mAuthListener->Resolve(EIID_IAuthListenerInternal, (IInterface**)&al);
        if (al == NULL) {
            Logger::E("NativeAuthListenerInternal", "VerifyCredentials(): Can't get new local reference to AuthListener");
            return false;
        }

        ec = al->VerifyCredentials(String(authMechanism), String(peerName), userName, cert, &acceptable);
    }

    return FAILED(ec) ? false : (bool)acceptable;

}

void NativeAuthListenerInternal::SecurityViolation(
    /* [in] */ QStatus status,
    /* [in] */ const ajn::Message& msg)
{
    NativeMessageContext context(msg);

    AutoLock lock(mBusPtr->mBaAuthenticationChangeLock);

    AutoPtr<IAuthListenerInternal> al;
    mAuthListener->Resolve(EIID_IAuthListenerInternal, (IInterface**)&al);
    if (al == NULL) {
        Logger::E("NativeAuthListenerInternal", "SecurityViolation(): Can't get new local reference to AuthListener");
        return;
    }

    al->SecurityViolation((ECode)status);
}

void NativeAuthListenerInternal::AuthenticationComplete(
    /* [in] */ const char* authMechanism,
    /* [in] */ const char* peerName,
    /* [in] */ bool success)
{
    AutoLock lock(mBusPtr->mBaAuthenticationChangeLock);

    AutoPtr<IAuthListenerInternal> al;
    mAuthListener->Resolve(EIID_IAuthListenerInternal, (IInterface**)&al);
    if (al == NULL) {
        Logger::E("NativeAuthListenerInternal", "AuthenticationComplete(): Can't get new local reference to AuthListener");
        return;
    }

    al->AuthenticationComplete(String(authMechanism), String(peerName), (Boolean)success);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
