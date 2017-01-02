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

#include "elastos/droid/content/CUriPermission.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CUriPermission, Object, IUriPermission, IParcelable)

CAR_OBJECT_IMPL(CUriPermission)

CUriPermission::CUriPermission()
    : mModeFlags(0)
    , mPersistedTime(0)
{
}

CUriPermission::~CUriPermission()
{
}

ECode CUriPermission::constructor()
{
    return NOERROR;
}

ECode CUriPermission::constructor(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int64 persistedTime)
{
    mUri = uri;
    mModeFlags = modeFlags;
    mPersistedTime = persistedTime;
    return NOERROR;
}

ECode CUriPermission::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CUriPermission::IsReadPermission(
    /* [out] */ Boolean* permission)
{
    VALIDATE_NOT_NULL(permission)
    *permission = (mModeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0;
    return NOERROR;
}

ECode CUriPermission::IsWritePermission(
    /* [out] */ Boolean* permission)
{
    VALIDATE_NOT_NULL(permission)
    *permission = (mModeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0;
    return NOERROR;
}

ECode CUriPermission::GetPersistedTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mPersistedTime;
    return NOERROR;
}

ECode CUriPermission::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    AutoPtr<IInterface> uri;
    in->ReadInterfacePtr((Handle32*)(IInterface**)&uri);
    mUri = IUri::Probe(uri);
    in->ReadInt32(&mModeFlags);
    in->ReadInt64(&mPersistedTime);
    return NOERROR;
}

ECode CUriPermission::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mUri);
    dest->WriteInt32(mModeFlags);
    dest->WriteInt64(mPersistedTime);
    return NOERROR;
}


} // Content
} // Droid
} // Elastos