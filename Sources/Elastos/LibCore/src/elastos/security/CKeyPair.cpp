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

#include "CKeyPair.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyPair)
CAR_INTERFACE_IMPL_2(CKeyPair, Object, IKeyPair, ISerializable)
ECode CKeyPair::constructor(
    /* [in] */ IPublicKey* publicKey,
    /* [in] */ IPrivateKey* privateKey)
{
    mPrivateKey = privateKey;
    mPublicKey = publicKey;
    return NOERROR;
}

ECode CKeyPair::GetPrivate(
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPrivateKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CKeyPair::GetPublic(
    /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPublicKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}
}
