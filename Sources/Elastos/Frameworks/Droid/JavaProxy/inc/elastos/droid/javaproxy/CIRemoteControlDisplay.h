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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteControlDisplay.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Media::IIRemoteControlDisplay;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteControlDisplay)
    , public Object
    , public IIRemoteControlDisplay
    , public IJavaNativeProxy
    , public IBinder
{
public:
    ~CIRemoteControlDisplay();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI GetJavaInstance(
        /* [out] */ HANDLE* obj);

    CARAPI SetCurrentClientId(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ IPendingIntent* clientMediaIntent,
        /* [in] */ Boolean clearing);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetPlaybackState(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 state,
        /* [in] */ Int64 stateChangeTimeMs,
        /* [in] */ Int64 currentPosMs,
        /* [in] */ Float speed);

    CARAPI SetTransportControlInfo(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 transportControlFlags,
        /* [in] */ Int32 posCapabilities);

    CARAPI SetMetadata(
        /* [in] */ Int32 generationId,
        /* [in] */ IBundle* metadata);

    CARAPI SetArtwork(
        /* [in] */ Int32 generationId,
        /* [in] */ IBitmap* artwork);

    CARAPI SetAllMetadata(
        /* [in] */ Int32 generationId,
        /* [in] */ IBundle* metadata,
        /* [in] */ IBitmap* artwork);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__
