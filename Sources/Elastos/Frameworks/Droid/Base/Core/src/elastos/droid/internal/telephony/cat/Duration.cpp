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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/Duration.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                   Duration::InnerParcelableCreator
////=====================================================================
//Duration::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ Duration* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode Duration::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ Duration** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Duration(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode Duration::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ Duration[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Duration[size];
//    assert(0);
//    return NOERROR;
//}

//=====================================================================
//                               Duration
//=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<Duration> > Duration::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(Duration, Object, IDuration, IParcelable);


Duration::Duration()
{
}

ECode Duration::constructor()
{
    return NOERROR;
}

ECode Duration::constructor(
    /* [in] */ Int32 timeInterval,
    /* [in] */ IDurationTimeUnit timeUnit)
{
    mTimeInterval = timeInterval;
    mTimeUnit = timeUnit;
    return NOERROR;
}

ECode Duration::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    dest->WriteInt32(mTimeInterval);
    dest->WriteInt32(mTimeUnit);
    return NOERROR;
}

ECode Duration::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mTimeInterval);
    assert(0 && "TODO");
    // mTimeUnit = TimeUnit.values()[in.readInt()];
    return NOERROR;
}

//ECode Duration::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
