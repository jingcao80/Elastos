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
#include "elastos/droid/media/projection/CMediaProjectionInfo.h"
#include <elastos/utility/Objects.h>

using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

CAR_INTERFACE_IMPL_2(CMediaProjectionInfo, Object, IMediaProjectionInfo, IParcelable)

CAR_OBJECT_IMPL(CMediaProjectionInfo)

CMediaProjectionInfo::CMediaProjectionInfo()
    : mPackageName(String(NULL))
{
}

CMediaProjectionInfo::~CMediaProjectionInfo()
{
}

ECode CMediaProjectionInfo::constructor()
{
    return NOERROR;
}

ECode CMediaProjectionInfo::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle * handle)
{
    mPackageName = packageName;
    mUserHandle = handle;
    return NOERROR;
}

ECode CMediaProjectionInfo::GetPackageName(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPackageName;
    return NOERROR;
}

ECode CMediaProjectionInfo::GetUserHandle(
    /* [out] */ IUserHandle ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUserHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaProjectionInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadString(&mPackageName);
    source->ReadInterfacePtr((HANDLE*)(IInterface**)&obj);
    mUserHandle = IUserHandle::Probe(obj);
    return NOERROR;
}

ECode CMediaProjectionInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPackageName);
    dest->WriteInterfacePtr(mUserHandle);
    return NOERROR;
}

ECode CMediaProjectionInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    IMediaProjectionInfo* o = IMediaProjectionInfo::Probe(obj);
    if (o == (IMediaProjectionInfo*)this) {
        CMediaProjectionInfo* other = (CMediaProjectionInfo*)o;
        *result = mPackageName.Equals(other->mPackageName)
            && Objects::Equals(mUserHandle, other->mUserHandle);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode CMediaProjectionInfo::GetHashCode(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    // return Objects.hash(mPackageName, mUserHandle);
    return E_NOT_IMPLEMENTED;
}

ECode CMediaProjectionInfo::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("MediaProjectionInfo{mPackageName=") + mPackageName;
         // + ", mUserHandle=" + mUserHandle + "}";
    return NOERROR;
}

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos
