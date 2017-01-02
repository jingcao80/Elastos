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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/pm/KeySet.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(KeySet, Object, IKeySet, IParcelable)

ECode KeySet::constructor(
    /* [in] */ IBinder* token)
{
    mToken = token;
    return NOERROR;
}

ECode KeySet::constructor()
{
    return NOERROR;
}

KeySet::~KeySet()
{
}

ECode KeySet::GetToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token)
    *token = mToken;
    REFCOUNT_ADD(*token)
    return NOERROR;
}

ECode KeySet::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IKeySet* ks = IKeySet::Probe(other);
    if (ks) {
        *result = mToken.Get() == ((KeySet*)ks)->mToken.Get();
    }
    return NOERROR;
}

ECode KeySet::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = Object::GetHashCode(mToken);
    return NOERROR;
}

ECode KeySet::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInterfacePtr((Handle32*)&mToken);
}

ECode KeySet::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInterfacePtr(mToken);
}


} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos