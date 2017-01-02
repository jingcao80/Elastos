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

#include "X509ExtendedKeyManager.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL_3(X509ExtendedKeyManager, Object, IX509ExtendedKeyManager, IX509KeyManager, IKeyManager)

X509ExtendedKeyManager::X509ExtendedKeyManager()
{}

ECode X509ExtendedKeyManager::ChooseEngineClientAlias(
    /* [in] */ ArrayOf<String>* keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* alias)
{
    VALIDATE_NOT_NULL(alias)

    *alias = NULL;
    return NOERROR;
}

ECode X509ExtendedKeyManager::ChooseEngineServerAlias(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* alias)
{
    VALIDATE_NOT_NULL(alias)

    *alias = NULL;
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
