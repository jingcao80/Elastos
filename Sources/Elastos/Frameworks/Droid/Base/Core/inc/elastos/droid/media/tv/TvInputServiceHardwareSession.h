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

#ifndef __ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICEHardwareSession_H__
#define __ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICEHardwareSession_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/tv/TvInputServiceSession.h"
#include "elastos/droid/media/tv/TvInputManagerSessionCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::Tv::TvInputManagerSessionCallback;
using Elastos::Droid::Media::Tv::TvInputServiceSession;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::ISurface;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Base class for a TV input session which represents an external device connected to a
 * hardware TV input. Once TV input returns an implementation of this class on
 * {@link #onCreateSession(String)}, the framework will create a hardware session and forward
 * the application's surface to the hardware TV input.
 * @see #onCreateSession(String)
 */
class TvInputServiceHardwareSession
    : public TvInputServiceSession
    , public ITvInputServiceHardwareSession
{
private:
    class HardwareSessionCallback
        : public TvInputManagerSessionCallback
    {
    public:
        HardwareSessionCallback(
            /* [in] */ TvInputServiceHardwareSession* host)
            : mHost(host)
        {}

        CARAPI OnSessionCreated(
            /* [in] */ ITvInputManagerSession* session);

        CARAPI OnVideoAvailable(
            /* [in] */ ITvInputManagerSession* session);

        CARAPI OnVideoUnavailable(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ Int32 reason);

    private:
        TvInputServiceHardwareSession* mHost;
    };

public:
    friend class TvInputService;

    TvInputServiceHardwareSession();

    virtual ~TvInputServiceHardwareSession();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a new HardwareSession.
     *
     * @param context The context of the application
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Returns the hardware TV input ID the external device is connected to.
     * <p>
     * TV input is expected to provide {@link android.R.attr#setupActivity} so that
     * the application can launch it before using this TV input. The setup activity may let
     * the user select the hardware TV input to which the external device is connected. The ID
     * of the selected one should be stored in the TV input so that it can be returned here.
     * </p>
     */
    CARAPI GetHardwareInputId(
        /* [out] */ String* result);

    /**
     * Called when the underlying hardware TV input session calls
     * {@link TvInputService.Session#notifyVideoAvailable()}.
     */
    CARAPI OnHardwareVideoAvailable();

    /**
     * Called when the underlying hardware TV input session calls
     * {@link TvInputService.Session#notifyVideoUnavailable(int)}.
     *
     * @param reason The reason that the hardware TV input stopped the playback:
     * <ul>
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_UNKNOWN}
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_TUNING}
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_WEAK_SIGNAL}
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_BUFFERING}
     * </ul>
     */
    CARAPI OnHardwareVideoUnavailable(
        /* [in] */ Int32 reason);

    CARAPI OnSetSurface(
        /* [in] */ ISurface* surface,
        /* [out] */ Boolean* result);

private:
    AutoPtr<ITvInputManagerSession> mHardwareSession;
    AutoPtr<IITvInputSession> mProxySession;
    AutoPtr<IITvInputSessionCallback> mProxySessionCallback;
    AutoPtr<IHandler> mServiceHandler;

    AutoPtr<ITvInputManagerSessionCallback> mHardwareSessionCallback;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICEHardwareSession_H__
