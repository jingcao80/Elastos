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

#include "elastos/droid/ims/CImsCallProfile.h"
#include "elastos/droid/ims/CImsCallProfileHelper.h"

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL(CImsCallProfileHelper, Singleton, IImsCallProfileHelper)

CAR_SINGLETON_IMPL(CImsCallProfileHelper)

ECode CImsCallProfileHelper::GetVideoStateFromCallType(
    /* [in] */ Int32 callType,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::GetVideoStateFromCallType(callType, result);
}

ECode CImsCallProfileHelper::GetCallTypeFromVideoState(
    /* [in] */ Int32 videoState,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::GetCallTypeFromVideoState(videoState, result);
}

ECode CImsCallProfileHelper::PresentationToOIR(
    /* [in] */ Int32 presentation,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::PresentationToOIR(presentation, result);
}

ECode CImsCallProfileHelper::OIRToPresentation(
    /* [in] */ Int32 oir,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::OIRToPresentation(oir, result);
}

} //namespace Ims
} //namespace Droid
} //namespace Elastos
