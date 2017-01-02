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

#include "elastos/droid/graphics/CAtlasEntry.h"

namespace Elastos {
namespace Droid {
namespace Graphics {


CAR_OBJECT_IMPL(CAtlasEntry)

CAR_INTERFACE_IMPL(CAtlasEntry, Object, IAtlasEntry)

CAtlasEntry::CAtlasEntry()
    : mX(0)
    , mY(0)
    , mRotated(FALSE)
{}

ECode CAtlasEntry::constructor()
{
    return NOERROR;
}

ECode CAtlasEntry::SetX(
    /* [in] */ Int32 value)
{
    mX = value;
    return NOERROR;
}

ECode CAtlasEntry::GetX(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mX;
    return NOERROR;
}

ECode CAtlasEntry::SetY(
    /* [in] */ Int32 value)
{
    mY = value;
    return NOERROR;
}

ECode CAtlasEntry::GetY(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mY;
    return NOERROR;
}

ECode CAtlasEntry::SetRotated(
    /* [in] */ Boolean value)
{
    mRotated = value;
    return NOERROR;
}

ECode CAtlasEntry::GetRotated(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mRotated;
    return NOERROR;
}


} // namespace Graphics
} // namepsace Droid
} // namespace Elastos