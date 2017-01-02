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

#include "elastos/droid/app/CActivityManagerStackInfo.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerStackInfo, Object, IActivityManagerStackInfo, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerStackInfo)

CActivityManagerStackInfo::CActivityManagerStackInfo()
    : mStackId(0)
    , mDisplayId(0)
{
    CRect::New((IRect**)&mBounds);
}

ECode CActivityManagerStackInfo::constructor()
{
    return NOERROR;
}

ECode CActivityManagerStackInfo::GetStackId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStackId;
    return NOERROR;
}

ECode CActivityManagerStackInfo::SetStackId(
    /* [in] */ Int32 id)
{
    mStackId = id;
    return NOERROR;
}

ECode CActivityManagerStackInfo::GetBounds(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect)
    *rect = mBounds;
    REFCOUNT_ADD(*rect)
    return NOERROR;
}

ECode CActivityManagerStackInfo::SetBounds(
    /* [in] */ IRect* rect)
{
    mBounds = rect;
    return NOERROR;
}

ECode CActivityManagerStackInfo::GetTaskIds(
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    VALIDATE_NOT_NULL(ids)
    *ids = mTaskIds;
    REFCOUNT_ADD(*ids)
    return NOERROR;
}

ECode CActivityManagerStackInfo::SetTaskId(
    /* [in] */ ArrayOf<Int32>* ids)
{
    mTaskIds = ids;
    return NOERROR;
}

ECode CActivityManagerStackInfo::GetTaskNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(mTaskNames)
    *names = mTaskNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode CActivityManagerStackInfo::SetTaskNames(
    /* [in] */ ArrayOf<String>* names)
{
    mTaskNames = names;
    return NOERROR;
}

ECode CActivityManagerStackInfo::GetDisplayId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mDisplayId;
    return NOERROR;
}

ECode CActivityManagerStackInfo::SetDisplayId(
    /* [in] */ Int32 id)
{
    mDisplayId = id;
    return NOERROR;
}

ECode CActivityManagerStackInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    CRect* bounds = (CRect*)mBounds.Get();
    dest->WriteInt32(mStackId);
    dest->WriteInt32(bounds->mLeft);
    dest->WriteInt32(bounds->mTop);
    dest->WriteInt32(bounds->mRight);
    dest->WriteInt32(bounds->mBottom);
    dest->WriteArrayOf((Handle32)mTaskIds.Get());
    dest->WriteArrayOfString(mTaskNames);
    dest->WriteInt32(mDisplayId);
    return NOERROR;
}

ECode CActivityManagerStackInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mStackId);
    Int32 l, r, t, b;
    source->ReadInt32(&l);
    source->ReadInt32(&t);
    source->ReadInt32(&r);
    source->ReadInt32(&b);

    mBounds = NULL;
    CRect::New(l, t, r, b, (IRect**)&mBounds);

    mTaskIds = NULL;
    source->ReadArrayOf((Handle32*)(&mTaskIds));
    source->ReadArrayOfString((ArrayOf<String>**)&mTaskNames);
    source->ReadInt32(&mDisplayId);
    return NOERROR;
}

ECode CActivityManagerStackInfo::ToString(
    /* [in] */ const String& prefix,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb(256);
    sb.Append(prefix); sb.Append(String("Stack id=")); sb.Append(mStackId);
    String rectStr;
    mBounds->ToShortString(&rectStr);
    sb.Append(String(" bounds=")); sb.Append(rectStr);
    sb.Append(String(" displayId=")); sb.Append(mDisplayId);
    sb.Append(String("\n"));

    StringBuilder p(prefix);
    p.Append("  ");
    String newPrefix = p.ToString();
    for (Int32 i = 0; i < mTaskIds->GetLength(); ++i) {
        sb.Append(newPrefix); sb.Append(String("taskId=")); sb.Append((*mTaskIds)[i]);
        sb.Append(String(": ")); sb.Append((*mTaskNames)[i]); sb.Append(String("\n"));
    }
    *str = sb.ToString();
    return NOERROR;
}

ECode CActivityManagerStackInfo::ToString(
    /* [out] */ String* str)
{
    return ToString(String(""), str);
}



}// namespace App
}// namespace Droid
}// namespace Elastos