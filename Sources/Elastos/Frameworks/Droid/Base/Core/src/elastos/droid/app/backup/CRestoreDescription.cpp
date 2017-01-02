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

#include "elastos/droid/app/backup/CRestoreDescription.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

static AutoPtr<IRestoreDescription> NO_MORE_PACKAGES_init()
{
    AutoPtr<IRestoreDescription> res;
    CRestoreDescription::New(CRestoreDescription::NO_MORE_PACKAGES_SENTINEL, 0, (IRestoreDescription**)&res);
    return res;
}

AutoPtr<IRestoreDescription> CRestoreDescription::NO_MORE_PACKAGES = NO_MORE_PACKAGES_init();

const String CRestoreDescription::NO_MORE_PACKAGES_SENTINEL("");

CAR_INTERFACE_IMPL_2(CRestoreDescription, Object, IRestoreDescription, IParcelable)

CAR_OBJECT_IMPL(CRestoreDescription)

CRestoreDescription::CRestoreDescription()
    : mDataType(0)
{
}

CRestoreDescription::CRestoreDescription(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mPackageName);
    in->ReadInt32(&mDataType);
}

ECode CRestoreDescription::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 dataType)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::GetPackageName(
    /* [out] */ String* name)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::GetDataType(
    /* [out] */ Int32* type)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode CRestoreDescription::ToString(
    /* [out] */ String* str)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

// ---------------------------------------
// Parcelable implementation - not used by transport

//@Override
ECode CRestoreDescription::DescribeContents(
    /* [out] */ Int32* contents)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode CRestoreDescription::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
