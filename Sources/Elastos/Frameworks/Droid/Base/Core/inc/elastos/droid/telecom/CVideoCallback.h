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

#ifndef __ELASTOS_DROID_TELECOM_CVIDEOCALLBACK_H__
#define __ELASTOS_DROID_TELECOM_CVIDEOCALLBACK_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecom_CVideoCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telecom::IIVideoCallback;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CVideoCallback)
    , public Object
    , public IIVideoCallback
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI ReceiveSessionModifyRequest(
        /* [in] */ IVideoProfile* videoProfile);

    CARAPI ReceiveSessionModifyResponse(
        /* [in] */ Int32 status,
        /* [in] */ IVideoProfile* requestedProfile,
        /* [in] */ IVideoProfile* responseProfile);

    CARAPI HandleCallSessionEvent(
        /* [in] */ Int32 event);

    CARAPI ChangePeerDimensions(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI ChangeCallDataUsage(
        /* [in] */ Int32 dataUsage);

    CARAPI ChangeCameraCapabilities(
        /* [in] */ ICameraCapabilities* cameraCapabilities);

public:
    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CVIDEOCALLBACK_H__