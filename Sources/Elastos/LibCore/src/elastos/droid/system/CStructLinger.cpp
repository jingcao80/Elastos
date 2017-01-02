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

#include "CStructLinger.h"

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructLinger)

CAR_INTERFACE_IMPL(CStructLinger, Object, IStructLinger)

ECode CStructLinger::GetOnoff(
    /* [out] */ Int32* onoff)
{
    VALIDATE_NOT_NULL(onoff);

    *onoff = mOnoff;
    return NOERROR;
}

ECode CStructLinger::GetLinger(
    /* [out] */ Int32* linger)
{
    VALIDATE_NOT_NULL(linger);

    *linger = mLinger;
    return NOERROR;
}

ECode CStructLinger::IsOn(
    /* [out] */ Boolean* isOn)
{
    VALIDATE_NOT_NULL(isOn);

    *isOn = mOnoff != 0;
    return NOERROR;
}

ECode CStructLinger::constructor(
    /* [in] */ Int32 onoff,
    /* [in] */ Int32 linger)
{
    mOnoff = onoff;
    mLinger = linger;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
