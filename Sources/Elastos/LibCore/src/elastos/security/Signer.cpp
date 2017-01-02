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

#include "Signer.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(Signer, Identity, ISigner)
Signer::~Signer()
{}

Signer::Signer()
{}

ECode Signer::constructor(
    /* [in] */ const String& name)
{
    return Identity::constructor(name);
}

ECode Signer::constructor(
    /* [in] */ const String& name,
    /* [in] */ IIdentityScope* scope) /*throws KeyManagementException*/
{
    return Identity::constructor(name, scope);
}

ECode Signer::GetPrivateKey(
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPrivateKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Signer::SetKeyPair(
    /* [in] */ IKeyPair* pair) /*throws InvalidParameterException, KeyException*/
{
    if (pair == NULL) {
        // throw new NullPointerException("pair == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IPrivateKey> pk;
    pair->GetPrivate((IPrivateKey**)&pk);
    AutoPtr<IPublicKey> k;
    if (pk.Get() == NULL || (pair->GetPublic((IPublicKey**)&k), k.Get()) == NULL) {
        // throw new InvalidParameterException();
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    k = NULL;
    pair->GetPublic((IPublicKey**)&k);
    SetPublicKey(k);
    mPrivateKey = pk;
    return NOERROR;
}

ECode Signer::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb("[Signer]");
    String s;
    GetName(&s);
    sb.Append(s);
    AutoPtr<IIdentityScope> is;
    GetScope((IIdentityScope**)&is);
    if (is != NULL) {
        sb.Append("[");
        sb.Append((IObject::Probe(is)->ToString(&s), s));
        sb.Append("]");
    }
    return sb.ToString(result);
}

} // namespace Security
} // namespace Elastos
