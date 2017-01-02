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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERARESULTEXTRAS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERARESULTEXTRAS_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

class CaptureResultExtras
    : public Object
    , public ICaptureResultExtras
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CaptureResultExtras();

    virtual ~CaptureResultExtras() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 requestId,
        /* [in] */ Int32 subsequenceId,
        /* [in] */ Int32 afTriggerId,
        /* [in] */ Int32 precaptureTriggerId,
        /* [in] */ Int64 frameNumber,
        /* [in] */ Int32 partialResultCount);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetRequestId(
       /* [out] */ Int32* id);

    CARAPI GetSubsequenceId(
        /* [out] */ Int32* id);

    CARAPI GetAfTriggerId(
        /* [out] */ Int32* id);

    CARAPI GetPrecaptureTriggerId(
        /* [out] */ Int32* id);

    CARAPI GetFrameNumber(
        /* [out] */ Int64* number);

    CARAPI GetPartialResultCount(
        /* [out] */ Int32* count);

private:
    Int32 mRequestId;
    Int32 mSubsequenceId;
    Int32 mAfTriggerId;
    Int32 mPrecaptureTriggerId;
    Int64 mFrameNumber;
    Int32 mPartialResultCount;
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CAMERARESULTEXTRAS_H__
