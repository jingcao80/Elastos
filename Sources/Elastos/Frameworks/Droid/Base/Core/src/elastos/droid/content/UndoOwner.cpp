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

#include "elastos/droid/content/UndoOwner.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(UndoOwner, Object, IUndoOwner)

UndoOwner::UndoOwner()
    : mOpCount(0)
    , mStateSeq(0)
    , mSavedIdx(0)
{}

UndoOwner::~UndoOwner()
{}

ECode UndoOwner::constructor(
    /* [in] */ const String& tag)
{
    mTag = tag;
    return NOERROR;
}

ECode UndoOwner::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag)
    *tag = mTag;
    return NOERROR;
}

ECode UndoOwner::GetData(
    /* [out] */ IInterface** data)
{
    VALIDATE_NOT_NULL(data)
    *data = mData;
    REFCOUNT_ADD(*data)
    return NOERROR;
}


}
}
}
