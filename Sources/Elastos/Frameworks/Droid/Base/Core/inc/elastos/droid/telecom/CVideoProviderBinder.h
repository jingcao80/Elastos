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

#ifndef __ELASTOS_DROID_TELECOM_CVIDEOPROVIDERBINDER_H__
#define __ELASTOS_DROID_TELECOM_CVIDEOPROVIDERBINDER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecom_CVideoProviderBinder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Internal::Telecom::IIVideoProvider;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CVideoProviderBinder)
    , public Object
    , public IIVideoProvider
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI SetVideoCallback(
        /* [in] */ IBinder* videoCallbackBinder);

    CARAPI SetCamera(
        /* [in] */ const String& cameraId);

    CARAPI SetPreviewSurface(
        /* [in] */ ISurface* surface);

    CARAPI SetDisplaySurface(
        /* [in] */ ISurface* surface);

    CARAPI SetDeviceOrientation(
        /* [in] */ Int32 rotation);

    CARAPI SetZoom(
        /* [in] */ Float value);

    CARAPI SendSessionModifyRequest(
        /* [in] */ IVideoProfile* requestProfile);

    CARAPI SendSessionModifyResponse(
        /* [in] */ IVideoProfile* responseProfile);

    CARAPI RequestCameraCapabilities();

    CARAPI RequestCallDataUsage();

    CARAPI SetPauseImage(
        /* [in] */ const String& uri);

public:
    AutoPtr<IHandler> mMessageHandler;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CVIDEOPROVIDERBINDER_H__