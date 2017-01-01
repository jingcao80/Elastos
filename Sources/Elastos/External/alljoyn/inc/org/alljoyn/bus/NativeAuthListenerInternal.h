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
