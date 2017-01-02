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

#include "elastos/droid/media/session/CMediaSessionToken.h"
#include <elastos/utility/Objects.h>

using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL_2(CMediaSessionToken, Object, IMediaSessionToken, IParcelable)

CAR_OBJECT_IMPL(CMediaSessionToken)

CMediaSessionToken::CMediaSessionToken()
{
}

CMediaSessionToken::~CMediaSessionToken()
{
}

ECode CMediaSessionToken::constructor()
{
    return NOERROR;
}

ECode CMediaSessionToken::constructor(
    /* [in] */ IISessionController * binder)
{
    mBinder = binder;
    return NOERROR;
}

ECode CMediaSessionToken::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&obj);
    mBinder = IISessionController::Probe(obj);
    return NOERROR;
}

ECode CMediaSessionToken::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mBinder);
    return NOERROR;
}

ECode CMediaSessionToken::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)

    IMediaSessionToken* other = IMediaSessionToken::Probe(obj);
    if ((IMediaSessionToken*)this == other) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    // if (getClass() != obj.getClass()) {
    //     *result = FALSE;
    //     return NOERROR;
    // }
    AutoPtr<CMediaSessionToken> o = (CMediaSessionToken*)other;
    if (mBinder == NULL) {
        if (o->mBinder != NULL) {
            *result = FALSE;
            return NOERROR;
        }
    // } else if (!mBinder.asBinder().equals(other.mBinder.asBinder())) {
    }
    else if (Objects::Equals(mBinder, o->mBinder)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CMediaSessionToken::GetHashCode(
    /* [out] */ Int32 * hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    Int32 prime = 31;
    Int32 result = 1;
    Int32 val;
    if (mBinder == NULL){
        val = 0;
    }
    else {
        IObject::Probe(mBinder)->GetHashCode(&val);
    }
    result = prime * result + val;
    *hashCode = result;
    return NOERROR;
}

ECode CMediaSessionToken::GetBinder(
    /* [out] */ IISessionController ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBinder;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
