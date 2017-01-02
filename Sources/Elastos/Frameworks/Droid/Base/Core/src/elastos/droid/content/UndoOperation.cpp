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

#include "elastos/droid/content/UndoOperation.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(UndoOperation, Object, IUndoOperation, IParcelable)

UndoOperation::UndoOperation()
{}

UndoOperation::~UndoOperation()
{}

ECode UndoOperation::constructor()
{
    return NOERROR;
}

ECode UndoOperation::constructor(
    /* [in] */ IUndoOwner* owner)
{
    mOwner = owner;
    return NOERROR;
}

ECode UndoOperation::GetOwner(
    /* [out] */ IUndoOwner** owner)
{
    VALIDATE_NOT_NULL(owner)
    *owner = mOwner;
    REFCOUNT_ADD(*owner)
    return NOERROR;
}

ECode UndoOperation::GetOwnerData(
    /* [out] */ IInterface** obj)
{
    return mOwner->GetData(obj);
}

ECode UndoOperation::MatchOwner(
    /* [in] */ IUndoOwner* owner,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = owner == mOwner.Get();
    return NOERROR;
}

ECode UndoOperation::HasData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode UndoOperation::AllowMerge(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

}
}
}
