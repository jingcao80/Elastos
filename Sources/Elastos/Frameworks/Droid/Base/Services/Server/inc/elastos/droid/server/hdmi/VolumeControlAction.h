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

#ifndef __ELASTOS_DROID_SERVER_HDMI_VOLUMECONTROLACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_VOLUMECONTROLACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

using Elastos::Droid::Media::IAudioManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that transmits volume change to Audio Receiver.
 * <p>
 * This action is created when a user pressed volume up/down. However, Android only provides a
 * listener for delta of some volume change instead of individual key event. Also it's hard to know
 * Audio Receiver's number of volume steps for a single volume control key. Because of this, it
 * sends key-down event until IRT timeout happens, and it will send key-up event if no additional
 * volume change happens; otherwise, it will send again key-down as press and hold feature does.
 */
class VolumeControlAction
    : public HdmiCecFeatureAction
    , public IVolumeControlAction
{
public:
    CAR_INTERFACE_DECL()

    VolumeControlAction();

    /**
     * Scale a custom volume value to cec volume scale.
     *
     * @param volume volume value in custom scale
     * @param scale scale of volume (max volume)
     * @return a volume scaled to cec volume range
     */
    static CARAPI ScaleToCecVolume(
        /* [in] */ Int32 volume,
        /* [in] */ Int32 scale,
        /* [out] */ Int32* result);

    /**
     * Scale a cec volume which is in range of 0 to 100 to custom volume level.
     *
     * @param cecVolume volume value in cec volume scale. It should be in a range of [0-100]
     * @param scale scale of custom volume (max volume)
     * @return a volume scaled to custom volume range
     */
    static CARAPI ScaleToCustomVolume(
        /* [in] */ Int32 cecVolume,
        /* [in] */ Int32 scale,
        /* [out] */ Int32* result);

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress,
        /* [in] */ Boolean isVolumeUp);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

    CARAPI HandleVolumeChange(
        /* [in] */ Boolean isVolumeUp);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Clear();

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 state);

private:
    CARAPI UpdateLastKeyUpdateTime();

    CARAPI SendVolumeKeyPressed();

    CARAPI ResetTimer();

    CARAPI SendVolumeKeyReleased();

    CARAPI_(Boolean) HandleReportAudioStatus(
        /* [in] */ IHdmiCecMessage* cmd);

    CARAPI_(Boolean) ShouldUpdateAudioVolume(
        /* [in] */ Boolean mute);

    CARAPI_(Boolean) HandleFeatureAbort(
        /* [in] */ IHdmiCecMessage* cmd);

private:
    static const String TAG;

    // State that wait for next volume press.
    static const Int32 STATE_WAIT_FOR_NEXT_VOLUME_PRESS;

    static const Int32 MAX_VOLUME;

    static const Int32 UNKNOWN_AVR_VOLUME;

    Int32 mAvrAddress;

    Boolean mIsVolumeUp;

    Int64 mLastKeyUpdateTime;

    Int32 mLastAvrVolume;

    Boolean mSentKeyPressed;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_VOLUMECONTROLACTION_H__
