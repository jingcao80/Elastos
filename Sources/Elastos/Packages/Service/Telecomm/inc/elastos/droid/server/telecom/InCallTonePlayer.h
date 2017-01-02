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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEPLAYER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEPLAYER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Core::IRunnable;
using Elastos::Core::Thread;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallAudioManager;
/**
 * Play a call-related tone (ringback, busy signal, etc.) through ToneGenerator. To use, create an
 * instance using InCallTonePlayer.Factory (passing in the TONE_* constant for the tone you want)
 * and start() it. Implemented on top of {@link Thread} so that the tone plays in its own thread.
 */
class InCallTonePlayer
    : public Thread
{
public:
    /**
     * Factory used to create InCallTonePlayers. Exists to aid with testing mocks.
     */
    class Factory
        : public Object
    {
    public:
        CARAPI constructor(
            /* [in] */ CallAudioManager* callAudioManager);

        CARAPI CreatePlayer(
            /* [in] */ Int32 tone,
            /* [out] */ InCallTonePlayer** result);

    private:
        AutoPtr<IWeakReference> mCallAudioManager;
    };

private:
    class SubRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SubRunnable(
            /* [in] */ InCallTonePlayer* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<InCallTonePlayer> mHost;
    };

public:
    /** {@inheritDoc} */
    // @Override
    CARAPI Run();

    CARAPI StartTone();

    /**
     * Stops the tone.
     */
    CARAPI StopTone();

private:
    /**
     * Initializes the tone player. Private; use the {@link Factory} to create tone players.
     *
     * @param toneId ID of the tone to play, see TONE_* constants.
     */
    InCallTonePlayer(
        /* [in] */ Int32 toneId,
        /* [in] */ CallAudioManager* callAudioManager);

    CARAPI CleanUpTonePlayer();

public:
    // The possible tones that we can play.
    static const Int32 TONE_INVALID;
    static const Int32 TONE_BUSY;
    static const Int32 TONE_CALL_ENDED;
    static const Int32 TONE_OTA_CALL_ENDED;
    static const Int32 TONE_CALL_WAITING;
    static const Int32 TONE_CDMA_DROP;
    static const Int32 TONE_CONGESTION;
    static const Int32 TONE_INTERCEPT;
    static const Int32 TONE_OUT_OF_SERVICE;
    static const Int32 TONE_REDIAL;
    static const Int32 TONE_REORDER;
    static const Int32 TONE_RING_BACK;
    static const Int32 TONE_UNOBTAINABLE_NUMBER;
    static const Int32 TONE_VOICE_PRIVACY;
    static const Int32 TONE_HOLD_RECALL;
    static const Int32 TONE_SUPERVISORY_CH;

private:
    static const Int32 RELATIVE_VOLUME_EMERGENCY;

    static const Int32 RELATIVE_VOLUME_HIPRI;

    static const Int32 RELATIVE_VOLUME_LOPRI;

    // value for a tone is exact duration of the tone itself.
    static const Int32 TIMEOUT_BUFFER_MILLIS;

    // The tone state.
    static const Int32 STATE_OFF;

    static const Int32 STATE_ON;

    static const Int32 STATE_STOPPED;

    /**
     * Keeps count of the number of actively playing tones so that we can notify CallAudioManager
     * when we need focus and when it can be release. This should only be manipulated from the main
     * thread.
     */
    static Int32 sTonesPlaying;

    AutoPtr<IWeakReference> mCallAudioManager;

    AutoPtr<IHandler> mMainThreadHandler;

    /** The ID of the tone to play. */
    const Int32 mToneId;

    /** Current state of the tone player. */
    Int32 mState;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEPLAYER_H__
