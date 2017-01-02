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

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CResultInfo.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CResultInfo, Object, IResultInfo, IParcelable)

CAR_OBJECT_IMPL(CResultInfo)

CResultInfo::CResultInfo()
    : mRequestCode(0)
    , mResultCode(0)
{}

CResultInfo::~CResultInfo()
{}

ECode CResultInfo::constructor()
{
    return NOERROR;
}

ECode CResultInfo::constructor(
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    mResultWho = resultWho;
    mRequestCode = requestCode;
    mResultCode = resultCode;
    mData = data;
    return NOERROR;
}

ECode CResultInfo::GetResultWho(
    /* [out] */ String* who)
{
    VALIDATE_NOT_NULL(who);
    *who = mResultWho;
    return NOERROR;
}

ECode CResultInfo::SetResultWho(
    /* [in] */ const String& who)
{
    mResultWho = who;
    return NOERROR;
}

ECode CResultInfo::GetRequestCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    *code = mRequestCode;
    return NOERROR;
}

ECode CResultInfo::SetRequestCode(
    /* [in] */ Int32 code)
{
    mRequestCode = code;
    return NOERROR;
}

ECode CResultInfo::GetResultCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    *code = mResultCode;
    return NOERROR;
}

ECode CResultInfo::SetResultCode(
    /* [in] */ Int32 code)
{
    mResultCode = code;
    return NOERROR;
}

ECode CResultInfo::GetData(
    /* [out] */ IIntent** data)
{
    VALIDATE_NOT_NULL(data);
    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CResultInfo::SetData(
    /* [in] */ IIntent* data)
{
    mData = data;
    return NOERROR;
}

ECode CResultInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadString(&mResultWho);
    source->ReadInt32(&mRequestCode);
    source->ReadInt32(&mResultCode);
    source->ReadInterfacePtr((Handle32*)&mData);
    return NOERROR;
}

ECode CResultInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteString(mResultWho);
    dest->WriteInt32(mRequestCode);
    dest->WriteInt32(mResultCode);
    dest->WriteInterfacePtr(mData.Get());
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
