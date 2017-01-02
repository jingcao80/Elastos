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

#include "_Elastos.Droid.Os.h"
#include "elastos/droid/net/UidRange.h"
#include "elastos/droid/net/CUidRange.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(UidRange, Object, IParcelable, IUidRange)

ECode UidRange::constructor()
{
    return NOERROR;
}

ECode UidRange::constructor(
    /* [in] */ Int32 startUid,
    /* [in] */ Int32 stopUid)
{
    if (startUid < 0) {
        Logger::E("UidRange", "Invalid start UID.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (stopUid < 0) {
        Logger::E("UidRange", "Invalid stop UID.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (startUid > stopUid) {
        Logger::E("UidRange", "Invalid UID range.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mStart = startUid;
    mStop  = stopUid;
    return NOERROR;
}

ECode UidRange::CreateForUser(
    /* [in] */ Int32 userId,
    /* [out] */ IUidRange** result)
{
    VALIDATE_NOT_NULL(result)

    return CUidRange::New(userId * IUserHandle::PER_USER_RANGE, (userId + 1) * IUserHandle::PER_USER_RANGE - 1, result);
}

ECode UidRange::GetStartUser(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mStart / IUserHandle::PER_USER_RANGE;
    return NOERROR;
}

ECode UidRange::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = 17;
    *result = 31 * *result + mStart;
    *result = 31 * *result + mStop;
    return NOERROR;
}

ECode UidRange::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) FUNC_RETURN(TRUE)
    if (IUidRange::Probe(o) != NULL) {
        AutoPtr<UidRange> other = (UidRange*) IUidRange::Probe(o);
        *result = mStart == other->mStart && mStop == other->mStop;
        return NOERROR;
    }
    FUNC_RETURN(FALSE)
}

ECode UidRange::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("%d-%d", mStart, mStop);
    *result = rev;
    return NOERROR;
}

ECode UidRange::ReadFromParcel(
        /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mStart);
    parcel->ReadInt32(&mStop);
    return NOERROR;
}

ECode UidRange::WriteToParcel(
        /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mStart);
    dest->WriteInt32(mStop);
    return NOERROR;
}

ECode UidRange::GetStart(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mStart;
    return NOERROR;
}

ECode UidRange::GetStop(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mStop;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
