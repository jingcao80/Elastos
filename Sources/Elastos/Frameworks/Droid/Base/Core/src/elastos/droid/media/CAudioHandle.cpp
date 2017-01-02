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

#include "elastos/droid/media/CAudioHandle.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioHandle, Object, IAudioHandle)

CAR_OBJECT_IMPL(CAudioHandle)

CAudioHandle::CAudioHandle()
    : mId(0)
{
}

CAudioHandle::~CAudioHandle()
{
}

ECode CAudioHandle::constructor(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CAudioHandle::Id(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode CAudioHandle::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode CAudioHandle::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (o == NULL || IAudioHandle::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CAudioHandle> ah = (CAudioHandle*)(IObject*)o;
    *result = mId == ah->mId;
    return NOERROR;
}

ECode CAudioHandle::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = StringUtils::ToString(mId);
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
