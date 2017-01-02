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
#include "elastos/droid/content/pm/CPathPermission.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CPathPermission, PatternMatcher, IPathPermission)

CAR_OBJECT_IMPL(CPathPermission)

ECode CPathPermission::constructor()
{
    return PatternMatcher::constructor();
}

ECode CPathPermission::constructor(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 type,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission)
{
    FAIL_RETURN(PatternMatcher::constructor(pattern, type))

    mReadPermission = readPermission;
    mWritePermission = writePermission;
    return NOERROR;
}

ECode CPathPermission::GetReadPermission(
    /* [out] */ String* readPermission)
{
    VALIDATE_NOT_NULL(readPermission);
    *readPermission = mReadPermission;
    return NOERROR;
}

ECode CPathPermission::GetWritePermission(
    /* [out] */ String* writePermission)
{
    VALIDATE_NOT_NULL(writePermission);
    *writePermission = mWritePermission;
    return NOERROR;
}

ECode CPathPermission::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    PatternMatcher::ReadFromParcel(source);
    source->ReadString(&mReadPermission);
    source->ReadString(&mWritePermission);
    return NOERROR;
}

ECode CPathPermission::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    PatternMatcher::WriteToParcel(dest);
    dest->WriteString(mReadPermission);
    dest->WriteString(mWritePermission);
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
