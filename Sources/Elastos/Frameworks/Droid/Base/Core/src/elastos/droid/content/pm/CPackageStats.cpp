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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CPackageStats.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CPackageStats, Object, IPackageStats, IParcelable)

CAR_OBJECT_IMPL(CPackageStats)

CPackageStats::CPackageStats()
    : mUserHandle(0)
    , mCodeSize(0)
    , mDataSize(0)
    , mCacheSize(0)
    , mExternalCodeSize(0)
    , mExternalDataSize(0)
    , mExternalCacheSize(0)
    , mExternalMediaSize(0)
    , mExternalObbSize(0)
{}

CPackageStats::~CPackageStats()
{}

ECode CPackageStats::constructor()
{
    return NOERROR;
}

ECode CPackageStats::constructor(
    /* [in] */ const String& pkgName)
{
    mPackageName = pkgName;
    // AutoPtr<Elastos::Droid::Os::IUserHandle> userHandle;
    // Elastos::Droid::Os::UserHandle::AcquireSingleton((Elastos::Droid::Os::IUserHandle**)&userHandle);
    // userHandle->myUserId(&mUserHandle);
    return NOERROR;
}

ECode CPackageStats::constructor(
    /* [in] */ const String& pkgName,
    /* [in] */ Int32 userHandle)
{
    mPackageName = pkgName;
    mUserHandle = userHandle;
    return NOERROR;
}

ECode CPackageStats::constructor(
    /* [in] */ IPackageStats * orig)
{
    assert(orig != NULL);
    orig->GetPackageName(&mPackageName);
    orig->GetUserHandle(&mUserHandle);
    orig->GetCodeSize(&mCodeSize);
    orig->GetDataSize(&mDataSize);
    orig->GetCacheSize(&mCacheSize);
    orig->GetExternalCodeSize(&mExternalCodeSize);
    orig->GetExternalDataSize(&mExternalDataSize);
    orig->GetExternalCacheSize(&mExternalCacheSize);
    orig->GetExternalMediaSize(&mExternalMediaSize);
    orig->GetExternalObbSize(&mExternalObbSize);
    return NOERROR;
}

ECode CPackageStats::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // final StringBuilder sb = new StringBuilder("PackageStats{");
    // sb.append(Integer.toHexString(System.identityHashCode(this)));
    // sb.append(" ");
    // sb.append(packageName);
    // if (codeSize != 0) {
    //     sb.append(" code=");
    //     sb.append(codeSize);
    // }
    // if (dataSize != 0) {
    //     sb.append(" data=");
    //     sb.append(dataSize);
    // }
    // if (cacheSize != 0) {
    //     sb.append(" cache=");
    //     sb.append(cacheSize);
    // }
    // if (externalCodeSize != 0) {
    //     sb.append(" extCode=");
    //     sb.append(externalCodeSize);
    // }
    // if (externalDataSize != 0) {
    //     sb.append(" extData=");
    //     sb.append(externalDataSize);
    // }
    // if (externalCacheSize != 0) {
    //     sb.append(" extCache=");
    //     sb.append(externalCacheSize);
    // }
    // if (externalMediaSize != 0) {
    //     sb.append(" media=");
    //     sb.append(externalMediaSize);
    // }
    // if (externalObbSize != 0) {
    //     sb.append(" obb=");
    //     sb.append(externalObbSize);
    // }
    // sb.append("}");
    // return sb.toString();
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPackageStats::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    source->ReadString(&mPackageName);
    source->ReadInt32(&mUserHandle);
    source->ReadInt64(&mCodeSize);
    source->ReadInt64(&mDataSize);
    source->ReadInt64(&mCacheSize);
    source->ReadInt64(&mExternalCodeSize);
    source->ReadInt64(&mExternalDataSize);
    source->ReadInt64(&mExternalCacheSize);
    source->ReadInt64(&mExternalMediaSize);
    source->ReadInt64(&mExternalObbSize);
    return NOERROR;
}

ECode CPackageStats::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    dest->WriteString(mPackageName);
    dest->WriteInt32(mUserHandle);
    dest->WriteInt64(mCodeSize);
    dest->WriteInt64(mDataSize);
    dest->WriteInt64(mCacheSize);
    dest->WriteInt64(mExternalCodeSize);
    dest->WriteInt64(mExternalDataSize);
    dest->WriteInt64(mExternalCacheSize);
    dest->WriteInt64(mExternalMediaSize);
    dest->WriteInt64(mExternalObbSize);
    return NOERROR;
}

ECode CPackageStats::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CPackageStats::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CPackageStats::GetUserHandle(
    /* [out] */ Int32* userHandle)
{
    VALIDATE_NOT_NULL(userHandle);
    *userHandle = mUserHandle;
    return NOERROR;
}

ECode CPackageStats::SetUserHandle(
    /* [in] */ Int32 userHandle)
{
    mUserHandle = userHandle;
    return NOERROR;
}

ECode CPackageStats::GetCodeSize(
    /* [out] */ Int64* codeSize)
{
    VALIDATE_NOT_NULL(codeSize);
    *codeSize = mCodeSize;
    return NOERROR;
}

ECode CPackageStats::SetCodeSize(
    /* [in] */ Int64 codeSize)
{
    mCodeSize = codeSize;
    return NOERROR;
}

ECode CPackageStats::GetDataSize(
    /* [out] */ Int64* dataSize)
{
    VALIDATE_NOT_NULL(dataSize);
    *dataSize = mDataSize;
    return NOERROR;
}

ECode CPackageStats::SetDataSize(
    /* [in] */ Int64 dataSize)
{
    mDataSize = dataSize;
    return NOERROR;
}

ECode CPackageStats::GetCacheSize(
    /* [out] */ Int64* cacheSize)
{
    VALIDATE_NOT_NULL(cacheSize);
    *cacheSize = mCacheSize;
    return NOERROR;
}

ECode CPackageStats::SetCacheSize(
    /* [in] */ Int64 cacheSize)
{
    mCacheSize = cacheSize;
    return NOERROR;
}

ECode CPackageStats::GetExternalCodeSize(
    /* [out] */ Int64* externalCodeSize)
{
    VALIDATE_NOT_NULL(externalCodeSize);
    *externalCodeSize = mExternalCodeSize;
    return NOERROR;
}

ECode CPackageStats::SetExternalCodeSize(
    /* [in] */ Int64 externalCodeSize)
{
    mExternalCodeSize = externalCodeSize;
    return NOERROR;
}

ECode CPackageStats::GetExternalDataSize(
    /* [out] */ Int64* externalDataSize)
{
    VALIDATE_NOT_NULL(externalDataSize);
    *externalDataSize = mExternalDataSize;
    return NOERROR;
}

ECode CPackageStats::SetExternalDataSize(
    /* [in] */ Int64 externalDataSize)
{
    mExternalDataSize = externalDataSize;
    return NOERROR;
}

ECode CPackageStats::GetExternalCacheSize(
    /* [out] */ Int64* externalCacheSize)
{
    VALIDATE_NOT_NULL(externalCacheSize);
    *externalCacheSize = mExternalCacheSize;
    return NOERROR;
}

ECode CPackageStats::SetExternalCacheSize(
    /* [in] */ Int64 externalCacheSize)
{
    mExternalCacheSize = externalCacheSize;
    return NOERROR;
}

ECode CPackageStats::GetExternalMediaSize(
    /* [out] */ Int64* externalMediaSize)
{
    VALIDATE_NOT_NULL(externalMediaSize);
    *externalMediaSize = mExternalMediaSize;
    return NOERROR;
}

ECode CPackageStats::SetExternalMediaSize(
    /* [in] */ Int64 externalMediaSize)
{
    mExternalMediaSize = externalMediaSize;
    return NOERROR;
}

ECode CPackageStats::GetExternalObbSize(
    /* [out] */ Int64* externalObbSize)
{
    VALIDATE_NOT_NULL(externalObbSize);
    *externalObbSize = mExternalObbSize;
    return NOERROR;
}

ECode CPackageStats::SetExternalObbSize(
    /* [in] */ Int64 externalObbSize)
{
    mExternalObbSize = externalObbSize;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
