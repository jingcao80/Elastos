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

#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERRECOGNITIONEVENT_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERRECOGNITIONEVENT_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Media::IAudioFormat;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

class SoundTriggerRecognitionEvent
    : public Object
    , public ISoundTriggerRecognitionEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    SoundTriggerRecognitionEvent();

    virtual ~SoundTriggerRecognitionEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 status,
        /* [in] */ Int32 soundModelHandle,
        /* [in] */ Boolean captureAvailable,
        /* [in] */ Int32 captureSession,
        /* [in] */ Int32 captureDelayMs,
        /* [in] */ Int32 capturePreambleMs,
        /* [in] */ Boolean triggerInData,
        /* [in] */ IAudioFormat* captureFormat,
        /* [in] */ ArrayOf<Byte>* data);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *equal);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Parcelable.Creator<RecognitionEvent> CREATOR
    //         = new Parcelable.Creator<RecognitionEvent>() {
    //     public RecognitionEvent createFromParcel(Parcel in) {
    //         return RecognitionEvent.fromParcel(in);
    //     }

    //     public RecognitionEvent[] newArray(int size) {
    //         return new RecognitionEvent[size];
    //     }
    // };

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerRecognitionEvent** result);

public:
    /** Recognition status e.g {@link #RECOGNITION_STATUS_SUCCESS} */
    Int32 mStatus;
    /** Sound Model corresponding to this event callback */
    Int32 mSoundModelHandle;
    /** True if it is possible to capture audio from this utterance buffered by the hardware */
    Boolean mCaptureAvailable;
    /** Audio session ID to be used when capturing the utterance with an AudioRecord
     * if captureAvailable() is true. */
    Int32 mCaptureSession;
    /** Delay in ms between end of model detection and start of audio available for capture.
     * A negative value is possible (e.g. if keyphrase is also available for capture) */
    Int32 mCaptureDelayMs;
    /** Duration in ms of audio captured before the start of the trigger. 0 if none. */
    Int32 mCapturePreambleMs;
    /** True if  the trigger (key phrase capture is present in binary data */
    Boolean mTriggerInData;
    /** Audio format of either the trigger in event data or to use for capture of the
      * rest of the utterance */
    AutoPtr<IAudioFormat> mCaptureFormat;
    /** Opaque data for use by system applications who know about voice engine internals,
     * typically during enrollment. */
    AutoPtr<ArrayOf<Byte> > mData;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERRECOGNITIONEVENT_H__
