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

#include "_Elastos.Droid.Core.h"
#include "elastos/droid/net/PskKeyManager.h"

using Elastosx::Net::Ssl::EIID_IKeyManager;
using Org::Conscrypt::EIID_IPSKKeyManager;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_3(PskKeyManager, Object, IKeyManager, IPSKKeyManager, IPskKeyManager)

ECode PskKeyManager::ChooseServerKeyIdentityHint(
    /* [in] */ ISocket* socket,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    return NOERROR;
}

ECode PskKeyManager::ChooseServerKeyIdentityHint(
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    return NOERROR;
}

ECode PskKeyManager::ChooseClientKeyIdentity(
    /* [in] */ const String& identityHint,
    /* [in] */ ISocket* socket,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("");

    return NOERROR;
}

ECode PskKeyManager::ChooseClientKeyIdentity(
    /* [in] */ const String& identityHint,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("");

    return NOERROR;
}

ECode PskKeyManager::GetKey(
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [in] */ ISocket* socket,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    return NOERROR;
}

ECode PskKeyManager::GetKey(
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
