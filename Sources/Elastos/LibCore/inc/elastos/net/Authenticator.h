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

#ifndef __ELASTOS_NET_AUTHENTICATOR_H__
#define __ELASTOS_NET_AUTHENTICATOR_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Net {

class Authenticator
    : public Object
    , public IAuthenticator
{
public:
    CAR_INTERFACE_DECL()

    Authenticator();

    virtual CARAPI GetPasswordAuthentication(
        /* [out] */ IPasswordAuthentication** passwordAuthentication);

    virtual CARAPI GetRequestingPort(
        /* [out] */ Int32* port);

    virtual CARAPI GetRequestingSite(
        /* [out] */ IInetAddress** address);

    virtual CARAPI GetRequestingPrompt(
        /* [out] */ String* prompt);

    virtual CARAPI GetRequestingProtocol(
        /* [out] */ String* protocol);

    virtual CARAPI GetRequestingScheme(
        /* [out] */ String* scheme);

    static CARAPI RequestPasswordAuthentication(
        /* [in] */ IInetAddress* rAddr,
        /* [in] */ Int32 rPort,
        /* [in] */ const String& rProtocol,
        /* [in] */ const String& rPrompt,
        /* [in] */ const String& rScheme,
        /* [out] */ IPasswordAuthentication** passwordAuthentication);

    static CARAPI_(void) SetDefault(
        /* [in] */ IAuthenticator* a);

    static CARAPI RequestPasswordAuthentication(
        /* [in] */ const String& rHost,
        /* [in] */ IInetAddress* rAddr,
        /* [in] */ Int32 rPort,
        /* [in] */ const String& rProtocol,
        /* [in] */ const String& rPrompt,
        /* [in] */ const String& rScheme,
        /* [out] */ IPasswordAuthentication** passwordAuthentication);

    static CARAPI RequestPasswordAuthentication(
        /* [in] */ const String& rHost,
        /* [in] */ IInetAddress* rAddr,
        /* [in] */ Int32 rPort,
        /* [in] */ const String& rProtocol,
        /* [in] */ const String& rPrompt,
        /* [in] */ const String& rScheme,
        /* [in] */ IURL* rURL,
        /* [in] */ AuthenticatorRequestorType reqType,
        /* [out] */ IPasswordAuthentication** passwordAuthentication);

    virtual CARAPI GetRequestingHost(
        /* [out] */ String* hostName);

    virtual CARAPI GetRequestingURL(
        /* [out] */ IURL** url);

    virtual CARAPI GetRequestorType(
        /* [out] */ AuthenticatorRequestorType* requestorType);

private:
    // the default authenticator that needs to be set
    static AutoPtr<Authenticator> sThisAuthenticator;
    static Object sLock;

    // the requester connection info
    String mHost;

    AutoPtr<IInetAddress> mAddr;

    Int32 mPort;

    String mProtocol;

    String mPrompt;

    String mScheme;

    AutoPtr<IURL> mUrl;

    AuthenticatorRequestorType mRt;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_AUTHENTICATOR_H__
