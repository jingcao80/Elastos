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

#include <Elastos.CoreLibrary.Security.h>
#include "elastos/droid/content/pm/CVerifierInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CVerifierInfo, Object, IVerifierInfo, IParcelable)

CAR_OBJECT_IMPL(CVerifierInfo)

CVerifierInfo::CVerifierInfo()
{}

CVerifierInfo::~CVerifierInfo()
{}

ECode CVerifierInfo::constructor()
{
    return NOERROR;
}

ECode CVerifierInfo::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ IPublicKey* publicKey)
{
    if (packageName.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    VALIDATE_NOT_NULL(publicKey);

    mPackageName = packageName;
    mPublicKey = publicKey;

    return NOERROR;
}

ECode CVerifierInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    source->ReadString(&mPackageName);
    source->ReadInterfacePtr((Handle32*)&mPublicKey);
    return NOERROR;
}

ECode CVerifierInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteString(mPackageName);
    dest->WriteInterfacePtr(mPublicKey);
    return NOERROR;
}

ECode CVerifierInfo::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CVerifierInfo::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CVerifierInfo::GetPublicKey(
    /* [out] */ IPublicKey** publicKey)
{
    VALIDATE_NOT_NULL(publicKey);
    *publicKey = mPublicKey;
    REFCOUNT_ADD(*publicKey)
    return NOERROR;
}

ECode CVerifierInfo::SetPublicKey(
    /* [in] */ IPublicKey* publicKey)
{
    mPublicKey = publicKey;
    return NOERROR;
}

}
}
}
}
