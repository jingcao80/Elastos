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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_HEADSETMEDIABUTTON_H__
#define __ELASTOS_DROID_SERVER_TELECOM_HEADSETMEDIABUTTON_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <Elastos.Droid.Media.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/media/session/MediaSessionCallback.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::Session::IMediaSession;
using Elastos::Droid::Media::Session::IMediaSessionCallback;
using Elastos::Droid::Media::Session::MediaSessionCallback;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallsManager;
/**
 * Static class to handle listening to the headset media buttons.
 */
class HeadsetMediaButton
    : public CallsManagerListenerBase
{
private:
    class SubMediaSessionCallback
        : public MediaSessionCallback
    {
    public:
        SubMediaSessionCallback(
            /* [in] */ HeadsetMediaButton* host);

        // @Override
        CARAPI OnMediaButtonEvent(
            /* [in] */ IIntent* intent,
            /* [out] */ Boolean* result);

    private:
        HeadsetMediaButton* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ CallsManager* callsManager);

    /** ${inheritDoc} */
    // @Override
    CARAPI OnCallAdded(
        /* [in] */ ICall* call);

    /** ${inheritDoc} */
    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

private:
    /**
     * Handles the wired headset button while in-call.
     *
     * @return true if we consumed the event.
     */
    CARAPI HandleHeadsetHook(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IAudioAttributes>) InitAUDIO_ATTRIBUTES();

public:
    // Types of media button presses
    static const Int32 SHORT_PRESS;

    static const Int32 LONG_PRESS;

private:
    static AutoPtr<IAudioAttributes> AUDIO_ATTRIBUTES;

    AutoPtr<IMediaSessionCallback> mSessionCallback;

    AutoPtr<IWeakReference> mCallsManager; // CallsManager

    AutoPtr<IMediaSession> mSession;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_HEADSETMEDIABUTTON_H__
