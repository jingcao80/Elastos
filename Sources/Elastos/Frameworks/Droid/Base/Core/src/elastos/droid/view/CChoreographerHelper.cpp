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
#include "elastos/droid/view/CChoreographerHelper.h"
#include "elastos/droid/view/Choreographer.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CChoreographerHelper, Singleton, IChoreographerHelper)
CAR_SINGLETON_IMPL(CChoreographerHelper)

ECode CChoreographerHelper::GetInstance(
    /* [out] */ IChoreographer** choreographer)
{
    VALIDATE_NOT_NULL(choreographer)

    AutoPtr<Choreographer> temp = Choreographer::GetInstance();
    *choreographer = temp;
    REFCOUNT_ADD(*choreographer)

    return NOERROR;
}

ECode CChoreographerHelper::GetFrameDelay(
    /* [out] */ Int64* frameDelay)
{
    VALIDATE_NOT_NULL(frameDelay)

    *frameDelay = Choreographer::GetFrameDelay();
    return NOERROR;
}

ECode CChoreographerHelper::SetFrameDelay(
    /* [in] */ Int64 frameDelay)
{
    Choreographer::SetFrameDelay(frameDelay);
    return NOERROR;
}

ECode CChoreographerHelper::SubtractFrameDelay(
    /* [in] */ Int64 delayMillis,
    /* [out] */ Int64* frameDelay)
{
    VALIDATE_NOT_NULL(frameDelay)

    *frameDelay = Choreographer::SubtractFrameDelay(delayMillis);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
