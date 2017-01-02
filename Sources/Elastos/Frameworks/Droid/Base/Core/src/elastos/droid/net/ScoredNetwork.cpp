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

#include "elastos/droid/net/ScoredNetwork.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkKey.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/Objects.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(ScoredNetwork, Object, IParcelable, IScoredNetwork)

ScoredNetwork::ScoredNetwork()
{}

ECode ScoredNetwork::constructor()
{
    return NOERROR;
}

ECode ScoredNetwork::constructor(
    /* [in] */ INetworkKey* networkKey,
    /* [in] */ IRssiCurve* rssiCurve)
{
    mNetworkKey = networkKey;
    mRssiCurve = rssiCurve;
    return NOERROR;
}

ECode ScoredNetwork::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) FUNC_RETURN(TRUE)
    ClassID this_cid, o_cid;
    IObject::Probe(TO_IINTERFACE(this))->GetClassID(&this_cid);
    IObject::Probe(o)->GetClassID(&o_cid);
    if (o == NULL || this_cid != o_cid) FUNC_RETURN(FALSE)
    AutoPtr<ScoredNetwork> that = (ScoredNetwork*) IScoredNetwork::Probe(o);
    *result = Objects::Equals(mNetworkKey, that->mNetworkKey) &&
            Objects::Equals(mRssiCurve, that->mRssiCurve);
    return NOERROR;
}

ECode ScoredNetwork::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    IObject::Probe(mNetworkKey)->GetHashCode(&((*array)[0]));
    IObject::Probe(mRssiCurve)->GetHashCode(&((*array)[1]));
    *result = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode ScoredNetwork::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = String("ScoredNetwork[key=");
    String s;
    IObject::Probe(mNetworkKey)->ToString(&s);
    *result += s;
    *result += ",score=";
    IObject::Probe(mRssiCurve);
    *result += s;
    *result += "]";
    return NOERROR;
}

ECode ScoredNetwork::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    AutoPtr<IInterface> obj;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mNetworkKey = INetworkKey::Probe(obj);
    obj = NULL;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mRssiCurve = IRssiCurve::Probe(obj);
    return NOERROR;
}


ECode ScoredNetwork::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mNetworkKey.Get());
    dest->WriteInterfacePtr(mRssiCurve.Get());
    return NOERROR;
}

ECode ScoredNetwork::GetNetworkKey(
    /* [out] */ INetworkKey** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNetworkKey;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ScoredNetwork::GetRssiCurve(
    /* [out] */ IRssiCurve** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRssiCurve;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
